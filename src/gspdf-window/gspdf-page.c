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

#include "gspdf-page.h"

typedef struct {
	GtkWidget *grid;
	GtkWidget *drawing_area;
	GtkWidget *vscroll;
	GtkWidget *hscroll;
	gpointer   user_data;
} GspdfPagePrivate;

struct _GspdfPage {
	GtkEventBox parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfPage, gspdf_page, GTK_TYPE_EVENT_BOX)


/* object's properties */

enum {
	PROP_DRAWING_AREA = 1,
	PROP_VSCROLL,
	PROP_HSCROLL,
	PROP_USER_DATA,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_page_set_property (GObject      *object,
	                       guint         property_id,
												 const GValue *value,
												 GParamSpec   *pspec)
{
	GspdfPagePrivate *priv = gspdf_page_get_instance_private (GSPDF_PAGE (object));

	switch (property_id) {
		case PROP_USER_DATA:
			priv->user_data = g_value_get_pointer (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_page_get_property (GObject    *object,
	                       guint       property_id,
												 GValue     *value,
												 GParamSpec *pspec)
{
	GspdfPagePrivate *priv = gspdf_page_get_instance_private (GSPDF_PAGE (object));

	switch (property_id) {
		case PROP_DRAWING_AREA:
			g_value_set_object (value, priv->drawing_area);
			break;
		case PROP_VSCROLL:
			g_value_set_object (value, priv->vscroll);
			break;
		case PROP_HSCROLL:
			g_value_set_object (value, priv->hscroll);
			break;
		case PROP_USER_DATA:
			g_value_set_pointer (value, priv->user_data);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* object's instance init */
static void
gspdf_page_init (GspdfPage *object)
{
	GspdfPagePrivate *priv = gspdf_page_get_instance_private (object);

	/* init drawing area */
	priv->drawing_area = gtk_drawing_area_new ();
	gtk_widget_set_hexpand (GTK_WIDGET (priv->drawing_area), TRUE);
	gtk_widget_set_vexpand (GTK_WIDGET (priv->drawing_area), TRUE);
	gtk_widget_set_can_focus (GTK_WIDGET (priv->drawing_area), TRUE);

	/* init vertical scrollbar */
	priv->vscroll = gtk_scrollbar_new (GTK_ORIENTATION_VERTICAL, NULL);

	/* init horizontal scrollbar */
	priv->hscroll = gtk_scrollbar_new (GTK_ORIENTATION_HORIZONTAL, NULL);

	/* init grid layout */
	priv->grid = gtk_grid_new ();
	gtk_grid_attach (GTK_GRID (priv->grid), priv->drawing_area, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (priv->grid), priv->vscroll, 1, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (priv->grid), priv->hscroll, 0, 1, 1, 1);

	gtk_container_add (GTK_CONTAINER (object), priv->grid);
}

/* object's class init */
static void
gspdf_page_class_init (GspdfPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gspdf_page_set_property;
	object_class->get_property = gspdf_page_get_property;

	obj_properties[PROP_DRAWING_AREA] = g_param_spec_object (
		"drawing-area",
		"Drawing-area",
		"",
		GTK_TYPE_DRAWING_AREA,
		G_PARAM_READABLE
	);

	obj_properties[PROP_VSCROLL] = g_param_spec_object (
		"v-scroll",
		"V-scroll",
		"",
		GTK_TYPE_SCROLLBAR,
		G_PARAM_READABLE
	);

	obj_properties[PROP_HSCROLL] = g_param_spec_object (
		"h-scroll",
		"H-scroll",
		"",
		GTK_TYPE_SCROLLBAR,
		G_PARAM_READABLE
	);

	obj_properties[PROP_USER_DATA] = g_param_spec_pointer (
		"user-data",
		"User-data",
		"",
		G_PARAM_READWRITE
	);

	 g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}


/* object's public methods */

GtkWidget *
gspdf_page_new (void)
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_PAGE, NULL));
}

void
gspdf_page_queue_draw (GspdfPage *page)
{
	g_return_if_fail (page != NULL);
	g_return_if_fail (GSPDF_IS_PAGE (page));

	GspdfPagePrivate *priv = gspdf_page_get_instance_private (page);

	gtk_widget_queue_draw (priv->drawing_area);
}
