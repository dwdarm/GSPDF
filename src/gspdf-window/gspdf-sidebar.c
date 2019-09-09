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

#include "gspdf-sidebar.h"

enum {
	LIST_OUTLINE = 0,
	LIST_BOOKMARK,
	N_LIST
};

typedef struct {
	GtkWidget *combo_box;
	GtkWidget *notebook;
	GtkWidget *swindow[N_LIST];
	GtkWidget *treeview[N_LIST];
} GspdfSidebarPrivate;

struct _GspdfSidebar {
	GtkBox parent;
};

enum {
	PROP_OUTLINE = 1,
	PROP_BOOKMARK,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfSidebar, gspdf_sidebar, GTK_TYPE_GRID)

static void
on_combo_box_changed (GtkComboBox *widget,
                      gpointer     user_data)
{
	GspdfSidebarPrivate *priv = gspdf_sidebar_get_instance_private (user_data);

	gtk_notebook_set_current_page (
		GTK_NOTEBOOK (priv->notebook),
		gtk_combo_box_get_active (GTK_COMBO_BOX (priv->combo_box))
	);
}

static void
gspdf_sidebar_set_property (GObject      *object,
	                          guint         property_id,
														const GValue *value,
														GParamSpec   *pspec)
{
	//GspdfSidebarPrivate *priv = gspdf_sidebar_get_instance_private (GSPDF_SIDEBAR (object));

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_sidebar_get_property (GObject    *object,
	                          guint       property_id,
														GValue     *value,
														GParamSpec *pspec)
{
	GspdfSidebarPrivate *priv = gspdf_sidebar_get_instance_private (
		GSPDF_SIDEBAR (object)
	);

	switch (property_id) {
		case PROP_OUTLINE:
			g_value_set_object (value, priv->treeview[LIST_OUTLINE]);
			break;
		case PROP_BOOKMARK:
			g_value_set_object (value, priv->treeview[LIST_BOOKMARK]);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_sidebar_init (GspdfSidebar *object)
{
	GspdfSidebarPrivate *priv = gspdf_sidebar_get_instance_private (object);
	//GtkTreeStore *store = NULL;

	// combo box
	priv->combo_box = gtk_combo_box_text_new ();
	gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->combo_box), NULL, "Outline");
	gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (priv->combo_box), NULL, "Bookmark");
	gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo_box), 0);
	gtk_widget_set_hexpand (priv->combo_box, TRUE);

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "ellipsize", PANGO_ELLIPSIZE_MIDDLE, NULL);

	// outline
	priv->treeview[LIST_OUTLINE] = gtk_tree_view_new ();
  gtk_tree_view_append_column (
		GTK_TREE_VIEW (priv->treeview[LIST_OUTLINE]),
		gtk_tree_view_column_new_with_attributes (
			"Outline",
			renderer,
			"text",
			0,
			NULL
		)
	);

	gtk_tree_view_set_headers_visible (
		GTK_TREE_VIEW (priv->treeview[LIST_OUTLINE]),
		FALSE
	);

	priv->swindow[LIST_OUTLINE] = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (
		GTK_CONTAINER (priv->swindow[LIST_OUTLINE]),
		priv->treeview[LIST_OUTLINE]
	);

	// bookmark
	priv->treeview[LIST_BOOKMARK] = gtk_tree_view_new ();
    gtk_tree_view_append_column (
		GTK_TREE_VIEW (priv->treeview[LIST_BOOKMARK]),
		gtk_tree_view_column_new_with_attributes (
			"Bookmark",
			renderer,
			"text",
			0,
			NULL
		)
	);

	gtk_tree_view_set_headers_visible (
		GTK_TREE_VIEW (priv->treeview[LIST_BOOKMARK]),
		FALSE
	);

	priv->swindow[LIST_BOOKMARK] = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (
		GTK_CONTAINER (priv->swindow[LIST_BOOKMARK]),
		priv->treeview[LIST_BOOKMARK]
	);

	// notebook
	priv->notebook = gtk_notebook_new ();
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->notebook), FALSE);
	gtk_notebook_append_page (
		GTK_NOTEBOOK (priv->notebook),
		priv->swindow[LIST_OUTLINE],
		NULL
	);
	gtk_notebook_append_page (
		GTK_NOTEBOOK (priv->notebook),
		priv->swindow[LIST_BOOKMARK],
		NULL
	);
	gtk_widget_set_hexpand (priv->notebook, TRUE);
	gtk_widget_set_vexpand (priv->notebook, TRUE);

	gtk_grid_attach (GTK_GRID (object), priv->combo_box, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (object), priv->notebook, 0, 1, 1, 1);

	g_signal_connect (
		G_OBJECT (priv->combo_box),
		"changed",
		G_CALLBACK (on_combo_box_changed),
		object
	);
}

static void
gspdf_sidebar_class_init (GspdfSidebarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gspdf_sidebar_set_property;
	object_class->get_property = gspdf_sidebar_get_property;

	obj_properties[PROP_OUTLINE] = g_param_spec_object (
		"outline",
		"Outlinea",
		"",
		GTK_TYPE_TREE_VIEW,
		G_PARAM_READABLE
	);

	obj_properties[PROP_BOOKMARK] = g_param_spec_object (
		"bookmark",
		"Bookmark",
		"",
		GTK_TYPE_TREE_VIEW,
		G_PARAM_READABLE
	);

	g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

GtkWidget *
gspdf_sidebar_new ()
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_SIDEBAR, NULL));
}
