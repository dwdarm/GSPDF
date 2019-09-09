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

#include "gspdf-window.h"

#ifndef GSPDF_MENU_H
#include "gspdf-menu.h"
#endif

#ifndef GSPDF_TOOLBAR_H
#include "gspdf-toolbar.h"
#endif

#ifndef GSPDF_SIDEBAR_H
#include "gspdf-sidebar.h"
#endif

typedef struct {
	GtkWidget *box;
	GtkWidget *menu;
	GtkWidget *toolbar;
	GtkWidget *sidebar;
	GtkWidget *paned;
	GtkWidget *tab;

	GtkAccelGroup *accel_group;
} GspdfWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GspdfWindow, gspdf_window, GTK_TYPE_WINDOW)


/* object's properties */

enum {
	PROP_NOTEBOOK = 1,
	PROP_MENU,
	PROP_TOOLBAR,
	PROP_SIDEBAR,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_window_set_property (GObject      *object,
	                         guint         property_id,
													 const GValue *value,
													 GParamSpec   *pspec)
{
	GspdfWindowPrivate *priv = gspdf_window_get_instance_private (
		GSPDF_WINDOW (object)
	);

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_window_get_property (GObject    *object,
	                         guint       property_id,
													 GValue     *value,
													 GParamSpec *pspec)
{
	GspdfWindowPrivate *priv = gspdf_window_get_instance_private (
		GSPDF_WINDOW (object)
	);

	switch (property_id) {
		case PROP_NOTEBOOK:
			g_value_set_object (value, priv->tab);
			break;
		case PROP_MENU:
			g_value_set_object (value, priv->menu);
			break;
		case PROP_TOOLBAR:
			g_value_set_object (value, priv->toolbar);
			break;
		case PROP_SIDEBAR:
			g_value_set_object (value, priv->sidebar);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* object's instance init */
static void
gspdf_window_init (GspdfWindow *object)
{
	GspdfWindowPrivate *priv = gspdf_window_get_instance_private (object);
	GtkWidget *ptr = NULL;

	/* init accel group */
	priv->accel_group = gtk_accel_group_new ();
	gtk_window_add_accel_group (GTK_WINDOW (object), priv->accel_group);

	/* init menu */
	priv->menu = gspdf_menu_new ();
	g_object_get (G_OBJECT (priv->menu), "open-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_O,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "save-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_S,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "close-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_W,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "exit-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_Q,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "next-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_space,
		GDK_RELEASE_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "prev-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_BackSpace,
		GDK_RELEASE_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "first-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_Home,
		GDK_RELEASE_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "last-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_End,
		GDK_RELEASE_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "zoomin-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_Up,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "zoomout-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_Down,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "zoomorg-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_0,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "zoomfitp-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_8,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "zoomfitw-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_9,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE
	);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (priv->menu), "markthis-menu-item", &ptr, NULL);
	gtk_widget_add_accelerator (
		ptr,
		"activate",
		priv->accel_group,
		GDK_KEY_D,
		GDK_CONTROL_MASK,
		GTK_ACCEL_VISIBLE);
	g_object_unref (G_OBJECT (ptr));

	/* init toolbar */
	priv->toolbar = gspdf_toolbar_new ();

	/* init horizontal paned */
	priv->paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_set_wide_handle (GTK_PANED (priv->paned), TRUE);

	/* sidebar */
	priv->sidebar = gspdf_sidebar_new ();
	gtk_paned_add1 (GTK_PANED (priv->paned), priv->sidebar);

	/* init tab */
	priv->tab = gtk_notebook_new ();
	gtk_paned_add2 (GTK_PANED (priv->paned), priv->tab);

	/* init box */
	priv->box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (priv->box), priv->menu);
	gtk_container_add (GTK_CONTAINER (priv->box), priv->toolbar);
	gtk_container_add (GTK_CONTAINER (priv->box), priv->paned);

	/* add paned to main window */
	gtk_container_add (GTK_CONTAINER (object), priv->box);
}

/* object's class init */
static void
gspdf_window_class_init (GspdfWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gspdf_window_set_property;
	object_class->get_property = gspdf_window_get_property;

	obj_properties[PROP_NOTEBOOK] = g_param_spec_object (
		"notebook",
		"Notebook",
		"",
		GTK_TYPE_NOTEBOOK,
		G_PARAM_READABLE
	);

	obj_properties[PROP_MENU] = g_param_spec_object (
		"menu",
		"Menu",
		"",
		GSPDF_TYPE_MENU,
		G_PARAM_READABLE
	);

	obj_properties[PROP_TOOLBAR] = g_param_spec_object (
		"toolbar",
		"Toolbar",
		"",
		GSPDF_TYPE_TOOLBAR,
		G_PARAM_READABLE
	);

	obj_properties[PROP_SIDEBAR] = g_param_spec_object (
		"sidebar",
		"Sidebar",
		"",
		GSPDF_TYPE_SIDEBAR,
		G_PARAM_READABLE
	);

	g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}


/* object's public methods */

GtkWidget *
gspdf_window_new (void)
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_WINDOW, NULL));
}
