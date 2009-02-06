/* vi: set et sw=4 ts=4 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * This file is part of mission-control
 *
 * Copyright (C) 2008 Nokia Corporation. 
 *
 * Contact: Alberto Mardegan  <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <config.h>

#include <dbus/dbus-glib-lowlevel.h>
#include <telepathy-glib/svc-generic.h>
#include <telepathy-glib/gtypes.h>
#include <telepathy-glib/util.h>
#include "mcd-account.h"
#include "mcd-account-priv.h"
#include "mcd-account-requests.h"
#include "mcd-account-manager.h"
#include "mcd-misc.h"
#include "_gen/interfaces.h"

static void
online_request_cb (McdAccount *account, gpointer userdata, const GError *error)
{
    McdChannel *channel = MCD_CHANNEL (userdata);
    McdConnection *connection;

    if (error)
    {
        g_warning ("%s: got error: %s", G_STRFUNC, error->message);
        _mcd_channel_set_error (channel, g_error_copy (error));
        g_object_unref (channel);
        return;
    }
    g_debug ("%s called", G_STRFUNC);
    connection = mcd_account_get_connection (account);
    g_return_if_fail (connection != NULL);
    g_return_if_fail (mcd_connection_get_connection_status (connection)
                      == TP_CONNECTION_STATUS_CONNECTED);

    if (mcd_channel_get_status (channel) == MCD_CHANNEL_STATUS_FAILED)
    {
        g_debug ("%s: channel %p is failed", G_STRFUNC, channel);
        g_object_unref (channel);
        return;
    }

    /* the connection will take ownership of the channel, so the reference we
     * are holding is passed to it */
    mcd_connection_request_channel (connection, channel);
}

static McdChannel *
get_channel_from_request (McdAccount *account, const gchar *request_id)
{
    McdConnection *connection;
    const GList *channels, *list;


    connection = mcd_account_get_connection (account);
    if (connection)
    {
        channels = mcd_operation_get_missions (MCD_OPERATION (connection));
        for (list = channels; list != NULL; list = list->next)
        {
            McdChannel *channel = MCD_CHANNEL (list->data);

            if (g_strcmp0 (_mcd_channel_get_request_path (channel),
                           request_id) == 0)
                return channel;
        }
    }

    /* if we don't have a connection in connected state yet, the channel might
     * be in the online requests queue */
    list = _mcd_account_get_online_requests (account);
    while (list)
    {
        McdOnlineRequestData *data = list->data;

        if (data->callback == online_request_cb)
        {
            McdChannel *channel = MCD_CHANNEL (data->user_data);

            if (g_strcmp0 (_mcd_channel_get_request_path (channel),
                           request_id) == 0)
                return channel;
        }

        list = list->next;
    }
    return NULL;
}

static void
on_channel_status_changed (McdChannel *channel, McdChannelStatus status,
                           McdAccount *account)
{
    const GError *error;

    if (status == MCD_CHANNEL_STATUS_FAILED)
    {
        const gchar *err_string;
        error = _mcd_channel_get_error (channel);
        g_warning ("Channel request %s failed, error: %s",
                   _mcd_channel_get_request_path (channel), error->message);

        err_string = _mcd_get_error_string (error);
        mc_svc_account_interface_channelrequests_emit_failed (account,
            _mcd_channel_get_request_path (channel),
            err_string, error->message);

        g_object_unref (channel);
    }
    else if (status == MCD_CHANNEL_STATUS_DISPATCHED)
    {
        mc_svc_account_interface_channelrequests_emit_succeeded (account,
            _mcd_channel_get_request_path (channel));

        g_object_unref (channel);
    }
}

static McdChannel *
create_request (McdAccount *account, GHashTable *properties,
                guint64 user_time, const gchar *preferred_handler,
                gboolean use_existing, GError **error)
{
    McdChannel *channel;
    GHashTable *props;

    g_return_val_if_fail (error != NULL, NULL);

    /* We MUST deep-copy the hash-table, as we don't know how dbus-glib will
     * free it */
    props = _mcd_deepcopy_asv (properties);
    channel = mcd_channel_new_request (props, user_time,
                                       preferred_handler);
    g_hash_table_unref (props);
    _mcd_channel_set_request_use_existing (channel, use_existing);

    /* we use connect_after, to make sure that other signals (such as
     * RemoveFailedRequest) are emitted before the Failed signal */
    g_signal_connect_after (channel, "status-changed",
                            G_CALLBACK (on_channel_status_changed), account);

    _mcd_account_online_request (account, online_request_cb, channel, error);
    if (*error)
    {
        g_warning ("%s: _mcd_account_online_request: %s", G_STRFUNC,
                   (*error)->message);
        _mcd_channel_set_error (channel, g_error_copy (*error));
        /* no unref here, as this will invoke our handler which will
         * unreference the channel */
        channel = NULL;
    }
    else
    {
        /* the channel must be kept alive until online_request_cb is called;
         * this reference will be removed in that callback */
        g_object_ref (channel);
    }

    return channel;
}

const McdDBusProp account_channelrequests_properties[] = {
    { 0 },
};

static void
account_request_common (McdAccount *account, GHashTable *properties,
                        guint64 user_time, const gchar *preferred_handler,
                        DBusGMethodInvocation *context, gboolean use_existing)
{
    GError *error = NULL;
    const gchar *request_id;
    McdChannel *channel;
    McdDispatcher *dispatcher;

    channel = create_request (account, properties, user_time,
                              preferred_handler, use_existing, &error);
    if (error)
    {
        dbus_g_method_return_error (context, error);
        g_error_free (error);
        return;
    }
    request_id = _mcd_channel_get_request_path (channel);
    g_debug ("%s: returning %s", G_STRFUNC, request_id);
    if (use_existing)
        mc_svc_account_interface_channelrequests_return_from_ensure_channel
            (context, request_id);
    else
        mc_svc_account_interface_channelrequests_return_from_create
            (context, request_id);

    dispatcher = mcd_master_get_dispatcher (mcd_master_get_default ());
    _mcd_dispatcher_add_request (dispatcher, account, channel);
}

static void
account_request_create (McSvcAccountInterfaceChannelRequests *self,
                        GHashTable *properties, guint64 user_time,
                        const gchar *preferred_handler,
                        DBusGMethodInvocation *context)
{
    account_request_common (MCD_ACCOUNT (self), properties, user_time,
                            preferred_handler, context, FALSE);
}

static void
account_request_ensure_channel (McSvcAccountInterfaceChannelRequests *self,
                                GHashTable *properties, guint64 user_time,
                                const gchar *preferred_handler,
                                DBusGMethodInvocation *context)
{
    account_request_common (MCD_ACCOUNT (self), properties, user_time,
                            preferred_handler, context, TRUE);
}

static void
account_request_cancel (McSvcAccountInterfaceChannelRequests *self,
                        const gchar *request_id,
                        DBusGMethodInvocation *context)
{
    GError *error;
    McdChannel *channel;
    McdChannelStatus status;

    g_debug ("%s called for %s", G_STRFUNC, request_id);
    g_return_if_fail (request_id != NULL);
    channel = get_channel_from_request (MCD_ACCOUNT (self), request_id);
    if (!channel)
    {
        error = g_error_new (TP_ERRORS, TP_ERROR_INVALID_ARGUMENT,
                             "Request %s not found", request_id);
        dbus_g_method_return_error (context, error);
        g_error_free (error);
        return;
    }

    status = mcd_channel_get_status (channel);
    g_debug ("channel %p is in status %u", channel, status);
    if (status == MCD_CHANNEL_STATUS_REQUEST ||
        status == MCD_CHANNEL_STATUS_REQUESTED ||
        status == MCD_CHANNEL_STATUS_DISPATCHING)
    {
        g_object_ref (channel);
        error = g_error_new (TP_ERRORS, TP_ERROR_CANCELLED, "Cancelled");
        _mcd_channel_set_error (channel, error);

        /* REQUESTED is a special case: the channel must not be aborted now,
         * because we need to explicitly close the channel object when it will
         * be created by the CM. In that case, mcd_mission_abort() will be
         * called once the Create/EnsureChannel method returns, if the channel
         * is ours */
        if (status != MCD_CHANNEL_STATUS_REQUESTED)
            mcd_mission_abort (MCD_MISSION (channel));

        g_object_unref (channel);
    }
    else
    {
        error = g_error_new (TP_ERRORS, TP_ERROR_NOT_AVAILABLE,
                             "Request %s is not cancellable (%u)",
                             request_id, status);
        dbus_g_method_return_error (context, error);
        g_error_free (error);
        return;
    }

    mc_svc_account_interface_channelrequests_return_from_cancel (context);
}

void
account_channelrequests_iface_init (McSvcAccountInterfaceChannelRequestsClass *iface,
                                    gpointer iface_data)
{
#define IMPLEMENT(x) mc_svc_account_interface_channelrequests_implement_##x (\
    iface, account_request_##x)
    IMPLEMENT(create);
    IMPLEMENT(ensure_channel);
    IMPLEMENT(cancel);
#undef IMPLEMENT
}

