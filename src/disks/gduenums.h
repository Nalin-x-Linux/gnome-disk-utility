/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2008-2012 Red Hat, Inc.
 *
 * Licensed under GPL version 2 or later.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#ifndef __GDU_ENUMS_H__
#define __GDU_ENUMS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
  GDU_VOLUME_GRID_ELEMENT_TYPE_CONTAINER,
  GDU_VOLUME_GRID_ELEMENT_TYPE_NO_MEDIA,
  GDU_VOLUME_GRID_ELEMENT_TYPE_FREE_SPACE,
  GDU_VOLUME_GRID_ELEMENT_TYPE_DEVICE
} GduVolumeGridElementType;

typedef enum
{
  GDU_POWER_STATE_FLAGS_NONE              = 0,
  GDU_POWER_STATE_FLAGS_STANDBY           = (1<<0),
  GDU_POWER_STATE_FLAGS_CHECKING          = (1<<1),
  GDU_POWER_STATE_FLAGS_FAILED            = (1<<2)
} GduPowerStateFlags;

G_END_DECLS

#endif /* __GDU_ENUMS_H__ */
