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

#include "gspdf-toolbar.h"

typedef struct {
	GtkToolItem *open_tool_item;
	GtkToolItem *index_entry_tool_item;
	GtkToolItem *index_label_tool_item;
	GtkToolItem *prev_tool_item;
	GtkToolItem *next_tool_item;
	GtkToolItem *zoomout_tool_item;
	GtkToolItem *zoomin_tool_item;
	GtkToolItem *zoomfit_tool_item;
	GtkToolItem *zoomorg_tool_item;
	GtkToolItem *find_entry_tool_item;
	GtkToolItem *find_tool_item;
} GspdfToolbarPrivate;

struct _GspdfToolbar {
	GtkToolbar parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfToolbar, gspdf_toolbar, GTK_TYPE_TOOLBAR)

enum {
	PROP_OPEN_TOOL_ITEM = 1,
	PROP_INDEX_ENTRY_TOOL_ITEM,
	PROP_INDEX_LABEL_TOOL_ITEM,
	PROP_PREV_TOOL_ITEM,
	PROP_NEXT_TOOL_ITEM,
	PROP_ZOOMOUT_TOOL_ITEM,
	PROP_ZOOMIN_TOOL_ITEM,
	PROP_ZOOMFIT_TOOL_ITEM,
	PROP_ZOOMORG_TOOL_ITEM,
	PROP_FIND_ENTRY_TOOL_ITEM,
	PROP_FIND_TOOL_ITEM,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_toolbar_set_property (GObject      *object,
	                          guint         property_id,
														const GValue *value,
														GParamSpec   *pspec)
{
	//GspdfToolbarPrivate *priv = gspdf_toolbar_get_instance_private (GSPDF_TOOLBAR (object));

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_toolbar_get_property (GObject    *object,
	                          guint       property_id,
														GValue     *value,
														GParamSpec *pspec)
{
	GspdfToolbarPrivate *priv = gspdf_toolbar_get_instance_private (
		GSPDF_TOOLBAR (object)
	);

	switch (property_id) {
		case PROP_OPEN_TOOL_ITEM:
			g_value_set_object (value, priv->open_tool_item);
			break;
		case PROP_INDEX_ENTRY_TOOL_ITEM:
			g_value_set_object (value, priv->index_entry_tool_item);
			break;
		case PROP_INDEX_LABEL_TOOL_ITEM:
			g_value_set_object (value, priv->index_label_tool_item);
			break;
		case PROP_PREV_TOOL_ITEM:
			g_value_set_object (value, priv->prev_tool_item);
			break;
		case PROP_NEXT_TOOL_ITEM:
			g_value_set_object (value, priv->next_tool_item);
			break;
		case PROP_ZOOMOUT_TOOL_ITEM:
			g_value_set_object (value, priv->zoomout_tool_item);
			break;
		case PROP_ZOOMIN_TOOL_ITEM:
			g_value_set_object (value, priv->zoomin_tool_item);
			break;
		case PROP_ZOOMFIT_TOOL_ITEM:
			g_value_set_object (value, priv->zoomfit_tool_item);
			break;
		case PROP_ZOOMORG_TOOL_ITEM:
			g_value_set_object (value, priv->zoomorg_tool_item);
			break;
		case PROP_FIND_ENTRY_TOOL_ITEM:
			g_value_set_object (value, priv->find_entry_tool_item);
			break;
		case PROP_FIND_TOOL_ITEM:
			g_value_set_object (value, priv->find_tool_item);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* object's instance init */
static void
gspdf_toolbar_init (GspdfToolbar *object)
{
	GspdfToolbarPrivate *priv = gspdf_toolbar_get_instance_private (object);

	// open
	priv->open_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->open_tool_item, -1);

	gtk_toolbar_insert (GTK_TOOLBAR (object), gtk_separator_tool_item_new (), -1);

	// index entry
	priv->index_entry_tool_item = gtk_tool_item_new ();
	GtkWidget *index_entry = gtk_entry_new ();
	gtk_entry_set_width_chars (GTK_ENTRY (index_entry), 4);
	gtk_container_add (GTK_CONTAINER (priv->index_entry_tool_item), index_entry);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->index_entry_tool_item, -1);

	// index label
	priv->index_label_tool_item = gtk_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (priv->index_label_tool_item), gtk_label_new (" of 0"));

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->index_label_tool_item, -1);

	// prev
	priv->prev_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->prev_tool_item, -1);

	// next
	priv->next_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("go-next", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->next_tool_item, -1);

	gtk_toolbar_insert (GTK_TOOLBAR (object), gtk_separator_tool_item_new (), -1);

	// zoomout
	priv->zoomout_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("zoom-out", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->zoomout_tool_item, -1);

	// zoomin
	priv->zoomin_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("zoom-in", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->zoomin_tool_item, -1);

	// zoomfit
	priv->zoomfit_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("zoom-fit-best", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->zoomfit_tool_item, -1);

	// zoomorg
	priv->zoomorg_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("zoom-original", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->zoomorg_tool_item, -1);

	gtk_toolbar_insert (GTK_TOOLBAR (object), gtk_separator_tool_item_new (), -1);

	// find entry
	priv->find_entry_tool_item = gtk_tool_item_new ();
	GtkWidget *find_entry = gtk_entry_new ();
	gtk_entry_set_width_chars (GTK_ENTRY (find_entry), 18);
	gtk_container_add (GTK_CONTAINER (priv->find_entry_tool_item), find_entry);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->find_entry_tool_item, -1);

	// find
	priv->find_tool_item = gtk_tool_button_new (
		gtk_image_new_from_icon_name ("edit-find", GTK_ICON_SIZE_LARGE_TOOLBAR),
		NULL
	);

	gtk_toolbar_insert (GTK_TOOLBAR (object), priv->find_tool_item, -1);
}

/* object's class init */
static void
gspdf_toolbar_class_init (GspdfToolbarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gspdf_toolbar_set_property;
	object_class->get_property = gspdf_toolbar_get_property;

	obj_properties[PROP_OPEN_TOOL_ITEM] = g_param_spec_object (
		"open-tool-item",
		"Open-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_INDEX_ENTRY_TOOL_ITEM] = g_param_spec_object (
		"index-entry-tool-item",
		"Index-entry-tool-item",
		"",
		GTK_TYPE_TOOL_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_INDEX_LABEL_TOOL_ITEM] = g_param_spec_object (
		"index-label-tool-item",
		"Index-label-tool-item",
		"",
		GTK_TYPE_TOOL_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_PREV_TOOL_ITEM] = g_param_spec_object (
		"prev-tool-item",
		"Prev-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_NEXT_TOOL_ITEM] = g_param_spec_object (
		"next-tool-item",
		"Next-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMOUT_TOOL_ITEM] = g_param_spec_object (
		"zoomout-tool-item",
		"Zoomout-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMIN_TOOL_ITEM] = g_param_spec_object (
		"zoomin-tool-item",
		"Zoomin-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMFIT_TOOL_ITEM] = g_param_spec_object (
		"zoomfit-tool-item",
		"Zoomfit-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMORG_TOOL_ITEM] = g_param_spec_object (
		"zoomorg-tool-item",
		"Zoomorg-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	obj_properties[PROP_FIND_ENTRY_TOOL_ITEM] = g_param_spec_object (
		"find-entry-tool-item",
		"Find-entry-tool-item",
		"",
		GTK_TYPE_TOOL_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_FIND_TOOL_ITEM] = g_param_spec_object (
		"find-tool-item",
		"Find-tool-item",
		"",
		GTK_TYPE_TOOL_BUTTON,
		G_PARAM_READABLE
	);

	g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

GtkWidget *
gspdf_toolbar_new ()
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_TOOLBAR, NULL));
}
