/*
 * Copyright (C) 2017, Fajar Dwi Darmanto <fajardwidarm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef GSPDF_SIDEBAR_H
#define GSPDF_SIDEBAR_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_SIDEBAR gspdf_sidebar_get_type ()
G_DECLARE_FINAL_TYPE (GspdfSidebar, gspdf_sidebar, GSPDF, SIDEBAR, GtkGrid)

/* public methods */

GtkWidget *gspdf_sidebar_new ();


G_END_DECLS

#endif
