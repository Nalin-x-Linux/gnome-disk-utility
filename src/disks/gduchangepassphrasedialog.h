/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2008-2012 Red Hat, Inc.
 *
 * Licensed under GPL version 2 or later.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#ifndef __GDU_CHANGE_PASSPHRASE_DIALOG_H__
#define __GDU_CHANGE_PASSPHRASE_DIALOG_H__

#include <gtk/gtk.h>
#include "gdutypes.h"

G_BEGIN_DECLS

void     gdu_change_passphrase_dialog_show (GduWindow    *window,
                                            UDisksObject *object);

G_END_DECLS

#endif /* __GDU_CHANGE_PASSPHRASE_DIALOG_H__ */
