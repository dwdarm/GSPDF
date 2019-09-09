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

#include "gspdf-menu.h"

typedef struct {
	GtkWidget *file_menu;
	GtkWidget *file_menu_item;
	GtkWidget *open_menu_item;
	GtkWidget *save_menu_item;
	GtkWidget *prop_menu_item;
	GtkWidget *close_menu_item;
	GtkWidget *exit_menu_item;

	GtkWidget *goto_menu;
	GtkWidget *goto_menu_item;
	GtkWidget *next_menu_item;
	GtkWidget *prev_menu_item;
	GtkWidget *first_menu_item;
	GtkWidget *last_menu_item;

	GtkWidget *view_menu;
	GtkWidget *view_menu_item;
	GtkWidget *cont_menu_item;
	GtkWidget *zoom_menu;
	GtkWidget *zoom_menu_item;
	GtkWidget *zoomin_menu_item;
	GtkWidget *zoomout_menu_item;
	GtkWidget *zoomorg_menu_item;
	GtkWidget *zoomfitp_menu_item;
	GtkWidget *zoomfitw_menu_item;

	GtkWidget *mark_menu;
	GtkWidget *mark_menu_item;
	GtkWidget *markthis_menu_item;

	GtkWidget *help_menu;
	GtkWidget *help_menu_item;
	GtkWidget *about_menu_item;

} GspdfMenuPrivate;

struct _GspdfMenu {
	GtkMenuBar parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfMenu, gspdf_menu, GTK_TYPE_MENU_BAR)

/* object's properties */

enum {
	PROP_OPEN_MENU_ITEM = 1,
	PROP_SAVE_MENU_ITEM,
	PROP_PROP_MENU_ITEM,
	PROP_CLOSE_MENU_ITEM,
	PROP_EXIT_MENU_ITEM,

	PROP_NEXT_MENU_ITEM,
	PROP_PREV_MENU_ITEM,
	PROP_FIRST_MENU_ITEM,
	PROP_LAST_MENU_ITEM,

	PROP_CONT_MENU_ITEM,
	PROP_ZOOMIN_MENU_ITEM,
	PROP_ZOOMOUT_MENU_ITEM,
	PROP_ZOOMORG_MENU_ITEM,
	PROP_ZOOMFITP_MENU_ITEM,
	PROP_ZOOMFITW_MENU_ITEM,

	PROP_MARKTHIS_MENU_ITEM,

	PROP_ABOUT_MENU_ITEM,

	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_menu_set_property (GObject      *object,
	                       guint         property_id,
												 const GValue *value,
												 GParamSpec   *pspec)
{
	//GspdfMenuPrivate *priv = gspdf_menu_get_instance_private (GSPDF_MENU (object));

	switch (property_id) {
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_menu_get_property (GObject    *object,
	                       guint       property_id,
												 GValue     *value,
												 GParamSpec *pspec)
{
	GspdfMenuPrivate *priv = gspdf_menu_get_instance_private (GSPDF_MENU (object));

	switch (property_id) {
		case PROP_OPEN_MENU_ITEM:
			g_value_set_object (value, priv->open_menu_item);
			break;
		case PROP_SAVE_MENU_ITEM:
			g_value_set_object (value, priv->save_menu_item);
			break;
		case PROP_PROP_MENU_ITEM:
			g_value_set_object (value, priv->prop_menu_item);
			break;
		case PROP_CLOSE_MENU_ITEM:
			g_value_set_object (value, priv->close_menu_item);
			break;
		case PROP_EXIT_MENU_ITEM:
			g_value_set_object (value, priv->exit_menu_item);
			break;
		case PROP_NEXT_MENU_ITEM:
			g_value_set_object (value, priv->next_menu_item);
			break;
		case PROP_PREV_MENU_ITEM:
			g_value_set_object (value, priv->prev_menu_item);
			break;
		case PROP_FIRST_MENU_ITEM:
			g_value_set_object (value, priv->first_menu_item);
			break;
		case PROP_LAST_MENU_ITEM:
			g_value_set_object (value, priv->last_menu_item);
			break;
		case PROP_CONT_MENU_ITEM:
			g_value_set_object (value, priv->cont_menu_item);
			break;
		case PROP_ZOOMIN_MENU_ITEM:
			g_value_set_object (value, priv->zoomin_menu_item);
			break;
		case PROP_ZOOMOUT_MENU_ITEM:
			g_value_set_object (value, priv->zoomout_menu_item);
			break;
		case PROP_ZOOMORG_MENU_ITEM:
			g_value_set_object (value, priv->zoomorg_menu_item);
			break;
		case PROP_ZOOMFITP_MENU_ITEM:
			g_value_set_object (value, priv->zoomfitp_menu_item);
			break;
		case PROP_ZOOMFITW_MENU_ITEM:
			g_value_set_object (value, priv->zoomfitw_menu_item);
			break;
		case PROP_MARKTHIS_MENU_ITEM:
			g_value_set_object (value, priv->markthis_menu_item);
			break;
		case PROP_ABOUT_MENU_ITEM:
			g_value_set_object (value, priv->about_menu_item);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;

	}
}

/* object's instance init */
static void
gspdf_menu_init (GspdfMenu *object)
{
	GspdfMenuPrivate *priv = gspdf_menu_get_instance_private (object);

	/* file */
	priv->file_menu = gtk_menu_new ();
	priv->file_menu_item = gtk_menu_item_new_with_mnemonic ("_File");
	priv->open_menu_item = gtk_menu_item_new_with_label ("Open");
	priv->save_menu_item = gtk_menu_item_new_with_label ("Save");
	priv->prop_menu_item = gtk_menu_item_new_with_label ("Properties");
	priv->close_menu_item = gtk_menu_item_new_with_label ("Close");
	priv->exit_menu_item = gtk_menu_item_new_with_label ("Exit");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->file_menu_item), priv->file_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), priv->open_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), gtk_separator_menu_item_new ());
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), priv->save_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), gtk_separator_menu_item_new ());
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), priv->prop_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), priv->close_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->file_menu), priv->exit_menu_item);

	/* go to */
	priv->goto_menu = gtk_menu_new ();
	priv->goto_menu_item = gtk_menu_item_new_with_mnemonic ("_Go");
	priv->next_menu_item = gtk_menu_item_new_with_label ("Next Page");
	priv->prev_menu_item = gtk_menu_item_new_with_label ("Previous Page");
	priv->first_menu_item = gtk_menu_item_new_with_label ("First Page");
	priv->last_menu_item = gtk_menu_item_new_with_label ("Last Page");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->goto_menu_item), priv->goto_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->goto_menu), priv->first_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->goto_menu), priv->next_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->goto_menu), priv->prev_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->goto_menu), priv->last_menu_item);

	/* view */
	priv->view_menu = gtk_menu_new ();
	priv->view_menu_item = gtk_menu_item_new_with_mnemonic ("_View");
	priv->cont_menu_item = gtk_check_menu_item_new_with_label ("Continuous");
	priv->zoom_menu_item = gtk_menu_item_new_with_label ("Zoom");
	priv->zoomin_menu_item = gtk_menu_item_new_with_label ("Zoom In");
	priv->zoomout_menu_item = gtk_menu_item_new_with_label ("Zoom Out");
	priv->zoomorg_menu_item = gtk_menu_item_new_with_label ("Original Size");
	priv->zoomfitp_menu_item = gtk_menu_item_new_with_label ("Fit Page");
	priv->zoomfitw_menu_item = gtk_menu_item_new_with_label ("Fit Width");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->view_menu_item), priv->view_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->view_menu), priv->cont_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->view_menu), gtk_separator_menu_item_new ());
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->view_menu), priv->zoom_menu_item);

	priv->zoom_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->zoom_menu_item), priv->zoom_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->zoom_menu), priv->zoomin_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->zoom_menu), priv->zoomout_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->zoom_menu), priv->zoomorg_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->zoom_menu), priv->zoomfitp_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->zoom_menu), priv->zoomfitw_menu_item);

	/* bookmark */
	priv->mark_menu = gtk_menu_new ();
	priv->mark_menu_item = gtk_menu_item_new_with_mnemonic ("_Bookmark");
	priv->markthis_menu_item = gtk_menu_item_new_with_label ("Bookmark this page");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->mark_menu_item), priv->mark_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->mark_menu), priv->markthis_menu_item);

	/* Help */
	priv->help_menu = gtk_menu_new ();
	priv->help_menu_item = gtk_menu_item_new_with_mnemonic ("_Help");
	priv->about_menu_item = gtk_menu_item_new_with_label ("About");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->help_menu_item), priv->help_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (priv->help_menu), priv->about_menu_item);

	gtk_menu_shell_append (GTK_MENU_SHELL (object), priv->file_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (object), priv->view_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (object), priv->goto_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (object), priv->mark_menu_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (object), priv->help_menu_item);
}

/* object's class init */
static void
gspdf_menu_class_init (GspdfMenuClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = gspdf_menu_set_property;
	object_class->get_property = gspdf_menu_get_property;

	obj_properties[PROP_OPEN_MENU_ITEM] = g_param_spec_object (
		"open-menu-item",
		"Open-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_SAVE_MENU_ITEM] = g_param_spec_object (
		"save-menu-item",
		"Save-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_PROP_MENU_ITEM] = g_param_spec_object (
		"prop-menu-item",
		"Prop-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_CLOSE_MENU_ITEM] = g_param_spec_object (
		"close-menu-item",
		"Close-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_EXIT_MENU_ITEM] = g_param_spec_object (
		"exit-menu-item",
		"Exit-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_NEXT_MENU_ITEM] = g_param_spec_object (
		"next-menu-item",
		"Next-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_PREV_MENU_ITEM] = g_param_spec_object (
		"prev-menu-item",
		"prev-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_FIRST_MENU_ITEM] = g_param_spec_object (
		"first-menu-item",
		"First-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_LAST_MENU_ITEM] = g_param_spec_object (
		"last-menu-item",
		"Last-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_CONT_MENU_ITEM] = g_param_spec_object (
		"cont-menu-item",
		"Cont-menu-item",
		"",
		GTK_TYPE_CHECK_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMIN_MENU_ITEM] = g_param_spec_object (
		"zoomin-menu-item",
		"Zoomin-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMOUT_MENU_ITEM] = g_param_spec_object (
		"zoomout-menu-item",
		"Zoomout-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMORG_MENU_ITEM] = g_param_spec_object (
		"zoomorg-menu-item",
		"Zoomorg-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMFITP_MENU_ITEM] = g_param_spec_object (
		"zoomfitp-menu-item",
		"Zoomfitp-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ZOOMFITW_MENU_ITEM] = g_param_spec_object (
		"zoomfitw-menu-item",
		"Zoomfitw-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_MARKTHIS_MENU_ITEM] = g_param_spec_object (
		"markthis-menu-item",
		"Markthis-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	obj_properties[PROP_ABOUT_MENU_ITEM] = g_param_spec_object (
		"about-menu-item",
		"about-menu-item",
		"",
		GTK_TYPE_MENU_ITEM,
		G_PARAM_READABLE
	);

	 g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}


/* object's public methods */

GtkWidget *
gspdf_menu_new ()
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_MENU, NULL));
}
