/* vi: set et sw=4 ts=8 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 8 -*- */
/*
 * This file is part of mission-control
 *
 * Copyright (C) 2007 Nokia Corporation. 
 *
 * Contact: Naba Kumar  <naba.kumar@nokia.com>
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

#ifndef __MCD_ACCOUNT_PRIV_H__
#define __MCD_ACCOUNT_PRIV_H__

#include "mcd-account.h"
#include "mcd-account-compat.h"
#include "mcd-account-config.h"
#include "mcd-dbusprop.h"

enum
{
    CONNECTION_STATUS_CHANGED,
    CURRENT_PRESENCE_CHANGED,
    REQUESTED_PRESENCE_CHANGED,
    VALIDITY_CHANGED,
    AVATAR_CHANGED,
    ALIAS_CHANGED,
    CONNECTION_PROCESS,
    PROFILE_SET,
    LAST_SIGNAL
};

extern guint _mcd_account_signals[LAST_SIGNAL];

G_GNUC_INTERNAL void _mcd_account_connect (McdAccount *account,
                                           GHashTable *params);

G_GNUC_INTERNAL gboolean _mcd_account_set_parameters (McdAccount *account,
                                                      GHashTable *params,
                                                      GError **error);
G_GNUC_INTERNAL void _mcd_account_set_parameter (McdAccount *account,
                                                 const gchar *name,
                                                 const GValue *value);

G_GNUC_INTERNAL void _mcd_account_set_current_presence (McdAccount *account,
    TpConnectionPresenceType presence, const gchar *status,
    const gchar *message);

G_GNUC_INTERNAL void _mcd_account_request_temporary_presence (McdAccount *self,
    TpConnectionPresenceType type, const gchar *status);

G_GNUC_INTERNAL GKeyFile *_mcd_account_get_keyfile (McdAccount *account);

G_GNUC_INTERNAL void _mcd_account_set_has_been_online (McdAccount *account);

G_GNUC_INTERNAL void _mcd_account_set_normalized_name (McdAccount *account,
                                                       const gchar *name);

G_GNUC_INTERNAL gboolean _mcd_account_set_avatar (McdAccount *account,
                                                  const GArray *avatar,
                                                  const gchar *mime_type,
                                                  const gchar *token,
                                                  GError **error);
G_GNUC_INTERNAL void _mcd_account_get_avatar (McdAccount *account,
                                              GArray **avatar,
                                              gchar **mime_type);
G_GNUC_INTERNAL void _mcd_account_set_avatar_token (McdAccount *account,
                                                    const gchar *token);
G_GNUC_INTERNAL gchar *_mcd_account_get_avatar_token (McdAccount *account);

G_GNUC_INTERNAL void _mcd_account_set_alias (McdAccount *account,
                                             const gchar *alias);

G_GNUC_INTERNAL gchar *_mcd_account_get_avatar_filename (McdAccount *account);

G_GNUC_INTERNAL void _mcd_account_tp_connection_changed (McdAccount *account);

G_GNUC_INTERNAL void _mcd_account_load (McdAccount *account,
                                        McdAccountLoadCb callback,
                                        gpointer user_data);
G_GNUC_INTERNAL void _mcd_account_set_connection (McdAccount *account,
                                                  McdConnection *connection);

G_GNUC_INTERNAL void _mcd_account_set_connection_status (McdAccount *account,
    TpConnectionStatus status, TpConnectionStatusReason reason);

typedef void (*McdOnlineRequestCb) (McdAccount *account, gpointer userdata,
				    const GError *error);
void _mcd_account_online_request (McdAccount *account,
                                  McdOnlineRequestCb callback,
                                  gpointer userdata);
void _mcd_account_request_connection (McdAccount *account);
G_GNUC_INTERNAL
void _mcd_account_online_request_completed (McdAccount *account,
                                            GError *error);

typedef struct {
    McdOnlineRequestCb callback;
    gpointer user_data;
} McdOnlineRequestData;

G_GNUC_INTERNAL
GList *_mcd_account_get_online_requests (McdAccount *account);


static inline void
_mcd_account_write_conf (McdAccount *account)
{
    McdAccountManager *account_manager;

    account_manager = mcd_account_get_account_manager (account);
    g_return_if_fail (MCD_IS_ACCOUNT_MANAGER (account_manager));

    mcd_account_manager_write_conf (account_manager);
}

G_GNUC_INTERNAL void _mcd_account_compat_class_init (McdAccountClass *klass);

extern const McdDBusProp account_compat_properties[];

void account_compat_iface_init (McSvcAccountInterfaceCompatClass *iface,
				     gpointer iface_data);

G_GNUC_INTERNAL void _mcd_account_connection_begin (McdAccount *account);
G_GNUC_INTERNAL void _mcd_account_connection_class_init (McdAccountClass *klass);

#endif /* __MCD_ACCOUNT_PRIV_H__ */

