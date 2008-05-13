/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 8 -*- */
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

#include "mcd-master.h"
#include "mcd-account.h"
#include "mcd-account-priv.h"
#include "mcd-account-connection.h"
#include "mcd-account-manager.h"

typedef struct {
    GHashTable *params;
    gint i_filter;
} McdAccountConnectionContext;

static GQuark account_connection_context_quark;

static void
context_free (gpointer ptr)
{
    McdAccountConnectionContext *ctx = ptr;

    g_free (ctx);
}

void
mcd_account_connection_begin (McdAccount *account)
{
    McdAccountConnectionContext *ctx;

    /* get account params */
    /* create dynamic params HT */
    /* run the handlers */
    ctx = g_malloc (sizeof (McdAccountConnectionContext));
    ctx->i_filter = 0;
    ctx->params = mcd_account_get_parameters (account);
    g_object_set_qdata_full ((GObject *)account,
			     account_connection_context_quark,
			     ctx, context_free);

    mcd_account_connection_proceed (account, TRUE);
}

void 
mcd_account_connection_proceed (McdAccount *account, gboolean success)
{
    McdAccountConnectionContext *ctx;
    McdAccountConnectionFunc func = NULL;
    gpointer userdata;
    McdMaster *master;

    /* call next handler, or terminate the chain (emitting proper signal).
     * if everything is fine, call mcd_manager_create_connection() and
     * mcd_connection_connect () with the dynamic parameters. Remove that call
     * from mcd_manager_create_connection() */
    ctx = g_object_get_qdata ((GObject *)account,
			      account_connection_context_quark);
    g_return_if_fail (ctx != NULL);

    if (success)
    {
	master = mcd_master_get_default ();
	mcd_master_get_nth_account_connection (master, ctx->i_filter++,
					       &func, &userdata);
    }
    if (func)
    {
	func (account, ctx->params, userdata);
    }
    else
    {
	/* end of the chain */
	g_signal_emit (account, _mcd_account_signals[CONNECTION_PROCESS], 0,
		       success);
	if (success)
	{
	    _mcd_account_connect (account, ctx->params);
	}
	g_object_set_qdata ((GObject *)account,
			    account_connection_context_quark, NULL);
    }
}

inline void
_mcd_account_connection_class_init (McdAccountClass *klass)
{
    _mcd_account_signals[CONNECTION_PROCESS] =
	g_signal_new ("connection-process",
		      G_OBJECT_CLASS_TYPE (klass),
		      G_SIGNAL_RUN_LAST,
		      0,
		      NULL, NULL, g_cclosure_marshal_VOID__BOOLEAN,
		      G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

    account_connection_context_quark = g_quark_from_static_string ("accontext");
}
