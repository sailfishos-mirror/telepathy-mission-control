/*
 * mcd-account.h - the Telepathy Account D-Bus interface (service side)
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __MCD_ACCOUNT_H__
#define __MCD_ACCOUNT_H__

/* auto-generated stubs */
#include "_gen/svc-Account.h"

G_BEGIN_DECLS
#define MCD_TYPE_ACCOUNT         (mcd_account_get_type ())
#define MCD_ACCOUNT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MCD_TYPE_ACCOUNT, McdAccount))
#define MCD_ACCOUNT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), MCD_TYPE_ACCOUNT, McdAccountClass))
#define MCD_IS_ACCOUNT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MCD_TYPE_ACCOUNT))
#define MCD_IS_ACCOUNT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MCD_TYPE_ACCOUNT))
#define MCD_ACCOUNT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MCD_TYPE_ACCOUNT, McdAccountClass))

typedef struct _McdAccount McdAccount;
typedef struct _McdAccountPrivate McdAccountPrivate;
typedef struct _McdAccountClass McdAccountClass;

struct _McdAccount
{
    GObject parent;
    McdAccountPrivate *priv;
};

struct _McdAccountClass
{
    GObjectClass parent_class;
};


GType mcd_account_get_type (void);
McdAccount *mcd_account_new (GKeyFile *keyfile, const gchar *name);

#endif
