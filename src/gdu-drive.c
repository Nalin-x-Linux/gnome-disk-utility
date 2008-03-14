/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* gdu-drive.c
 *
 * Copyright (C) 2007 David Zeuthen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <config.h>
#include <string.h>
#include <glib/gi18n.h>
#include <dbus/dbus-glib.h>

#include "gdu-main.h"
#include "gdu-pool.h"
#include "gdu-drive.h"
#include "gdu-presentable.h"

struct _GduDrivePrivate
{
        GduDevice *device;
};

static GObjectClass *parent_class = NULL;

static void gdu_drive_presentable_iface_init (GduPresentableIface *iface);
G_DEFINE_TYPE_WITH_CODE (GduDrive, gdu_drive, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GDU_TYPE_PRESENTABLE,
                                                gdu_drive_presentable_iface_init))

static void
gdu_drive_finalize (GduDrive *drive)
{
        if (drive->priv->device != NULL)
                g_object_unref (drive->priv->device);

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (G_OBJECT (drive));
}

static void
gdu_drive_class_init (GduDriveClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = (GObjectFinalizeFunc) gdu_drive_finalize;

}

static void
gdu_drive_init (GduDrive *drive)
{
        drive->priv = g_new0 (GduDrivePrivate, 1);
}

static void
device_changed (GduDevice *device, gpointer user_data)
{
        GduDrive *drive = GDU_DRIVE (user_data);
        g_signal_emit_by_name (drive, "changed");
}

static void
device_removed (GduDevice *device, gpointer user_data)
{
        GduDrive *drive = GDU_DRIVE (user_data);
        g_signal_emit_by_name (drive, "removed");
}

GduDrive *
gdu_drive_new_from_device (GduDevice *device)
{
        GduDrive *drive;

        drive = GDU_DRIVE (g_object_new (GDU_TYPE_DRIVE, NULL));
        drive->priv->device = g_object_ref (device);
        g_signal_connect (device, "changed", (GCallback) device_changed, drive);
        g_signal_connect (device, "removed", (GCallback) device_removed, drive);

        return drive;
}

static GduDevice *
gdu_drive_get_device (GduPresentable *presentable)
{
        GduDrive *drive = GDU_DRIVE (presentable);
        return g_object_ref (drive->priv->device);
}

static GduPresentable *
gdu_drive_get_enclosing_presentable (GduPresentable *presentable)
{
        return NULL;
}

static char *
gdu_drive_get_name (GduPresentable *presentable)
{
        GduDrive *drive = GDU_DRIVE (presentable);
        const char *vendor;
        const char *model;
        guint64 size;
        gboolean is_removable;
        char *strsize;
        char *result;

        vendor = gdu_device_drive_get_vendor (drive->priv->device);
        model = gdu_device_drive_get_model (drive->priv->device);
        size = gdu_device_get_size (drive->priv->device);
        is_removable = gdu_device_is_removable (drive->priv->device);

        strsize = NULL;
        if (!is_removable && size > 0) {
                strsize = gdu_util_get_size_for_display (size, FALSE);
        }

        if (strsize != NULL) {
                result = g_strdup_printf ("%s %s %s",
                                        strsize,
                                        vendor != NULL ? vendor : "",
                                        model != NULL ? model : "");
        } else {
                result = g_strdup_printf ("%s %s",
                                        vendor != NULL ? vendor : "",
                                        model != NULL ? model : "");
        }
        g_free (strsize);

        return result;
}

static char *
gdu_drive_get_icon_name (GduPresentable *presentable)
{
        //GduDrive *drive = GDU_DRIVE (presentable);
        return g_strdup ("drive-harddisk");
}

static guint64
gdu_drive_get_offset (GduPresentable *presentable)
{
        return 0;
}

static GList *
gdu_drive_get_info (GduPresentable *presentable)
{
        GduDrive *drive = GDU_DRIVE (presentable);
        GduDevice *device = drive->priv->device;
        GList *kv_pairs = NULL;

	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Vendor")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (gdu_device_drive_get_vendor (device)));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Model")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (gdu_device_drive_get_model (device)));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Revision")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (gdu_device_drive_get_revision (device)));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Serial Number")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (gdu_device_drive_get_serial (device)));

	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Device File")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (gdu_device_get_device_file (device)));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Connection")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("-"))); /* TODO */

	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Removable Media")));
	if (gdu_device_is_removable (device)) {
	        if (gdu_device_is_media_available (device)) {
                        kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Yes")));
	        } else {
                        kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Yes (No media inserted)")));
	        }
	} else {
	        kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("No")));
	}
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Media Type")));
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Disk"))); /* TODO */
	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Capacity")));
	if (gdu_device_is_media_available (device)) {
	        kv_pairs = g_list_prepend (kv_pairs,
                                           gdu_util_get_size_for_display (gdu_device_get_size (device), TRUE));
	} else {
	        kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("-")));
	}

	kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("Partitioning")));
	if (gdu_device_is_partition_table (device)) {
	        const char *scheme;
	        char *name;
	        scheme = gdu_device_partition_table_get_scheme (device);
	        if (strcmp (scheme, "apm") == 0) {
                        name = g_strdup (_("Apple Partition Map"));
	        } else if (strcmp (scheme, "mbr") == 0) {
                        name = g_strdup (_("Master Boot Record"));
	        } else if (strcmp (scheme, "gpt") == 0) {
                        name = g_strdup (_("GUID Partition Table"));
	        } else {
                        name = g_strdup_printf (_("Unknown (%s)"), scheme);
	        }
	        kv_pairs = g_list_prepend (kv_pairs, name);
	} else {
	        kv_pairs = g_list_prepend (kv_pairs, g_strdup (_("-")));
	}

        kv_pairs = g_list_reverse (kv_pairs);
        return kv_pairs;
}


static void
gdu_drive_presentable_iface_init (GduPresentableIface *iface)
{
        iface->get_device = gdu_drive_get_device;
        iface->get_enclosing_presentable = gdu_drive_get_enclosing_presentable;
        iface->get_name = gdu_drive_get_name;
        iface->get_icon_name = gdu_drive_get_icon_name;
        iface->get_offset = gdu_drive_get_offset;
        iface->get_info = gdu_drive_get_info;
}
