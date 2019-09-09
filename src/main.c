
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

#include "gspdf-app.h"

static void
on_activate (GtkApplication *app,
	           gpointer user_data)
{
	GtkWidget *gspdf = gspdf_app_new ();
	gtk_widget_show_all (gspdf);
	gtk_application_add_window (app, GTK_WINDOW (gspdf));
}

static void
on_open (GApplication *application,
         gpointer      files,
         gint          n_files,
         gchar        *hint,
         gpointer      user_data)
{
	g_application_activate (application);

	if (!files) {
		return;
	}

	if (!n_files) {
		return;
	}

	GFile **gfiles = (GFile**) files;
	gchar *uri = g_file_get_uri (gfiles[0]);

	if (!uri) {
		return;
	}

	GtkWidget *window = (GtkWidget*) gtk_application_get_active_window (
		GTK_APPLICATION (application)
	);

	if (!window) {
		g_free (uri);
		return;
	}

	gspdf_app_open (GSPDF_APP (window), uri);
	g_free (uri);
}

int
main (int    argc,
      char **argv)
{
	GtkApplication *app = gtk_application_new (
		"org.gtk.gspdf",
		G_APPLICATION_HANDLES_OPEN
	);

	g_signal_connect (
		G_OBJECT (app),
		"activate",
		G_CALLBACK (on_activate),
		NULL
	);

	g_signal_connect (
		G_OBJECT (app),
		"open",
		G_CALLBACK (on_open),
		NULL
	);

	gint status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (G_OBJECT (app));

	return status;
}
