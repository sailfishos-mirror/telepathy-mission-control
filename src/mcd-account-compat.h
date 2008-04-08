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

#ifndef __MCD_ACCOUNT_COMPAT_H__
#define __MCD_ACCOUNT_COMPAT_H__

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/enums.h>
/* auto-generated stubs */
#include "_gen/svc-Account_Interface_Compat.h"

#include "mcd-dbusprop.h"

G_BEGIN_DECLS

const McdDBusProp *_mcd_account_compat_get_properties (void);
void _mcd_account_compat_iface_init (McSvcAccountInterfaceCompatClass *iface,
				     gpointer iface_data);

G_END_DECLS
#endif