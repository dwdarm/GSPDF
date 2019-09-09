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

#ifndef GSPDF_APP_H
#define GSPDF_APP_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

#ifndef GSPDF_WINDOW_H
#include "gspdf-window/gspdf-window.h"
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_APP gspdf_app_get_type ()
G_DECLARE_FINAL_TYPE (GspdfApp, gspdf_app, GSPDF, APP, GspdfWindow)

GtkWidget *
gspdf_app_new ();

void
gspdf_app_open (GspdfApp    *app,
                const gchar *uri);

G_END_DECLS

#endif
