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

#ifndef GSPDF_WINDOW_H
#define GSPDF_WINDOW_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_WINDOW gspdf_window_get_type ()
G_DECLARE_DERIVABLE_TYPE (GspdfWindow, gspdf_window, GSPDF, WINDOW, GtkWindow)

struct _GspdfWindowClass {
	GtkWindowClass parent_class;

	gpointer padding[12];
};

/* public methods */

GtkWidget *gspdf_window_new (void);

G_END_DECLS

#endif
