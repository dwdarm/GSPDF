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
#include <math.h>
#include <stdlib.h>

#ifndef GSPDF_PAGE_H
#include "gspdf-window/gspdf-page.h"
#endif

#ifndef GSPDF_PAGE_CACHE_H
#include "gspdf-page-cache.h"
#endif

/* Main Window's signal id*/
enum {
	SIGNAL_WINDOW_OUTLINE = 0,
	SIGNAL_WINDOW_BOOKMARK,
	SIGNAL_WINDOW_CONTINUOUS,
	N_WINDOW_SIGNALS
};

/* Page's signal id*/
enum {
	SIGNAL_HADJ = 0,
	SIGNAL_VADJ,
	SIGNAL_MOTION,
	N_SIGNALS
};

/* Config's indexs */
enum {
	CONFIG_CACHE_INDEX = 0,
	CONFIG_CACHE_SCALE,
	CONFIG_CACHE_SCALEMODE,
	CONFIG_CACHE_HPRCNT,
	CONFIG_CACHE_VPRCNT,
	CONFIG_CACHE_CONTINUOUS
};

/* scale mode */
enum {
	SCALE_NORMAL,
	SCALE_FITP,
	SCALE_FITW,
	SCALE_CUSTOM
};

/* pointer mode */
enum {
	POINTER_MODE_NORMAL = 0,
	POINTER_MODE_TEXT,
	POINTER_MODE_LINK
};

#define DEFAULT_SCALE_VALUE      1.0
#define DEFAULT_SPACING_VALUE    10
#define DEFAULT_CONTINUOUS_VALUE TRUE
#define DEFAULT_SCALE_MODE_VALUE SCALE_NORMAL
#define SCALE_STEP				       0.25

typedef struct {
	gint   index;
	GList *selection;
} GspdfPageSelection;

typedef struct {
	GtkWidget *menu;
	GtkWidget *copy;
} GspdfPagePopup;

typedef struct {
	GtkWidget *menu;
	GtkWidget *delete;
} GspdfBookmarkPopup;

typedef struct {
	GtkWidget      *window;
	GtkWidget      *page;

	GdkRGBA         background;
	gdouble         spacing;

	GspdfDocument  *document;
	gint            index;
	gboolean        continuous;
	gdouble         scale;
	gint            scale_mode;

	gint            pointer_mode;
	GspdfDocAction *link_action;

	gdouble         hadj_val_prcnt;
	gdouble         vadj_val_prcnt;

	gboolean        selecting;
	gdouble         start_x;
	gdouble         start_y;
	gdouble         end_x;
	gdouble         end_y;
	GList          *selection;

	gchar          *find_text;
	gint            find_index;
	GList          *find_rect;
	GList          *find_rect_iter;

	GtkTreeStore   *outline;
	GtkTreeIter     outline_iter;

	GtkTreeStore   *bookmark;
	GtkTreeIter     bookmark_iter;

	GPtrArray      *doc_map;

	GspdfPageCache *page_cache;

	gint            signals[N_SIGNALS];
} GspdfPageData;

typedef struct {
	GspdfPagePopup     *page_popup;
	GspdfBookmarkPopup *bookmark_popup;
	GKeyFile           *config;
	gint                signals[N_WINDOW_SIGNALS];
} GspdfAppPrivate;

struct _GspdfApp {
	GspdfWindow parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfApp, gspdf_app, GSPDF_TYPE_WINDOW)

static void
reset_menu (GspdfApp *object);

static void
reset_toolbar (GspdfApp *object);

static void
load_config (GspdfApp *object);

static void
save_config (GspdfApp *object);

static GtkWidget *
get_current_page (GspdfApp *object);

static GspdfPageData *
get_current_page_data (GspdfApp *object);

static gint
get_n_pages (GspdfApp *object);

static void
update_index_toolbar (GspdfApp *object);

static void
update_config_bookmark (GspdfPageData *page_data);

static void
update_config_cache (GspdfPageData *page_data);

static void
restore_from_config_cache (GspdfPageData *page_data);

static void
open_document (GspdfPageData *page_data,
			         const gchar   *uri,
			         const gchar   *password);

static void
close_document (GspdfPageData *page_data);

static void
fill_outline (GspdfPageData *page_data,
			        GspdfDocument *doc);

static void
fill_bookmark (GspdfPageData *page_data);

static gdouble
get_page_allocated_width (GspdfPageData *page_data);

static gdouble
get_page_allocated_height (GspdfPageData *page_data);

static gdouble
get_page_maximum_width (GspdfPageData *page_data);

static gdouble
get_page_maximum_height (GspdfPageData *page_data);

static gdouble
get_page_total_height (GspdfPageData *page_data);

static gdouble
get_page_y_offset (GspdfPageData *page_data,
				   			   gint           index);

static gboolean
get_current_pointer_position (GspdfPageData *page_data,
							                gdouble        x_win,
							                gdouble        y_win,
							                gint          *index,
							                gdouble       *x_point,
							                gdouble       *y_point);

static gboolean
get_selected_area (GspdfPageData        *page_data,
				           const GspdfRectangle *selection,
				           gint                 *start,
				           gint                 *end,
				           GspdfRectangle       *area);

static gboolean
get_index_page_area (GspdfPageData        *page_data,
					           const GspdfRectangle *area,
					           gint                 *start,
				             gint                 *end);

static GList *
get_selected_page_area (GspdfPageData        *page_data,
					              const GspdfRectangle *area);

static gchar *
get_selected_text_area (GspdfPageData        *page_data,
					              const GspdfRectangle *area);

static void
update_page_range (GspdfPageData *page_data,
				           gint           start,
				           gint           end);

static void
update_page_range_area (GspdfPageData        *page_data,
					              const GspdfRectangle *area);

static void
draw_single_page (GspdfPageData        *page_data,
				          GtkWidget            *widget,
                  cairo_t              *cr,
                  gint                  index,
                  const GspdfRectangle *image_dim,
                  const GspdfRectangle *surface_dim,
                  const GspdfRectangle *find_label,
                  GList                *selection);

static void
draw_page (GspdfPageData *page_data,
		       GtkWidget     *widget,
           cairo_t       *cr);

static void
update_page (GspdfPageData *page_data);

static void
scroll_page (GspdfPageData *page_data);

static void
update_hscroll_page_size (GspdfPageData *page_data,
                          gdouble        page);

static void
update_vscroll_page_size (GspdfPageData *page_data,
                          gdouble        page);

static void
update_hscroll_range (GspdfPageData *page_data,
                      gdouble        start,
                      gdouble        end);

static void
update_vscroll_range (GspdfPageData *page_data,
                      gdouble        start,
                      gdouble        end);

static void
update_hscroll_value (GspdfPageData *page_data,
                      gdouble        val);

static void
update_vscroll_value (GspdfPageData *page_data,
                      gdouble        val);

static gdouble
get_hscroll_value (GspdfPageData *page_data);

static gdouble
get_vscroll_value (GspdfPageData *page_data);

static gdouble
get_hscroll_upper (GspdfPageData *page_data);

static gdouble
get_vscroll_upper (GspdfPageData *page_data);

static void
update_hscroll_value_block (GspdfPageData *page_data,
                            gdouble        val);

static void
update_vscroll_value_block (GspdfPageData *page_data,
                            gdouble        val);

static void
next_page (GspdfPageData *page_data);

static void
prev_page (GspdfPageData *page_data);

static void
first_page (GspdfPageData *page_data);

static void
last_page (GspdfPageData *page_data);

static void
goto_page (GspdfPageData *page_data,
		       gint           index);

static void
goto_page_at_pos (GspdfPageData *page_data,
	                gint           index,
	                gdouble        x,
	                gdouble        y);

static void
process_action (GspdfPageData  *page_data,
                GspdfDocAction *action);

static void
find_text (GspdfPageData  *page_data,
           const gchar    *text,
           GspdfFindFlags  options);

static void
clear_find (GspdfPageData *page_data);

static void
show_page_popup (GspdfPageData  *page_data,
				         GdkEventButton *event);

static void
show_bookmark_popup (GspdfPageData  *page_data,
				             GdkEventButton *event);

static void
handle_document_error (GspdfPageData  *page_data,
					             GError         *error);

/* signal declaration */

static void
on_destroy (GtkWidget *widget,
			      gpointer user_data);

static void
on_notebook_page_added (GtkNotebook *notebook,
						            GtkWidget   *child,
						            guint        page_num,
						            gpointer     user_data);

static void
on_notebook_page_switch (GtkNotebook *notebook,
						             GtkWidget   *child,
						             guint        page_num,
						             gpointer     user_data);

static void
on_open_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_save_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_prop_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_close_menu_item_activate (GtkMenuItem *menuitem,
                             gpointer     user_data);

static void
on_exit_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_next_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_prev_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_first_menu_item_activate (GtkMenuItem *menuitem,
                             gpointer     user_data);

static void
on_last_menu_item_activate (GtkMenuItem *menuitem,
                            gpointer     user_data);

static void
on_cont_menu_item_toggled (GtkCheckMenuItem *checkmenuitem,
                           gpointer          user_data);

static void
on_zoomin_menu_item_activate (GtkMenuItem *menuitem,
                              gpointer     user_data);

static void
on_zoomout_menu_item_activate (GtkMenuItem *menuitem,
                               gpointer     user_data);

static void
on_zoomorg_menu_item_activate (GtkMenuItem *menuitem,
                               gpointer     user_data);

static void
on_zoomfitp_menu_item_activate (GtkMenuItem *menuitem,
                                gpointer     user_data);

static void
on_zoomfitw_menu_item_activate (GtkMenuItem *menuitem,
                                gpointer     user_data);

static void
on_markthis_menu_item_activate (GtkMenuItem *menuitem,
                                gpointer     user_data);

static void
on_index_entry_tool_item_activate (GtkEntry *entry,
                                   gpointer  user_data);

static void
on_open_tool_button_clicked (GtkToolButton *toolbutton,
                             gpointer       user_data);

static void
on_prev_tool_button_clicked (GtkToolButton *toolbutton,
                             gpointer       user_data);

static void
on_next_tool_button_clicked (GtkToolButton *toolbutton,
                             gpointer       user_data);

static void
on_zoomout_tool_button_clicked (GtkToolButton *toolbutton,
                                gpointer       user_data);

static void
on_zoomin_tool_button_clicked (GtkToolButton *toolbutton,
                               gpointer       user_data);

static void
on_zoomfit_tool_button_clicked (GtkToolButton *toolbutton,
                                gpointer       user_data);

static void
on_zoomorg_tool_button_clicked (GtkToolButton *toolbutton,
                                gpointer       user_data);

static void
on_find_entry_tool_item_activate (GtkEntry *entry,
                                  gpointer  user_data);

static void
on_find_tool_button_clicked (GtkToolButton *toolbutton,
                             gpointer       user_data);

static gboolean
on_page_drawing_area_draw (GtkWidget *widget,
                           cairo_t   *cr,
                           gpointer   user_data);

static gboolean
on_page_drawing_area_size_allocate (GtkWidget    *widget,
                                    GdkRectangle *allocation,
                                    gpointer      user_data);

static gboolean
on_page_drawing_area_leave (GtkWidget        *widget,
                            GdkEventCrossing *event,
                            gpointer          user_data);

static gboolean
on_page_drawing_area_motion (GtkWidget       *widget,
                             GdkEventMotion  *event,
                             gpointer         user_data);

static gboolean
on_page_drawing_button_press (GtkWidget       *widget,
                              GdkEventButton  *event,
                              gpointer         user_data);

static gboolean
on_page_drawing_button_release (GtkWidget       *widget,
                                GdkEventButton  *event,
                                gpointer         user_data);

static gboolean
on_page_drawing_scroll (GtkWidget       *widget,
                        GdkEventScroll  *event,
                        gpointer         user_data);

static void
on_hadj_value_changed (GtkAdjustment *adjustment,
                       gpointer       user_data);

static void
on_vadj_value_changed (GtkAdjustment *adjustment,
                       gpointer       user_data);

static void
on_page_cache_document_load_finished (GObject *object,
                                      gpointer user_data);

static void
on_page_cache_document_render_finished (GObject *object,
                                        gpointer user_data);

static gboolean
on_outline_treeview_button_press (GtkWidget       *widget,
                                  GdkEventButton  *event,
                                  gpointer         user_data);

static gboolean
on_outline_treeview_query_tooltip (GtkWidget  *widget,
                                   gint        x,
						                       gint        y,
                                   gboolean    keyboard_mode,
                                   GtkTooltip *tooltip,
                                   gpointer    user_data);

static gboolean
on_bookmark_treeview_button_press (GtkWidget       *widget,
                                   GdkEventButton  *event,
                                   gpointer         user_data);

static gboolean
on_window_key_press (GtkWidget   *widget,
                     GdkEventKey *event,
                     gpointer     user_data);

static void
on_copy_popup_menu_item_activate (GtkMenuItem *menuitem,
                                  gpointer     user_data);

static void
on_delete_popup_menu_item_activate (GtkMenuItem *menuitem,
                                    gpointer     user_data);

static void
on_about_menu_item_activate (GtkMenuItem *menuitem,
														 gpointer     user_data);

static void
on_g_list_rect_free_func (gpointer data)
{
	g_free ((GspdfRectangle*)data);
}

static void
str_replace (gchar *str,
			       gchar  target,
			       gchar  replace)
{
	for (gint i = 0;; i++) {
		if (str[i] == target) {
			str[i] = replace;
		} else if (str[i] == '\0') {
			return;
		}
	}
}

/*****************************************************************************/
/*                         DEFINITIONS                                       */
/*****************************************************************************/

/* register notebook(Tab)'s signal */
static void
_register_notebook_signal (GspdfApp *object)
{
	GtkWidget *notebook = NULL;
	g_object_get (G_OBJECT (object), "notebook", &notebook, NULL);
	g_object_unref (G_OBJECT (notebook));

	// when new a tab is created
	g_signal_connect (
		G_OBJECT (notebook),
		"page-added",
		G_CALLBACK (on_notebook_page_added),
		object
	);

	// swicth tab
	g_signal_connect (
		G_OBJECT (notebook),
		"switch-page",
		G_CALLBACK (on_notebook_page_switch),
		object
	);
}

/* register menu's signal */
static void
_register_menu_signal (GspdfApp *object)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);
	GtkWidget *menu = NULL;
	GtkWidget *ptr = NULL;
	g_object_get (G_OBJECT (object), "menu", &menu, NULL);
	g_object_unref (G_OBJECT (menu));

	// 'File' menu
	g_object_get (G_OBJECT (menu), "open-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_open_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Save' menu
	g_object_get (G_OBJECT (menu), "save-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_save_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Properties' menu
	g_object_get (G_OBJECT (menu), "prop-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_prop_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Close' menu
	g_object_get (G_OBJECT (menu), "close-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_close_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Exit' menu
	g_object_get (G_OBJECT (menu), "exit-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_exit_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Go to' menu
	g_object_get (G_OBJECT (menu), "next-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_next_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Prev page' menu
	g_object_get (G_OBJECT (menu), "prev-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_prev_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'First page' menu
	g_object_get (G_OBJECT (menu), "first-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_first_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Last page' menu
	g_object_get (G_OBJECT (menu), "last-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_last_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'continous page' menu
	g_object_get (G_OBJECT (menu), "cont-menu-item", &ptr, NULL);
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (ptr), TRUE);
	priv->signals[SIGNAL_WINDOW_CONTINUOUS] = g_signal_connect (
		G_OBJECT (ptr),
		"toggled",
		G_CALLBACK (on_cont_menu_item_toggled),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Zoom in page' menu
	g_object_get (G_OBJECT (menu), "zoomin-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_zoomin_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Zoom out page' menu
	g_object_get (G_OBJECT (menu), "zoomout-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_zoomout_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'zoom origin page' menu
	g_object_get (G_OBJECT (menu), "zoomorg-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_zoomorg_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Fit page' menu
	g_object_get (G_OBJECT (menu), "zoomfitp-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_zoomfitp_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'Fit width' menu
	g_object_get (G_OBJECT (menu), "zoomfitw-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_zoomfitw_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'mark this (as bookmark)' menu
	g_object_get (G_OBJECT (menu), "markthis-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_markthis_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));

	// 'About' menu
	g_object_get (G_OBJECT (menu), "about-menu-item", &ptr, NULL);
	g_signal_connect (
		G_OBJECT (ptr),
		"activate",
		G_CALLBACK (on_about_menu_item_activate),
		object
	);
	g_object_unref (G_OBJECT (ptr));
}

/* Register toolbar's signal */
static void
_register_toolbar_signal (GspdfApp *object)
{
	GtkWidget *toolbar = NULL;
	g_object_get (G_OBJECT (object), "toolbar", &toolbar, NULL);
	g_object_unref (G_OBJECT (toolbar));

	GtkWidget *index_entry = NULL;
	g_object_get (G_OBJECT (toolbar), "index-entry-tool-item", &index_entry, NULL);
	g_object_unref (G_OBJECT (index_entry));

	g_signal_connect (
		G_OBJECT (gtk_bin_get_child (GTK_BIN (index_entry))),
		"activate",
		G_CALLBACK (on_index_entry_tool_item_activate),
		object
	);

	GtkWidget *ptr = NULL;

	// open
	g_object_get (G_OBJECT (toolbar), "open-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_open_tool_button_clicked),
		object
	);

	// prev
	g_object_get (G_OBJECT (toolbar), "prev-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_prev_tool_button_clicked),
		object
	);

	// next
	g_object_get (G_OBJECT (toolbar), "next-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_next_tool_button_clicked),
		object
	);

	// zoomout
	g_object_get (G_OBJECT (toolbar), "zoomout-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_zoomout_tool_button_clicked),
		object
	);

	// zoomin
	g_object_get (G_OBJECT (toolbar), "zoomin-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_zoomin_tool_button_clicked),
		object
	);

	// zoomfit
	g_object_get (G_OBJECT (toolbar), "zoomfit-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_zoomfit_tool_button_clicked),
		object
	);

	// zoomorg
	g_object_get (G_OBJECT (toolbar), "zoomorg-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_zoomorg_tool_button_clicked),
		object
	);

	// find entry
	g_object_get (G_OBJECT (toolbar), "find-entry-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (gtk_bin_get_child (GTK_BIN (ptr))),
		"activate",
		G_CALLBACK (on_find_entry_tool_item_activate),
		object
	);

	// find
	g_object_get (G_OBJECT (toolbar), "find-tool-item", &ptr, NULL);
	g_object_unref (G_OBJECT (ptr));

	g_signal_connect (
		G_OBJECT (ptr),
		"clicked",
		G_CALLBACK (on_find_tool_button_clicked),
		object
	);

}

/* register sidebar's signal */
static void
_register_sidebar_signal (GspdfApp *object)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);

	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (object), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);

	GtkWidget *outline = NULL;
	g_object_get (G_OBJECT (sidebar), "outline", &outline, NULL);
	g_object_unref (outline);

	GtkWidget *bookmark = NULL;
	g_object_get (G_OBJECT (sidebar), "bookmark", &bookmark, NULL);
	g_object_unref (bookmark);

	GtkTreeSelection *outline_sel = gtk_tree_view_get_selection (
		GTK_TREE_VIEW (outline)
	);

	GtkTreeSelection *bookmark_sel = gtk_tree_view_get_selection (
		GTK_TREE_VIEW (bookmark)
	);

	gtk_tree_selection_set_mode (outline_sel, GTK_SELECTION_SINGLE);
	gtk_tree_selection_set_mode (bookmark_sel, GTK_SELECTION_SINGLE);
	g_object_set (G_OBJECT (outline), "has-tooltip", TRUE, NULL);

	gtk_widget_add_events (
		outline,
		gtk_widget_get_events (outline) |
		GDK_BUTTON_PRESS_MASK
	);

	gtk_widget_add_events (
		bookmark,
		gtk_widget_get_events (bookmark) |
		GDK_BUTTON_PRESS_MASK
	);

	priv->signals[SIGNAL_WINDOW_OUTLINE] = g_signal_connect (
		G_OBJECT (outline),
		"button-press-event",
		G_CALLBACK (on_outline_treeview_button_press),
		object
	);

	g_signal_connect (
		G_OBJECT (outline),
		"query-tooltip",
		G_CALLBACK (on_outline_treeview_query_tooltip),
		object
	);

	priv->signals[SIGNAL_WINDOW_BOOKMARK] = g_signal_connect (
		G_OBJECT (bookmark),
		"button-press-event",
		G_CALLBACK (on_bookmark_treeview_button_press),
		object
	);
}

static GspdfPagePopup *
_init_page_popup ()
{
	GspdfPagePopup *ret = g_malloc0 (sizeof (GspdfPagePopup));
	ret->menu = gtk_menu_new ();
	ret->copy = gtk_menu_item_new_with_label("Copy");

	gtk_widget_show (ret->copy);
	gtk_menu_shell_append (GTK_MENU_SHELL (ret->menu), ret->copy);

	return ret;
}

static GspdfBookmarkPopup *
_init_bookmark_popup ()
{
	GspdfBookmarkPopup *ret = g_malloc0 (sizeof (GspdfBookmarkPopup));
	ret->menu = gtk_menu_new ();
	ret->delete = gtk_menu_item_new_with_label("Delete");

	gtk_widget_show (ret->delete);
	gtk_menu_shell_append (GTK_MENU_SHELL (ret->menu), ret->delete);

	return ret;
}

static void
gspdf_app_init (GspdfApp *object)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);

	GtkWidget *notebook = NULL;
	g_object_get (G_OBJECT (object), "notebook", &notebook, NULL);
	g_object_unref (G_OBJECT (notebook));

	priv->page_popup = _init_page_popup ();
	priv->bookmark_popup = _init_bookmark_popup ();

	load_config (object);
	_register_menu_signal (object);
	_register_notebook_signal (object);
	_register_toolbar_signal (object);
	_register_sidebar_signal (object);

	g_signal_connect (
		G_OBJECT (priv->page_popup->copy),
		"activate",
		G_CALLBACK (on_copy_popup_menu_item_activate),
		object
	);

	g_signal_connect (
		G_OBJECT (priv->bookmark_popup->delete),
		"activate",
		G_CALLBACK (on_delete_popup_menu_item_activate),
		object
	);

	g_signal_connect (
		G_OBJECT (object),
		"key-press-event",
		G_CALLBACK (on_window_key_press),
		NULL
	);

	g_signal_connect (
		G_OBJECT (object),
		"destroy",
		G_CALLBACK (on_destroy),
		NULL
	);

	gtk_window_set_default_size (GTK_WINDOW (object), 800, 600);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), gspdf_page_new (), NULL);
}

static void
gspdf_app_class_init (GspdfAppClass *klass)
{
	//GObjectClass *object_class = G_OBJECT_CLASS (klass);
}

static void
reset_menu (GspdfApp *object)
{
	GtkWidget *menu = NULL;
	g_object_get (G_OBJECT (object), "menu", &menu, NULL);
	g_object_unref (G_OBJECT (menu));

	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);
	GspdfPageData *page_data = get_current_page_data (object);
	gboolean opened = (page_data->document != NULL) ? TRUE : FALSE;
	GtkWidget *ptr = NULL;

	g_object_get (G_OBJECT (menu), "save-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "prop-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "close-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "next-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "prev-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "first-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "last-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "cont-menu-item", &ptr, NULL);
	g_signal_handler_block (ptr, priv->signals[SIGNAL_WINDOW_CONTINUOUS]);
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (ptr), page_data->continuous);
	g_signal_handler_unblock (ptr, priv->signals[SIGNAL_WINDOW_CONTINUOUS]);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "zoomin-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "zoomout-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "zoomorg-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "zoomfitp-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "zoomfitw-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	g_object_get (G_OBJECT (menu), "markthis-menu-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

}

static void
reset_toolbar (GspdfApp *object)
{
	GtkWidget *toolbar = NULL;
	g_object_get (G_OBJECT (object), "toolbar", &toolbar, NULL);
	g_object_unref (G_OBJECT (toolbar));

	GspdfPageData *page_data = get_current_page_data (object);
	gboolean opened = (page_data->document != NULL) ? TRUE : FALSE;
	GtkWidget *ptr = NULL;

	// index
	g_object_get (G_OBJECT (toolbar), "index-entry-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// prev
	g_object_get (G_OBJECT (toolbar), "prev-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// next
	g_object_get (G_OBJECT (toolbar), "next-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// zoomout
	g_object_get (G_OBJECT (toolbar), "zoomout-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// zoomin
	g_object_get (G_OBJECT (toolbar), "zoomin-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// zoomfit
	g_object_get (G_OBJECT (toolbar), "zoomfit-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// zoomorg
	g_object_get (G_OBJECT (toolbar), "zoomorg-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// find entry
	g_object_get (G_OBJECT (toolbar), "find-entry-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

	// find
	g_object_get (G_OBJECT (toolbar), "find-tool-item", &ptr, NULL);
	gtk_widget_set_sensitive (ptr, opened);
	g_object_unref (G_OBJECT (ptr));

}

static void
load_config (GspdfApp *object)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);

	if (priv->config) {
		g_key_file_unref (priv->config);
		priv->config = NULL;
	}

	priv->config = g_key_file_new ();
	GError *err = NULL;

	if (!g_key_file_load_from_file (priv->config, "config", G_KEY_FILE_NONE, &err)) {
		if (err) {
			g_error_free (err);
		}
	}
}

static void
save_config (GspdfApp *object)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (object);

	if (!priv->config) {
		return;
	}

	GError *err = NULL;

	if (!g_key_file_save_to_file (priv->config, "config", &err)) {
		if (err) {
			g_error_free (err);
		}
	}
}

static GtkWidget *
get_current_page (GspdfApp *object)
{
	GtkWidget *notebook = NULL;
	g_object_get (G_OBJECT (object), "notebook", &notebook, NULL);
	g_object_unref (notebook);

	return gtk_notebook_get_nth_page (
		GTK_NOTEBOOK (notebook),
		gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook))
	);
}

static GspdfPageData *
get_current_page_data (GspdfApp *object)
{
	GspdfPageData *page_data = NULL;

	g_object_get (
		G_OBJECT (get_current_page (object)),
		"user-data",
		&page_data,
		NULL
	);

	return page_data;
}

static gint
get_n_pages (GspdfApp *object)
{
	GtkWidget *notebook = NULL;
	g_object_get (G_OBJECT (object), "notebook", &notebook, NULL);
	g_object_unref (notebook);

	return gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook));
}

static void
update_index_toolbar (GspdfApp *object)
{
	GspdfPageData *page_data = get_current_page_data (object);

	GtkWidget *toolbar = NULL;
	g_object_get (object, "toolbar", &toolbar, NULL);
	g_object_unref (toolbar);

	GtkWidget *index_entry = NULL;
	g_object_get (toolbar, "index-entry-tool-item", &index_entry, NULL);
	g_object_unref (index_entry);

	GtkWidget *index_label = NULL;
	g_object_get (toolbar, "index-label-tool-item", &index_label, NULL);
	g_object_unref (index_label);

	if (page_data->document) {
		gchar *temp = g_strdup_printf ("%d", page_data->index + 1);

		gtk_entry_set_text (
			GTK_ENTRY (gtk_bin_get_child (GTK_BIN (index_entry))),
			temp
		);
		g_free (temp);

		temp = g_strdup_printf (
			" of %d",
			gspdf_document_get_n_pages (page_data->document)
		);

		gtk_label_set_text (
			GTK_LABEL (gtk_bin_get_child (GTK_BIN (index_label))),
			temp
		);
		g_free (temp);
	} else {
		gtk_entry_set_text (
			GTK_ENTRY (gtk_bin_get_child (GTK_BIN (index_entry))),
			"0"
		);

		gtk_label_set_text (
			GTK_LABEL (gtk_bin_get_child (GTK_BIN (index_label))),
			" of 0"
		);
	}
}

static gboolean
_bookmark_iter_foreach_func (GtkTreeModel *model,
                             GtkTreePath *path,
                             GtkTreeIter *iter,
                             gpointer     data)
{
	GList **indexs = (GList**) data;
	gint index = 0;
	gtk_tree_model_get (model, iter, 1, &index, -1);
	*indexs = g_list_append (*indexs, GINT_TO_POINTER (index));

	return FALSE;
}

static void
update_config_bookmark (GspdfPageData *page_data)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (
		GSPDF_APP (page_data->window)
	);

	if (!priv->config) {
		return;
	}

	gchar *uri = gspdf_page_cache_get_uri (page_data->page_cache);

	if (!uri) {
		return;
	}

	GList *indexs = NULL;
	gtk_tree_model_foreach (
		GTK_TREE_MODEL (page_data->bookmark),
		_bookmark_iter_foreach_func,
		&indexs
	);

	if (!indexs) {
		g_key_file_remove_key (priv->config, "bookmark", uri, NULL);
		g_free (uri);
		return;
	}

	GList *indexs_iter = indexs;
	gint *in = g_malloc0 (sizeof (gint) * g_list_length (indexs));
	gint i = 0;

	while (indexs_iter) {
		in[i] = GPOINTER_TO_INT (indexs_iter->data);
		indexs_iter = indexs_iter->next;
		i++;
	}

	g_key_file_set_integer_list (priv->config, "bookmark", uri, in, i);
	g_list_free (indexs);
	g_free (in);
	g_free (uri);
}

static void
update_config_cache (GspdfPageData *page_data)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private
		(GSPDF_APP (page_data->window));

	if (!page_data->document) {
		return;
	}

	gchar *uri = gspdf_page_cache_get_uri (page_data->page_cache);
	if (!uri) {
		return;
	}

	gchar *index = g_strdup_printf ("%d", page_data->index);
	gchar *scale = g_strdup_printf ("%f", page_data->scale);
	gchar *scale_mode = g_strdup_printf ("%d", page_data->scale_mode);
	gchar *hprcnt = g_strdup_printf ("%f", page_data->hadj_val_prcnt);
	gchar *vprcnt = g_strdup_printf ("%f", page_data->vadj_val_prcnt);
	gchar *cont = g_strdup_printf ("%d", (page_data->continuous) ? 1 : 0);
	str_replace (scale, ',', '.');
	str_replace (hprcnt, ',', '.');
	str_replace (vprcnt, ',', '.');
	const gchar *const cache[] = {
		index, scale, scale_mode, hprcnt, vprcnt, cont
	};
	g_key_file_set_string_list (
		priv->config,
		"cache",
		uri, cache, 6
	);
	g_free (uri);
	g_free (index);
	g_free (scale);
	g_free (scale_mode);
	g_free (hprcnt);
	g_free (vprcnt);
	g_free (cont);
}

static void
restore_from_config_cache (GspdfPageData *page_data)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private
		(GSPDF_APP (page_data->window));

	if (!page_data->document) {
		return;
	}

	gchar *uri = gspdf_page_cache_get_uri (page_data->page_cache);
	if (!uri) {
		return;
	}

	gchar **cache = g_key_file_get_string_list (
		priv->config,
		"cache",
		uri,
		NULL,
		NULL
	);

	if (cache) {
		page_data->index = (gint) g_ascii_strtoll (cache[CONFIG_CACHE_INDEX], NULL, 10);
		page_data->scale = g_ascii_strtod (cache[CONFIG_CACHE_SCALE], NULL);
		page_data->scale_mode = (gint) g_ascii_strtoll (cache[CONFIG_CACHE_SCALEMODE], NULL, 10);
		page_data->hadj_val_prcnt = g_ascii_strtod (cache[CONFIG_CACHE_HPRCNT], NULL);
		page_data->vadj_val_prcnt = g_ascii_strtod (cache[CONFIG_CACHE_VPRCNT], NULL);
		page_data->continuous = ((gint) g_ascii_strtoll (cache[CONFIG_CACHE_CONTINUOUS], NULL, 10) == 1) ? TRUE : FALSE;
		g_strfreev (cache);
	}

	g_free (uri);
}

static void
open_document (GspdfPageData       *page_data,
			   			 const gchar         *uri,
			         const gchar         *password)
{
	if (page_data->document) {
		close_document (page_data);
	}

	gspdf_page_cache_open_document (page_data->page_cache, uri, password);
}

static void
close_document (GspdfPageData *page_data)
{
	update_config_cache (page_data);
	save_config (GSPDF_APP (page_data->window));

	if (page_data->document) {
		g_object_unref (page_data->document);
		page_data->document = NULL;
	}

	if (page_data->doc_map) {
		g_ptr_array_unref (page_data->doc_map);
		page_data->doc_map = NULL;
	}

	page_data->index = 0;
	page_data->continuous = DEFAULT_CONTINUOUS_VALUE;
	page_data->hadj_val_prcnt = 0;
	page_data->vadj_val_prcnt = 0;
	update_hscroll_range (page_data, 0, 0);
  update_vscroll_range (page_data, 0, 0);
  update_hscroll_value_block (page_data, 0);
  update_vscroll_value_block (page_data, 0);
  gtk_tree_store_clear (page_data->outline);
  gtk_tree_store_clear (page_data->bookmark);
  reset_menu (GSPDF_APP (page_data->window));
  reset_toolbar (GSPDF_APP (page_data->window));
}

static void
_walk_outline (GspdfDocOutline *outline,
			   			 GtkTreeStore    *store,
			         GtkTreeIter     *tree_iter,
			         GtkTreeIter     *tree_iter_parent)
{
	GspdfDocOutline *iter = outline;

	while (iter) {
		gtk_tree_store_set (
			store,
			tree_iter,
			0, ((GspdfDocActionAny*) iter->action)->title,
			1, gspdf_doc_action_copy (iter->action),
			-1
		);

		if (iter->child) {
			GtkTreeIter tree_iter_child;
			gtk_tree_store_append (store, &tree_iter_child, tree_iter);
			_walk_outline (iter->child, store, &tree_iter_child, tree_iter);
		}

		iter = iter->next;
		if (iter) {
			gtk_tree_store_append (store, tree_iter, tree_iter_parent);
		}
	}
}

static void
fill_outline (GspdfPageData *page_data, GspdfDocument *doc)
{
	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (page_data->window), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);
	GtkWidget *treeview = NULL;
	g_object_get (G_OBJECT (sidebar), "outline", &treeview, NULL);
	g_object_unref (treeview);
	gtk_tree_store_clear (page_data->outline);
	GspdfDocOutline *outline = gspdf_document_get_outline (doc);

	if (!outline) {
		return;
	}

	GtkTreeIter iter;
	gtk_tree_store_append (page_data->outline, &iter, NULL);
	_walk_outline (outline, page_data->outline, &iter, NULL);
	gspdf_doc_outline_free (outline);
}

static void
fill_bookmark (GspdfPageData *page_data)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (
		GSPDF_APP (page_data->window)
	);

	if (!priv->config) {
		return;
	}

	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (page_data->window), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);
	GtkWidget *treeview = NULL;
	g_object_get (G_OBJECT (sidebar), "bookmark", &treeview, NULL);
	g_object_unref (treeview);
	gtk_tree_store_clear (page_data->bookmark);
	gchar *uri = gspdf_page_cache_get_uri (page_data->page_cache);

	if (!uri) {
		return;
	}

	gsize len = 0;
	gint *indexs = g_key_file_get_integer_list (
		priv->config, "bookmark", uri, &len, NULL
	);

	if (indexs) {
		GtkTreeIter iter;
		gchar *text = NULL;

		for (gint i = 0; i < len; i++) {
			text = g_strdup_printf ("Page %d", indexs[i]);
			gtk_tree_store_append (page_data->bookmark, &iter, NULL);
			gtk_tree_store_set (
				page_data->bookmark, &iter, 0, text, 1, indexs[i], -1
			);
			g_free (text);
		}
	}

	g_free (uri);
}

static gdouble
get_page_allocated_width (GspdfPageData *page_data)
{
	GtkWidget *drawing_area = NULL;
	g_object_get (G_OBJECT (page_data->page), "drawing_area", &drawing_area, NULL);
	g_object_unref (drawing_area);

	return gtk_widget_get_allocated_width (drawing_area);
}

static gdouble
get_page_allocated_height (GspdfPageData *page_data)
{
	GtkWidget *drawing_area = NULL;
	g_object_get (G_OBJECT (page_data->page), "drawing_area", &drawing_area, NULL);
	g_object_unref (drawing_area);

	return gtk_widget_get_allocated_height (drawing_area);
}

static gdouble
get_page_maximum_width (GspdfPageData *page_data)
{
	if (page_data->doc_map == NULL) {
		return -1;
	}

	gdouble ret = 0;

	for (gint i = 0; i < page_data->doc_map->len; i++) {
		if ((((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->width) > ret) {
			ret = ((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->width;
		}
	}

	return ret;
}

static gdouble
get_page_maximum_height (GspdfPageData *page_data)
{
	if (page_data->doc_map == NULL) {
		return -1;
	}

	gdouble ret = 0;

	for (gint i = 0; i < page_data->doc_map->len; i++) {
		if ((((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->height) > ret) {
			ret = ((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->height;
		}
	}

	return ret;
}

static gdouble
get_page_total_height (GspdfPageData *page_data)
{
	if (page_data->doc_map == NULL) {
		return -1;
	}

	gdouble ret = 0;

	for (gint i = 0; i < page_data->doc_map->len; i++) {
		ret +=
			((((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->height) * page_data->scale);
		ret += page_data->spacing;
	}

	return ret;
}

static gdouble
get_page_y_offset (GspdfPageData *page_data,
				   				 gint           index)
{
	if (page_data->doc_map == NULL) {
		return -1;
	}

	gdouble ret = 0;

	for (gint i = 1; i <= index; i++) {
		ret +=
			((((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i-1))->height) * page_data->scale);

		ret += page_data->spacing;
	}

	return ret;
}

static gboolean
get_current_pointer_position (GspdfPageData *page_data,
							                gdouble        x_win,
							                gdouble        y_win,
							                gint          *index,
							                gdouble       *x_point,
							                gdouble       *y_point)
{
	if (page_data->doc_map == NULL) {
		return FALSE;
	}

	GtkWidget *drawing_area = NULL;
	g_object_get (G_OBJECT (page_data->page), "drawing_area", &drawing_area, NULL);
	g_object_unref (drawing_area);
	const gdouble width = gtk_widget_get_allocated_width (drawing_area);
	const gdouble height = gtk_widget_get_allocated_height (drawing_area);
	const gdouble x_scroll = get_hscroll_value (page_data);
	const gdouble y_scroll = get_vscroll_value (page_data);
	gint in = -1;
	gdouble x = -1, y =-1;
	gboolean ret = FALSE;

	if (page_data->continuous) {
		const gdouble max_width = get_page_maximum_width (page_data) * page_data->scale;
		GspdfDocMap *doc_map = NULL;
		gdouble page_width = 0, page_height = 0;
		gdouble pos = 0;

		for (gint i = 0; i < page_data->doc_map->len; i++) {
			doc_map = (GspdfDocMap*) g_ptr_array_index (page_data->doc_map, i);
			page_width = doc_map->width * page_data->scale;
			page_height =  (doc_map->height * page_data->scale) + page_data->spacing;
			pos += page_height;

			if ((y_scroll + y_win) < pos) {
				const gdouble offset_y = y_scroll - get_page_y_offset (page_data, i);
				in = i;
				if (max_width > width) {
					if (page_width < max_width) {
						const gdouble abs_width = ((max_width - page_width)/2) - x_scroll;
						if (page_width > width) {
							x =  x_win - abs_width;
						} else {
							if ((x_win > abs_width) && (x_win < (abs_width + page_width))) {
								x =  x_win - abs_width;
							}
						}
					} else {
						x = x_scroll + x_win;
					}
				} else {
					const gdouble abs_width = (width - page_width)/2;
					if ((x_win > abs_width) && (x_win < (abs_width + page_width))) {
						x = x_win - abs_width;
					}
				}
				y = offset_y + y_win;
				ret = TRUE;
				break;
			}
		}
	} else {
		const GspdfDocMap *doc_map = (GspdfDocMap*) g_ptr_array_index (
			page_data->doc_map,
			page_data->index
		 );
		const gdouble page_width = doc_map->width * page_data->scale;
		const gdouble page_height = doc_map->height * page_data->scale;
		in = page_data->index;

		if (page_width > width) {
			x = x_scroll + x_win;
		} else {
			const gdouble abs_width = (width - page_width)/2;
			if ((x_win > abs_width) && (x_win < (abs_width + page_width))) {
				x = x_win - abs_width;
			}
		}

		if (page_height > height) {
			y = y_scroll + y_win;
		} else {
			const gdouble abs_height = (height - page_height)/2;
			if ((y_win > abs_height) && (y_win < (abs_height + page_height))) {
				y = y_win - abs_height;
			}
		}

		ret = TRUE;
	}

	if (index) { *index = in; }
	if (x_point) { *x_point = x; }
	if (y_point) { *y_point = y; }

	return ret;
}

static gboolean
get_index_page_area (GspdfPageData        *page_data,
					 					 const GspdfRectangle *area,
					 					 gint                 *start,
				             gint                 *end)
{
	if (page_data->doc_map == NULL) {
		return FALSE;
	}

	gint s = -1, e = -1;
	gdouble pos = 0;
	gboolean ret = FALSE;

	for (gint i = 0; i < page_data->doc_map->len; i++) {

		pos +=
			((((GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i))->height) * page_data->scale);

		pos += page_data->spacing;

		if (area->y <= pos) {

			if (s < 0) { s = i; }

			if (!ret) { ret = TRUE; }

			if ((area->y + area->height) <= pos) {

				if (e < 0) { e = i; }

				break;

			}
		}
	}

	if (start) { *start = s; }
	if (end) { *end = e; }

	return ret;
}

static gboolean
get_selected_area (GspdfPageData        *page_data,
				   const GspdfRectangle *selection,
				   gint                 *start,
				   gint                 *end,
				   GspdfRectangle       *area)
{
	if (!page_data->doc_map) {
		return FALSE;
	}

	if (!selection) {
		return FALSE;
	}

	GtkWidget *drawing_area = NULL;
	g_object_get (G_OBJECT (page_data->page), "drawing_area", &drawing_area, NULL);
	g_object_unref (drawing_area);

	const gdouble width = gtk_widget_get_allocated_width (drawing_area);
	const gdouble height = gtk_widget_get_allocated_height (drawing_area);
	const gdouble x_scroll = get_hscroll_value (page_data);
	const gdouble y_scroll = get_vscroll_value (page_data);

	gint s = -1, e = -1;
	gdouble x = -1, y = -1, w = -1, h = -1;
	gboolean ret = FALSE;

	if (page_data->continuous) {

		const gdouble max_width = floor
			(get_page_maximum_width (page_data) * page_data->scale);

		GspdfDocMap *doc_map = NULL;
		gdouble page_width = 0, page_height = 0;
		gdouble pos = 0;

		for (gint i = 0; i < page_data->doc_map->len; i++) {

			doc_map = (GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i);
			page_width = doc_map->width * page_data->scale;
			page_height =  (doc_map->height * page_data->scale) + page_data->spacing;

			pos += page_height;

			if ((y_scroll + selection->y) < pos) {

				// start index

				if (s < 0) {

					s = i;

					// compute x

					if (max_width > width) {

						const gdouble abs_width = ((max_width - page_width)/2) - x_scroll;

						if (page_width < max_width) {

							if (page_width > width) {

								x =  selection->x - abs_width;

							} else {

								x = (selection->x >= abs_width) ?
									selection->x - abs_width : 0;
							}

						} else {

							x = x_scroll + selection->x;

						}

					} else {

						const gdouble abs_width = (width - page_width)/2;

						x = (selection->x >= abs_width) ?
							(selection->x - abs_width) + x_scroll : x_scroll;

					}

					// compute y

					y = ((y_scroll - get_page_y_offset (page_data, i)) + selection->y);

				}

				if ((y_scroll + selection->y + selection->height) < pos) {

					if (e < 0) {

						e = i;

						// compute w

						if (selection->width < page_width) {

							w = selection->width;

						} else {

							w = page_width;

						}

						// compute h

						h = selection->height;

					}

					ret = TRUE;
					break;

				}

			}

		}

	} else {

		const GspdfDocMap *doc_map = (GspdfDocMap*)g_ptr_array_index
			(page_data->doc_map, page_data->index);

		const gdouble page_width = doc_map->width * page_data->scale;

		const gdouble page_height = doc_map->height * page_data->scale;

		s = page_data->index;
		e = page_data->index;

		if (page_width > width) {

			x = x_scroll + selection->x;

		} else {

			x = (selection->x >= ((width - page_width)/2)) ?
				(selection->x - ((width - page_width)/2)) : 0;

		}

		if (page_height > height) {

			y = y_scroll + selection->y;

		} else {

			y = (selection->y >= ((height - page_height)/2)) ?
				(selection->y - ((height - page_height)/2)) : 0;

		}

		w = selection->width;
		h = selection->height;

		ret = TRUE;

	}

	if (start) { *start = s; }
	if (end) { *end = e; }

	if (area) {

		area->x = x / page_data->scale;
		area->y = y / page_data->scale;
		area->width = w / page_data->scale;
		area->height = h / page_data->scale;

	}

	return ret;
}

static GList *
get_selected_page_area (GspdfPageData        *page_data,
					    const GspdfRectangle *area)
{
	if (!page_data->document) {
		return NULL;
	}


	gint start = -1, end = -1;
	GspdfRectangle selection;

	if (!get_selected_area (page_data, area, &start, &end, &selection)) {
		return NULL;
	}

	GspdfRectangle temp;
	GspdfDocMap *doc_map = NULL;
	GspdfPageSelection *ps = NULL;
	GList *sel = NULL;
	GList *ret = NULL;
	gdouble pos = selection.height;

	for (gint i = start; i <= end; i++) {

		doc_map = (GspdfDocMap*) g_ptr_array_index (page_data->doc_map, i);

		temp.x = 0;
		temp.y = 0;
		temp.width = doc_map->width;
		temp.height = doc_map->height;

		if (i == start) {

			temp.x = selection.x;
			temp.y = selection.y;

		}

		if (i == end) {

			temp.width = (i == start) ? selection.width : (selection.x + selection.width);
			temp.height = (i == start) ? selection.height : pos;

		}

		pos -= ((doc_map->height + page_data->spacing) - temp.y);

		sel = gspdf_page_cache_get_selected_region (
			page_data->page_cache,
			i,
			GSPDF_SELECTION_GLYPH,
			&temp
		);

		if (sel) {

			ps = g_malloc (sizeof (GspdfPageSelection));
			ps->index = i;
			ps->selection = sel;

			ret = g_list_append (ret, ps);
		}

	}

	return ret;
}

static gchar *
get_selected_text_area (GspdfPageData        *page_data,
					    const GspdfRectangle *area)
{
	if (!page_data->document) {
		return NULL;
	}


	gint start = -1, end = -1;
	GspdfRectangle selection;

	if (!get_selected_area (page_data, area, &start, &end, &selection)) {
		return NULL;
	}

	GspdfRectangle temp;
	GspdfDocMap *doc_map = NULL;
	gdouble pos = selection.height;

	GString *string = g_string_new (NULL);
	gchar *text = NULL;

	for (gint i = start; i <= end; i++) {

		doc_map = (GspdfDocMap*)g_ptr_array_index (page_data->doc_map, i);

		temp.x = 0;
		temp.y = 0;
		temp.width = doc_map->width;
		temp.height = doc_map->height;

		if (i == start) {

			temp.x = selection.x;
			temp.y = selection.y;

		}

		if (i == end) {

			temp.width = (i == start) ? selection.width : (selection.x + selection.width);
			temp.height = (i == start) ? selection.height : pos;

		}

		pos -= ((doc_map->height + page_data->spacing) - temp.y);

		text = gspdf_page_cache_get_selected_text (
			page_data->page_cache,
			i,
			GSPDF_SELECTION_GLYPH,
			&temp
		);

		if (text) {

			g_string_append (string, text);
			g_string_append_c (string, '\n');

			g_free (text);

		}

	}

	return g_string_free (string, FALSE);
}

static void
update_page_range (GspdfPageData *page_data,
				           gint           start,
				           gint           end)
{
	gspdf_page_cache_set_range (page_data->page_cache, start, end, page_data->scale);
}

static void
update_page_range_area (GspdfPageData        *page_data,
					              const GspdfRectangle *area)
{
	if (page_data->doc_map == NULL) {
		return;
	}

	gint start = -1, end = -1;

	if (!get_index_page_area (page_data, area, &start, &end)) {
		return;
	}

	if (end == -1) { end = start; }

	page_data->index = start;
	update_page_range (page_data, start, end);
}

static void
draw_single_page (GspdfPageData        *page_data,
				  GtkWidget            *widget,
                  cairo_t              *cr,
                  gint                  index,
                  const GspdfRectangle *image_dim,
                  const GspdfRectangle *surface_dim,
                  const GspdfRectangle *find_label,
                  GList                *selection)
{

	// draw a white blank page

	cairo_set_source_rgb (cr, 1, 1, 1);

	cairo_rectangle (
		cr,
		surface_dim->x,
		surface_dim->y,
		image_dim->width,
		image_dim->height
	);

	cairo_fill (cr);

	// draw a page

	GdkPixbuf *pixbuf = gspdf_page_cache_get_pixbuf (page_data->page_cache, index);

	if (pixbuf) {

		cairo_surface_t *image_surface = cairo_image_surface_create_for_data (
			gdk_pixbuf_get_pixels (pixbuf),
			CAIRO_FORMAT_ARGB32,
			gdk_pixbuf_get_width (pixbuf),
			gdk_pixbuf_get_height (pixbuf),
			gdk_pixbuf_get_width (pixbuf) * 4
		);

		cairo_surface_t *surface = cairo_surface_create_for_rectangle (
			image_surface,
			image_dim->x,
			image_dim->y,
			image_dim->width,
			image_dim->height
		);

		cairo_set_source_surface (cr, surface, surface_dim->x, surface_dim->y);

		cairo_paint (cr);

		// draw selection
		GList *iter = selection;
		GspdfRectangle *rect = NULL;

		while (iter) {
			rect = (GspdfRectangle*) iter->data;

			cairo_set_source_rgba (cr, 0.4, 0.698, 1.0, 0.5);
			cairo_rectangle (
				cr,
				((rect->x * page_data->scale) - image_dim->x) + surface_dim->x,
				((rect->y * page_data->scale) - image_dim->y) + surface_dim->y,
				rect->width * page_data->scale,
				rect->height * page_data->scale
			);
			cairo_fill (cr);

			iter = iter->next;
		}

		// draw a find label
		if (find_label) {
			cairo_set_source_rgba (cr, 1.0, 1.0, 0.0, 0.5);
			cairo_rectangle (
				cr,
				((find_label->x * page_data->scale) - image_dim->x) + surface_dim->x,
				((find_label->y * page_data->scale) - image_dim->y) + surface_dim->y,
				find_label->width * page_data->scale,
				find_label->height * page_data->scale
			);
			cairo_fill (cr);

		}

		cairo_surface_destroy (surface);

		cairo_surface_destroy (image_surface);

		g_object_unref (pixbuf);

	}
}

static void
draw_page (GspdfPageData *page_data,
		   GtkWidget     *widget,
           cairo_t       *cr)
{
	GtkWidget *hscroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &hscroll, NULL);
	GtkAdjustment *hadv = gtk_range_get_adjustment (GTK_RANGE (hscroll));
	g_object_unref (hscroll);

	GtkWidget *vscroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &vscroll, NULL);
	GtkAdjustment *vadv = gtk_range_get_adjustment (GTK_RANGE (vscroll));
	g_object_unref (vscroll);

	const gint alloc_width = gtk_widget_get_allocated_width (widget);
	const gint alloc_height = gtk_widget_get_allocated_height (widget);
	const gdouble scroll_x = gtk_adjustment_get_value (hadv);
	const gdouble scroll_y = gtk_adjustment_get_value (vadv);

	// continuous mode

	if (page_data->continuous) {

		GspdfDocMap *doc_map = NULL;
		gint page_width = 0, page_height = 0;
		gint start = 0, end = 0;
		gdouble pos_y = 0;

		GspdfRectangle image_dim, surface_dim;

		gspdf_page_cache_get_range (page_data->page_cache, &start, &end);

		const gdouble max_width = floor (get_page_maximum_width (page_data) * page_data->scale);
		const gint offset = scroll_y - get_page_y_offset (page_data, start);

		for (gint i = start; i <= end; i++) {

			doc_map = (GspdfDocMap*) g_ptr_array_index (page_data->doc_map, i);

			page_width = floor (doc_map->width) * page_data->scale;
			page_height = floor (doc_map->height) * page_data->scale;

			if (max_width > alloc_width) {

				if (page_width < max_width) {

					image_dim.x = 0;
					image_dim.y = (i == start) ? offset : 0;
					image_dim.width = page_width - image_dim.x;
					image_dim.height = page_height - image_dim.y;

					surface_dim.x = ((max_width - page_width)/2) - scroll_x;

				} else {

					image_dim.x = (page_width > alloc_width) ? scroll_x : 0;
					image_dim.y = (i == start) ? offset : 0;
					image_dim.width = page_width - image_dim.x;
					image_dim.height = page_height - image_dim.y;

					surface_dim.x = (page_width > alloc_width) ?
						0 : ((alloc_width - page_width)/2);

				}

				surface_dim.y = pos_y;
				surface_dim.width = alloc_width;
				surface_dim.height = alloc_height;

			} else {

				image_dim.x = 0;
				image_dim.y = (i == start) ? offset : 0;
				image_dim.width = page_width;
				image_dim.height = page_height - image_dim.y;

				surface_dim.x = (page_width < alloc_width) ?
					((alloc_width - page_width)/2) - scroll_x :
					0;

				surface_dim.y = pos_y;
				surface_dim.width = alloc_width;
				surface_dim.height = alloc_height;

			}

			GspdfPageSelection *selection = NULL;
			GList *iter = page_data->selection;

			while (iter) {

				if (((GspdfPageSelection*)iter->data)->index == i) {
					selection = (GspdfPageSelection*) iter->data;
					break;
				}

				iter = iter->next;
			}

			if (image_dim.height > 0) {
				draw_single_page (
					page_data,
					widget,
					cr,
					i,
					&image_dim,
					&surface_dim,
					(page_data->find_index == i) ?
						((GspdfRectangle*)(page_data->find_rect_iter) ?
							page_data->find_rect_iter->data : NULL) :
						NULL,
					(selection != NULL) ? selection->selection : NULL
				);
			}

			pos_y += (page_height + page_data->spacing);
		    pos_y -= ((i == start) ? offset : 0);

		}

	// single page mode

	} else {

		const GspdfDocMap *doc_map = (GspdfDocMap*) g_ptr_array_index
			(page_data->doc_map, page_data->index);

		const gdouble page_width = ((doc_map->width - floor (doc_map->width)) >= 0.5 ?
			ceil (doc_map->width) : floor (doc_map->width)) * page_data->scale;

		const gdouble page_height = ((doc_map->height - floor (doc_map->height)) >= 0.5 ?
				ceil (doc_map->height) : floor (doc_map->height)) * page_data->scale;

		const GspdfRectangle image_dim = {
			(page_width < alloc_width) ? 0 : scroll_x,
			scroll_y,
			page_width,
			page_height - scroll_y
		};

		const GspdfRectangle surface_dim = {
			(page_width < alloc_width) ? (alloc_width - page_width)/2 : 0,
			(page_height < alloc_height) ? (alloc_height - page_height)/2 : 0,
			alloc_width ,
			alloc_height
		};

		GspdfPageSelection *selection = NULL;
		GList *iter = page_data->selection;

		while (iter) {

			if (((GspdfPageSelection*)iter->data)->index == page_data->index) {
				selection = (GspdfPageSelection*) iter->data;
				break;
			}

			iter = iter->next;
		}

		draw_single_page (
			page_data,
			widget,
			cr,
			page_data->index,
			&image_dim,
			&surface_dim,
			(page_data->find_index == page_data->index) ?
				((GspdfRectangle*)(page_data->find_rect_iter) ?
					page_data->find_rect_iter->data : NULL) :
				NULL,
			(selection != NULL) ? selection->selection : NULL
		);

		update_hscroll_range (page_data, 0, page_width);
		update_vscroll_range (page_data, 0, page_height);
	}
}

static void
update_page (GspdfPageData *page_data)
{
	if (!page_data->document) {
		return;
	}

	const gdouble width = get_page_allocated_width (page_data);
	const gdouble height = get_page_allocated_height (page_data);

	update_hscroll_page_size (page_data, width);
	update_vscroll_page_size (page_data, height);
	gspdf_page_cache_clear (page_data->page_cache);

	switch (page_data->scale_mode) {
		case SCALE_NORMAL:
			page_data->scale = DEFAULT_SCALE_VALUE;
			break;
		case SCALE_FITP:
			page_data->scale = height / get_page_maximum_height (page_data);
			break;
		case SCALE_FITW:
			page_data->scale = width / get_page_maximum_width (page_data);
			break;
		default:
			break;
	}

	if (page_data->continuous) {
		update_hscroll_range (
			page_data,
			0,
			get_page_maximum_width (page_data) * page_data->scale
		);

		update_vscroll_range (
			page_data,
			0,
			get_page_total_height (page_data)
		);

		update_hscroll_value_block (
			page_data,
			get_hscroll_upper (page_data) * page_data->hadj_val_prcnt
		);

		update_vscroll_value_block (
			page_data,
			get_vscroll_upper (page_data) * page_data->vadj_val_prcnt
		);

		const GspdfRectangle area = {
			0,
			get_vscroll_value (page_data),
			width,
			height
		};

		update_page_range_area (page_data, &area);
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		gspdf_page_cache_set_range (
			page_data->page_cache,
			page_data->index,
			page_data->index,
			page_data->scale
		);
	}

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
scroll_page (GspdfPageData *page_data)
{
	if (!page_data->document) {
		return;
	}

	const gdouble x_value = get_hscroll_value (page_data);
	const gdouble y_value = get_vscroll_value (page_data);
	const gdouble x_upper = get_hscroll_upper (page_data);
	const gdouble y_upper = get_vscroll_upper (page_data);

	page_data->hadj_val_prcnt = x_value / x_upper;
	page_data->vadj_val_prcnt = y_value / y_upper;

	if (page_data->continuous) {
		const GspdfRectangle area = {
			0,
			(y_value <= 0) ? 1 : y_value,
			get_page_allocated_width (page_data),
			get_page_allocated_height (page_data)
		};

		update_page_range_area (page_data, &area);
	}

	update_index_toolbar (GSPDF_APP (page_data->window));
	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
update_hscroll_page_size (GspdfPageData *page_data,
                          gdouble        page)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_HADJ]);
	gtk_adjustment_set_page_size (adv, page);
	gtk_adjustment_set_page_increment (adv, 10);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_HADJ]);
}

static void
update_vscroll_page_size (GspdfPageData *page_data,
                          gdouble        page)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_VADJ]);
	gtk_adjustment_set_page_size (adv, page);
	gtk_adjustment_set_page_increment (adv, 10);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_VADJ]);
}

static void
update_hscroll_range (GspdfPageData *page_data,
                      gdouble        start,
                      gdouble        end)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_HADJ]);
	gtk_adjustment_set_lower (adv, start);
	gtk_adjustment_set_upper (adv, end);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_HADJ]);
}

static void
update_vscroll_range (GspdfPageData *page_data,
                      gdouble        start,
                      gdouble        end)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_VADJ]);
	gtk_adjustment_set_lower (adv, start);
	gtk_adjustment_set_upper (adv, end);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_VADJ]);
}

static void
update_hscroll_value (GspdfPageData *page_data,
                      gdouble        val)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	gtk_adjustment_set_value (adv, val);
}

static void
update_vscroll_value (GspdfPageData *page_data,
                      gdouble        val)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	gtk_adjustment_set_value (adv, val);
}

static void
update_hscroll_value_block (GspdfPageData *page_data,
                            gdouble        val)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_HADJ]);
	gtk_adjustment_set_value (adv, val);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_HADJ]);
}

static void
update_vscroll_value_block (GspdfPageData *page_data,
                            gdouble        val)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	g_signal_handler_block (adv, page_data->signals[SIGNAL_VADJ]);
	gtk_adjustment_set_value (adv, val);
	g_signal_handler_unblock (adv, page_data->signals[SIGNAL_VADJ]);
}

static gdouble
get_hscroll_value (GspdfPageData *page_data)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	return gtk_adjustment_get_value (adv);
}

static gdouble
get_vscroll_value (GspdfPageData *page_data)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	return gtk_adjustment_get_value (adv);
}

static gdouble
get_hscroll_upper (GspdfPageData *page_data)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "h-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	return gtk_adjustment_get_upper (adv);
}

static gdouble
get_vscroll_upper (GspdfPageData *page_data)
{
	GtkWidget *scroll = NULL;
	g_object_get (G_OBJECT (page_data->page), "v-scroll", &scroll, NULL);
	g_object_unref (scroll);

	GtkAdjustment *adv = gtk_range_get_adjustment (GTK_RANGE (scroll));

	return gtk_adjustment_get_upper (adv);
}

static void
next_page (GspdfPageData *page_data)
{
	g_return_if_fail ((page_data->index + 1) < page_data->doc_map->len);

	page_data->index = page_data->index + 1;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value (page_data, get_page_y_offset (page_data, page_data->index));
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
prev_page (GspdfPageData *page_data)
{
	g_return_if_fail ((page_data->index - 1) >= 0);

	page_data->index = page_data->index - 1;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value (page_data, get_page_y_offset (page_data, page_data->index));
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
first_page (GspdfPageData *page_data)
{
	page_data->index = 0;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value (page_data, get_page_y_offset (page_data, page_data->index));
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
last_page (GspdfPageData *page_data)
{
	page_data->index = page_data->doc_map->len - 1;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value (page_data, get_page_y_offset (page_data, page_data->index));
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
goto_page (GspdfPageData *page_data, gint index)
{
	page_data->index = index;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value (
			page_data,
			get_page_y_offset (page_data, page_data->index) + 1
		);
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
goto_page_at_pos (GspdfPageData *page_data, gint index, gdouble x, gdouble y)
{
	page_data->index = index;

	update_index_toolbar (GSPDF_APP (page_data->window));

	if (page_data->continuous) {
		update_vscroll_value_block (
			page_data,
			get_page_y_offset (page_data, page_data->index) + y + 1
		);
		scroll_page (page_data);
	} else {
		update_hscroll_value_block (page_data, 0);
		update_vscroll_value_block (page_data, 0);
		update_page_range (page_data, page_data->index, page_data->index);
	}
}

static void
_process_goto_dest_action (GspdfPageData *page_data, GspdfDocActionGotoDest *action)
{
	gint index = -1;

	if (action->dest->named_dest) {

		GspdfDocDest *dest = gspdf_document_find_dest
			(page_data->document, action->dest->named_dest);

		if (dest) {

			index = dest->index;
			gspdf_doc_dest_free (dest);

		}

	} else {

		index = action->dest->index;

	}

	if (index > 0) {

		goto_page (page_data, index -1);

	}
}

static void
_process_named_action (GspdfPageData *page_data, GspdfDocActionNamed *action)
{
	gint index = -1;

	GspdfDocDest *dest = gspdf_document_find_dest (page_data->document, action->named_dest);

	if (dest) {

		index = dest->index;
		gspdf_doc_dest_free (dest);

	}

	if (index > 0) {

		goto_page (page_data, index - 1);

	}
}

static void
_process_uri_action (GspdfPageData *page_data, GspdfDocActionUri *action)
{
	if (action->uri) {
		gchar *cmd = g_strdup_printf ("xdg-open %s", action->uri);
		system (cmd);
		g_free (cmd);
	}
}

static void
process_action (GspdfPageData *page_data, GspdfDocAction *action)
{
	GspdfDocActionAny *act = (GspdfDocActionAny*) action;

	switch (act->type) {
		case GSPDF_DOC_ACTION_GOTO_DEST:
			_process_goto_dest_action (page_data, (GspdfDocActionGotoDest*) action);
			return;
		case GSPDF_DOC_ACTION_NAMED:
			_process_named_action (page_data, (GspdfDocActionNamed*) action);
			return;
		case GSPDF_DOC_ACTION_URI:
			_process_uri_action (page_data, (GspdfDocActionUri*) action);
			return;
		default:
			return;
	}
}

static void
find_text (GspdfPageData *page_data, const gchar *text, GspdfFindFlags options)
{
	GspdfDocumentPage *doc_page = NULL;
	GList *res = NULL;
	gint index = page_data->index;

	if (g_strcmp0 (page_data->find_text, text) == 0) {
		page_data->find_rect_iter = page_data->find_rect_iter->next;

		if ((page_data->find_rect_iter) && (page_data->index == page_data->find_index)) {

			if (!page_data->continuous) {
				gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
			}

			goto_page_at_pos (
				page_data,
				page_data->index,
				((GspdfRectangle*)page_data->find_rect_iter->data)->x,
				((GspdfRectangle*)page_data->find_rect_iter->data)->y * page_data->scale
			);

			return;
		}

		index += 1;
	}

	if (page_data->find_text) {
		g_free (page_data->find_text);
		page_data->find_text = NULL;
	}

	if (index >= gspdf_document_get_n_pages (page_data->document)) {
		index = 0;
	}

	if (page_data->find_rect) {
		g_list_free_full (page_data->find_rect, on_g_list_rect_free_func);
		page_data->find_rect = NULL;
	}

	for (gint i = index; i < gspdf_document_get_n_pages (page_data->document); i++) {
		doc_page = gspdf_document_get_page (page_data->document, i);
		res = gspdf_document_page_find_text (doc_page, text, options);

		g_object_unref (doc_page);

		if (res) {
			page_data->find_text = g_strdup (text);
			page_data->find_index = i;
			page_data->find_rect = res;
			page_data->find_rect_iter = res;
			goto_page_at_pos (
				page_data,
				page_data->find_index,
				((GspdfRectangle*)page_data->find_rect_iter->data)->x,
				((GspdfRectangle*)page_data->find_rect_iter->data)->y * page_data->scale
			);
			return;
		}
	}
}

static void
clear_find (GspdfPageData *page_data)
{
	if (page_data->find_text) {
		g_free (page_data->find_text);
		page_data->find_text = NULL;
	}

	if (page_data->find_rect) {
		g_list_free_full (
			page_data->find_rect,
			on_g_list_rect_free_func
		);
		page_data->find_rect = NULL;
	}

	page_data->find_rect_iter = NULL;

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
show_page_popup (GspdfPageData  *page_data,
				 GdkEventButton *event)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (
		GSPDF_APP (page_data->window)
	);

	gtk_widget_set_sensitive (
		priv->page_popup->copy,
		(page_data->selection != NULL) ? TRUE : FALSE
	);

	gtk_menu_popup(
		GTK_MENU (priv->page_popup->menu),
		NULL, NULL, NULL, NULL, event->button, event->time
	);
}

static void
show_bookmark_popup (GspdfPageData  *page_data,
				     GdkEventButton *event)
{
	GspdfAppPrivate *priv = gspdf_app_get_instance_private (
		GSPDF_APP (page_data->window)
	);

	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (page_data->window), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);

	GtkWidget *bookmark = NULL;
	g_object_get (G_OBJECT (sidebar), "bookmark", &bookmark, NULL);
	g_object_unref (bookmark);

	if (!gtk_tree_view_get_path_at_pos (
		GTK_TREE_VIEW (bookmark), event->x, event->y, NULL, NULL, NULL, NULL))
	{
		return;
	}

	gtk_menu_popup (
		GTK_MENU (priv->bookmark_popup->menu),
		NULL, NULL, NULL, NULL, event->button, event->time
	);
}

static void
_show_error_dialog (GspdfPageData  *page_data,
					const gchar    *text)
{
	GtkWidget *dialog = gtk_message_dialog_new (
		GTK_WINDOW (page_data->window),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		text,
		NULL
	);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void
_show_password_dialog (GspdfPageData *page_data)
{
	GtkWidget *dialog = gtk_dialog_new ();
	GtkWidget *content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	GtkWidget *grid = gtk_grid_new ();
	GtkWidget *pass_label = gtk_label_new ("Password:");
	GtkWidget *pass_entry = gtk_entry_new ();

	gtk_entry_set_visibility  (GTK_ENTRY (pass_entry), FALSE);
	gtk_entry_set_invisible_char (GTK_ENTRY (pass_entry), '*');
	gtk_grid_set_row_spacing (GTK_GRID (grid), 25);
	gtk_grid_set_column_spacing (GTK_GRID (grid), 50);
	gtk_grid_attach (GTK_GRID (grid), pass_label, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), pass_entry, 1, 0, 1, 1);
	gtk_container_add (GTK_CONTAINER (content), grid);
	gtk_dialog_add_button (GTK_DIALOG (dialog), "Ok", 1);
	gtk_dialog_add_button (GTK_DIALOG (dialog), "Cancel", 0);
	gtk_window_set_title (GTK_WINDOW (dialog), "Enter password");
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (page_data->window));
	gtk_widget_show_all (dialog);

	gint res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == 1) {
		const gchar *pass = gtk_entry_get_text (GTK_ENTRY (pass_entry));
		gchar *uri = gspdf_page_cache_get_uri (page_data->page_cache);

		if (uri) {
			open_document (page_data, uri, pass);
			g_free (uri);
		}
	}

	gtk_widget_destroy (dialog);
}

static void
handle_document_error (GspdfPageData  *page_data,
					   GError         *error)
{
	switch (error->code) {
		case GSPDF_DOCUMENT_ERROR_ENCRYPTED:
			_show_password_dialog (page_data);
			break;
		default:
			_show_error_dialog (page_data, error->message);
			break;
	}
}

/* signal definition */



static void
on_destroy (GtkWidget *widget,
            gpointer   user_data)
{
	for (gint i = 0; i < get_n_pages (GSPDF_APP (widget)); i++) {
		close_document (get_current_page_data (GSPDF_APP (widget)));
	}
}

static void
on_notebook_page_added (GtkNotebook *notebook,
					    GtkWidget   *child,
					    guint        page_num,
					    gpointer     user_data)
{
	GspdfPageData *page_data = g_malloc0 (sizeof (GspdfPageData));

	page_data->window = (GtkWidget*) user_data;
	page_data->page = (GtkWidget*) child;
	page_data->page_cache = gspdf_page_cache_new ();
	page_data->continuous = DEFAULT_CONTINUOUS_VALUE;
	page_data->scale = DEFAULT_SCALE_VALUE;
	page_data->spacing = DEFAULT_SPACING_VALUE;
	page_data->scale_mode = DEFAULT_SCALE_MODE_VALUE;
	page_data->outline = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	page_data->bookmark = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_INT);

	g_object_set (G_OBJECT (child), "user-data", page_data, NULL);

	// page cache
	g_signal_connect (
		G_OBJECT (page_data->page_cache),
		"document-load-finished",
		G_CALLBACK (on_page_cache_document_load_finished),
		page_data
	);

	g_signal_connect (
		G_OBJECT (page_data->page_cache),
		"document-render-finished",
		G_CALLBACK (on_page_cache_document_render_finished),
		page_data
	);

	// drawing area
	GtkWidget *drawing_area = NULL;
	g_object_get (G_OBJECT (child), "drawing-area", &drawing_area, NULL);
	g_object_unref (drawing_area);

	gtk_widget_add_events (
		drawing_area,
		gtk_widget_get_events (drawing_area) |
		GDK_LEAVE_NOTIFY_MASK |
		GDK_POINTER_MOTION_MASK |
		GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK |
		GDK_SCROLL_MASK
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"draw",
		G_CALLBACK (on_page_drawing_area_draw),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"size-allocate",
		G_CALLBACK (on_page_drawing_area_size_allocate),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"leave-notify-event",
		G_CALLBACK (on_page_drawing_area_leave),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"motion-notify-event",
		G_CALLBACK (on_page_drawing_area_motion),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"button-press-event",
		G_CALLBACK (on_page_drawing_button_press),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"button-release-event",
		G_CALLBACK (on_page_drawing_button_release),
		page_data
	);

	g_signal_connect (
		G_OBJECT (drawing_area),
		"scroll-event",
		G_CALLBACK (on_page_drawing_scroll),
		page_data
	);

	// h-scroll & v-scroll
	GtkWidget *hscroll = NULL;
	g_object_get (G_OBJECT (child), "h-scroll", &hscroll, NULL);
	g_object_unref (hscroll);

	GtkWidget *vscroll = NULL;
	g_object_get (G_OBJECT (child), "v-scroll", &vscroll, NULL);
	g_object_unref (vscroll);

	page_data->signals[SIGNAL_HADJ] = g_signal_connect (
		G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (hscroll))),
		"value-changed",
		G_CALLBACK (on_hadj_value_changed),
		page_data
	);

	page_data->signals[SIGNAL_VADJ] = g_signal_connect (
		G_OBJECT (gtk_range_get_adjustment (GTK_RANGE (vscroll))),
		"value-changed",
		G_CALLBACK (on_vadj_value_changed),
		page_data
	);
}

static void
on_notebook_page_switch (GtkNotebook *notebook,
						 GtkWidget   *child,
						 guint        page_num,
						 gpointer     user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;
	GtkWidget *page = (GtkWidget*) child;

	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (window), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);

	GtkWidget *outline = NULL;
	g_object_get (G_OBJECT (sidebar), "outline", &outline, NULL);
	g_object_unref (outline);

	GtkWidget *bookmark = NULL;
	g_object_get (G_OBJECT (sidebar), "bookmark", &bookmark, NULL);
	g_object_unref (bookmark);

	GspdfPageData *page_data = NULL;
	g_object_get (G_OBJECT (page), "user-data", &page_data, NULL);

	reset_menu (GSPDF_APP (window));
	reset_toolbar (GSPDF_APP (window));

	gtk_tree_view_set_model (GTK_TREE_VIEW (outline), GTK_TREE_MODEL (page_data->outline));
	gtk_tree_view_set_model (GTK_TREE_VIEW (bookmark), GTK_TREE_MODEL (page_data->bookmark));
}

static void
on_open_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;

	GtkWidget *dialog = gtk_file_chooser_dialog_new (
		"Open File",
		GTK_WINDOW (window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		 "_Cancel", GTK_RESPONSE_CANCEL,
		 "_Open", GTK_RESPONSE_ACCEPT,
		 NULL
	);

	gint res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT) {

		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		gchar *uri = gtk_file_chooser_get_uri(chooser);

		open_document (get_current_page_data (GSPDF_APP (window)), uri, NULL);

		g_free (uri);
	}

	gtk_widget_destroy (dialog);
}

static void
on_save_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	g_print ("activate\n");
}

static void
on_prop_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	g_print ("activate\n");
}

static void
on_close_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	g_return_if_fail (page_data->doc_map != NULL);

	close_document (page_data);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_exit_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_widget_destroy ((GtkWidget*)user_data);
}

static void
on_next_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	next_page (page_data);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_prev_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	prev_page (page_data);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_first_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	first_page (page_data);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_last_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	last_page (page_data);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_cont_menu_item_toggled (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->continuous = gtk_check_menu_item_get_active (checkmenuitem);

	if (!page_data->document) {
		return;
	}

	if (page_data->continuous) {

		GtkWidget *drawing_area = NULL;
		g_object_get (G_OBJECT (page_data->page), "drawing_area", &drawing_area, NULL);
		g_object_unref (drawing_area);

		update_hscroll_range (page_data, 0, get_page_maximum_width (page_data) * page_data->scale);
		update_vscroll_range (page_data, 0, get_page_total_height (page_data));
		update_vscroll_value (page_data, get_page_y_offset (page_data, page_data->index));

	} else {

		update_vscroll_value_block (page_data, 0);

		update_page_range (page_data, page_data->index, page_data->index);

	}

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static void
on_zoomin_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_CUSTOM;

	if (!page_data->document) {
		return;
	}

	page_data->scale = page_data->scale + SCALE_STEP;

	update_page (page_data);
}

static void
on_zoomout_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_CUSTOM;

	if (!page_data->document) {
		return;
	}

	const gdouble scale = page_data->scale - SCALE_STEP;

	g_return_if_fail (scale > 0);

	page_data->scale = scale;

	update_page (page_data);
}

static void
on_zoomorg_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_NORMAL;

	if (!page_data->document) {
		return;
	}

	update_page (page_data);

}

static void
on_zoomfitp_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_FITP;

	if (!page_data->document) {
		return;
	}

	update_page (page_data);
}

static void
on_zoomfitw_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_FITW;

	if (!page_data->document) {
		return;
	}

	update_page (page_data);
}

static void
on_markthis_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	gchar *text = g_strdup_printf ("Page %d", page_data->index);
	GtkTreeIter iter;

	gtk_tree_store_append (page_data->bookmark, &iter, NULL);
	gtk_tree_store_set (
		page_data->bookmark, &iter,
		0, text, 1, page_data->index, -1
	);

	update_config_bookmark (page_data);

	g_free (text);
}

static void
on_index_entry_tool_item_activate (GtkEntry *entry, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	gint index = g_ascii_strtoll (gtk_entry_get_text (entry), NULL, 10) - 1;

	g_return_if_fail ((index >= 0) && (index < page_data->doc_map->len));

	goto_page (page_data, index);
}

static void
on_open_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;

	GtkWidget *dialog = gtk_file_chooser_dialog_new (
		"Open File",
		GTK_WINDOW (window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		 "_Cancel", GTK_RESPONSE_CANCEL,
		 "_Open", GTK_RESPONSE_ACCEPT,
		 NULL
	);

	gint res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT) {

		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		gchar *uri = gtk_file_chooser_get_uri(chooser);

		open_document (get_current_page_data (GSPDF_APP (window)), uri, NULL);

		g_free (uri);
	}

	gtk_widget_destroy (dialog);
}

static void
on_prev_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	prev_page (page_data);
}

static void
on_next_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	next_page (page_data);
}

static void
on_zoomout_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_CUSTOM;

	if (!page_data->document) {
		return;
	}

	const gdouble scale = page_data->scale - SCALE_STEP;

	g_return_if_fail (scale > 0);

	page_data->scale = scale;

	update_page (page_data);
}

static void
on_zoomin_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_CUSTOM;

	if (!page_data->document) {
		return;
	}

	page_data->scale = page_data->scale + SCALE_STEP;

	update_page (page_data);
}

static void
on_zoomfit_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_FITP;

	if (!page_data->document) {
		return;
	}

	update_page (page_data);
}

static void
on_zoomorg_tool_button_clicked (GtkToolButton *toolbutton, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	page_data->scale_mode = SCALE_NORMAL;

	if (!page_data->document) {
		return;
	}

	update_page (page_data);
}

static void
on_find_entry_tool_item_activate (GtkEntry *entry, gpointer user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	const gchar *text = gtk_entry_get_text (entry);

	if (!g_strcmp0 (text, "")) {
		return;
	}

	find_text (page_data, text, GSPDF_FIND_DEFAULT);
}

static void
on_find_tool_button_clicked (GtkToolButton *toolbutton,
                             gpointer       user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (!page_data->document) {
		return;
	}

	GtkWidget *toolbar = NULL;
	g_object_get (GSPDF_APP (user_data), "toolbar", &toolbar, NULL);
	g_object_unref (toolbar);

	GtkWidget *find = NULL;
	g_object_get (G_OBJECT (toolbar), "find-entry-tool-item", &find, NULL);
	g_object_unref (G_OBJECT (find));

	const gchar *text = gtk_entry_get_text (
		GTK_ENTRY (gtk_bin_get_child (GTK_BIN (find)))
	);

	if (!g_strcmp0 (text, "")) {
		return;
	}

	find_text (page_data, text, GSPDF_FIND_DEFAULT);
}

static gboolean
on_page_drawing_area_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	cairo_set_source_rgba (cr, 0, 0, 0, 0.4);

	cairo_rectangle (
		cr, 0, 0,
		gtk_widget_get_allocated_width (widget),
		gtk_widget_get_allocated_height (widget)
	);

	cairo_fill (cr);

	if (!page_data->document) {
		return FALSE;
	}

	draw_page (page_data, widget, cr);

	return TRUE;
}

static gboolean
on_page_drawing_area_size_allocate (GtkWidget    *widget,
	                                  GdkRectangle *allocation,
																		gpointer      user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	update_hscroll_page_size (page_data, allocation->width);
	update_vscroll_page_size (page_data, allocation->height);

	if (!page_data->document) {
		return FALSE;
	}

	update_page (page_data);

	return TRUE;
}

static void
on_hadj_value_changed (GtkAdjustment *adjustment, gpointer user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	if (page_data->document && page_data->doc_map) {
		scroll_page (page_data);
	}
}

static void
on_vadj_value_changed (GtkAdjustment *adjustment, gpointer user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	if (page_data->document && page_data->doc_map) {
		scroll_page (page_data);
	}
}

static void
on_page_cache_document_load_finished (GObject *object, gpointer user_data)
{
	g_return_if_fail (GSPDF_IS_PAGE_CACHE (object));
	g_return_if_fail (user_data != NULL);

	GspdfPageCache *page_cache = GSPDF_PAGE_CACHE (object);
	GspdfPageData *page_data = (GspdfPageData*) user_data;
	GError *err = NULL;
	GspdfDocument *doc = gspdf_page_cache_get_document (page_cache, &err);

	if (doc != NULL) {
		gchar *title = NULL;
		g_object_get (doc, "title", &title, NULL);

		if (title != NULL) {
			gtk_window_set_title (GTK_WINDOW (page_data->window), title);
			g_free (title);
		}

		page_data->document = doc;
		page_data->doc_map = gspdf_page_cache_get_document_map (page_cache);

		fill_outline (page_data, doc);
		fill_bookmark (page_data);
		restore_from_config_cache (page_data);
		reset_menu (GSPDF_APP (page_data->window));
		reset_toolbar (GSPDF_APP (page_data->window));
		update_index_toolbar (GSPDF_APP (page_data->window));

		const gdouble width = get_page_allocated_width (page_data);
		const gdouble height = get_page_allocated_height (page_data);

		if (page_data->scale_mode == SCALE_NORMAL) {
			page_data->scale = DEFAULT_SCALE_VALUE;
		} else if (page_data->scale_mode == SCALE_FITP) {
			const gdouble max_height = get_page_maximum_height (page_data);
			page_data->scale = height / max_height;
		} else if (page_data->scale_mode == SCALE_FITW) {
			const gdouble max_width = get_page_maximum_width (page_data);
			page_data->scale = width / max_width;
		}

		if (page_data->continuous) {
			update_hscroll_range (
				page_data,
				0,
				get_page_maximum_width (page_data) * page_data->scale
			);

			update_vscroll_range (
				page_data,
				0,
				get_page_total_height (page_data)
			);

			update_hscroll_value_block (
				page_data,
				get_hscroll_upper (page_data) * page_data->hadj_val_prcnt
			);

			update_vscroll_value_block (
				page_data,
				(get_vscroll_upper (page_data) * page_data->vadj_val_prcnt) + 1
			);

			const gdouble y_value = get_vscroll_value (page_data);
			const gdouble y_upper = get_vscroll_upper (page_data);
			const GspdfRectangle area = {
				0,
				(y_value <= 0) ? 1 : y_value,
				get_page_allocated_width (page_data),
				get_page_allocated_height (page_data)
			};

			update_page_range_area (page_data, &area);
		} else {
			update_page_range (page_data, page_data->index, page_data->index);
		}

		gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
	} else {
		if (err) {
			handle_document_error (page_data, err);
			g_error_free (err);
		}
	}
}

static void
on_page_cache_document_render_finished (GObject *object, gpointer user_data)
{
	g_return_if_fail (GSPDF_IS_PAGE_CACHE (object));
	g_return_if_fail (user_data != NULL);

	GspdfPageData *page_data = (GspdfPageData*) user_data;

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
}

static gboolean
on_outline_treeview_button_press (GtkWidget       *widget,
                                  GdkEventButton  *event,
                                  gpointer         user_data)
{
	GtkTreePath *path = NULL;
	if (!gtk_tree_view_get_path_at_pos (
		GTK_TREE_VIEW (widget), event->x, event->y, &path, NULL, NULL, NULL)) {
		return FALSE;
	}

	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));
	if (!model) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	GtkTreeIter iter;
	if (!gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	GspdfDocAction *action = NULL;
	gtk_tree_model_get (model, &iter, 1, &action, -1);
	if (!action) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));
	if (!page_data->document) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	process_action (page_data, action);

	page_data->outline_iter = iter;

	gtk_tree_path_free (path);

	return FALSE;
}

static gboolean
on_outline_treeview_query_tooltip (GtkWidget  *widget,
                                   gint        x,
						           gint        y,
                                   gboolean    keyboard_mode,
                                   GtkTooltip *tooltip,
                                   gpointer    user_data)
{
	GtkTreePath *path = NULL;
	if (!gtk_tree_view_get_path_at_pos (
		GTK_TREE_VIEW (widget), x, y, &path, NULL, NULL, NULL)) {
		return FALSE;
	}

	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));
	if (!model) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	GtkTreeIter iter;
	if (!gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	gchar *text = NULL;
	gtk_tree_model_get (model, &iter, 0, &text, -1);
	if (!text) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	gtk_tooltip_set_text (tooltip, text);
	gtk_tree_view_set_tooltip_row (GTK_TREE_VIEW (widget), tooltip, path);

	g_free (text);
	gtk_tree_path_free (path);

	return TRUE;
}

static gboolean
on_bookmark_treeview_button_press (GtkWidget       *widget,
                                   GdkEventButton  *event,
                                   gpointer         user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (user_data));

	if (event->button == 3) {
		show_bookmark_popup (page_data, event);
		return FALSE;
	}

	GtkTreePath *path = NULL;
	if (!gtk_tree_view_get_path_at_pos (
		GTK_TREE_VIEW (widget), event->x, event->y, &path, NULL, NULL, NULL)) {
		return FALSE;
	}

	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));
	if (!model) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	GtkTreeIter iter;
	if (!gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_path_free (path);
		return FALSE;
	}

	gint index = 0;
	gtk_tree_model_get (model, &iter, 1, &index, -1);
	goto_page (page_data, index);

	return FALSE;
}

static void
_list_selection_free_func (gpointer data)
{
	g_free ((GspdfRectangle*)data);
}

static void
_list_page_selection_free_func (gpointer data)
{
	GspdfPageSelection *sel = (GspdfPageSelection*) data;

	if (sel->selection) {

		g_list_free_full (sel->selection, _list_selection_free_func);

	}

	g_free (sel);
}

static void
_list_text_mapping_free_func (gpointer data)
{
	GspdfDocLinkMapping *link_mapping = (GspdfDocLinkMapping*) data;

	if (link_mapping->action) {

		gspdf_doc_action_free (link_mapping->action);

	}

	g_free (link_mapping);
}

static void
_update_cursor (GspdfPageData  *page_data,
				GdkEventMotion *event)
{
	gint index = -1;
	gdouble x = -1, y  = -1;

	if (!get_current_pointer_position (page_data, event->x, event->y, &index, &x, &y)) {

		return;

	}

	if ((index < 0) || (x < 0) || (y < 0)) {

		return;

	}

	// link
	GspdfDocumentPage *doc_page = gspdf_document_get_page (page_data->document, index);
	GList *links = gspdf_document_page_get_link_mapping (doc_page);

	g_object_unref (doc_page);

	if (links) {

		GspdfDocLinkMapping *link_mapping = NULL;
		GList *links_iter = links;

		while (links_iter) {

			link_mapping = (GspdfDocLinkMapping*) links_iter->data;

			if ((x > (link_mapping->area.x * page_data->scale)) &&
				(y > (link_mapping->area.y  * page_data->scale)) &&
				(x < ((link_mapping->area.x  * page_data->scale) +
					 (link_mapping->area.width  * page_data->scale))) &&
				(y < ((link_mapping->area.y  * page_data->scale) +
				      (link_mapping->area.height  * page_data->scale))))
			{

				GdkWindow *window = gtk_widget_get_window (page_data->window);

				GdkCursor *cursor = gdk_cursor_new_for_display (
					gdk_window_get_display (window), GDK_HAND2
				);

				gdk_window_set_cursor (window, cursor);

				page_data->pointer_mode = POINTER_MODE_LINK;

				if (page_data->link_action) {

					gspdf_doc_action_free (page_data->link_action);

					page_data->link_action = NULL;

				}

				page_data->link_action = gspdf_doc_action_copy (link_mapping->action);

				g_object_unref (cursor);

				g_list_free_full (links, _list_text_mapping_free_func);

				return;

			}

			links_iter = links_iter->next;

		}

	}

	// text

	GList *text_mapping = gspdf_page_cache_get_text_mapping (page_data->page_cache, index);

	if (text_mapping) {

		GList *text_mapping_iter = text_mapping;

		while (text_mapping_iter) {

			if ((x > (((GspdfRectangle*)text_mapping_iter->data)->x * page_data->scale)) &&
				(y > (((GspdfRectangle*)text_mapping_iter->data)->y * page_data->scale)) &&
				(x < ((((GspdfRectangle*)text_mapping_iter->data)->x * page_data->scale) +
					 (((GspdfRectangle*)text_mapping_iter->data)->width * page_data->scale))) &&
				(y < ((((GspdfRectangle*)text_mapping_iter->data)->y * page_data->scale) +
				      (((GspdfRectangle*)text_mapping_iter->data)->height * page_data->scale))))
			{

				GdkWindow *window = gtk_widget_get_window (page_data->window);

				GdkCursor *cursor = gdk_cursor_new_for_display (
					gdk_window_get_display (window), GDK_XTERM
				);

				gdk_window_set_cursor (window, cursor);

				page_data->pointer_mode = POINTER_MODE_TEXT;

				g_object_unref (cursor);

				//g_list_free_full (text_mapping, _list_selection_free_func);

				return;

			}

			text_mapping_iter = text_mapping_iter->next;

		}

	}

	gdk_window_set_cursor (gtk_widget_get_window (page_data->window), NULL);

	page_data->pointer_mode = POINTER_MODE_NORMAL;

}

static gboolean
on_page_drawing_area_leave (GtkWidget        *widget,
                            GdkEventCrossing *event,
                            gpointer          user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	gdk_window_set_cursor (gtk_widget_get_window (page_data->window), NULL);

	return FALSE;
}

static gboolean
on_page_drawing_area_motion (GtkWidget       *widget,
                             GdkEventMotion  *event,
                             gpointer         user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	if ((!page_data->document) || (!page_data->doc_map)) {
		return FALSE;
	}

	if (!page_data->selecting) {

		_update_cursor (page_data, event);

		//g_print ("%d [%f, %f]\n", index, x, y); */

		return FALSE;
	}

	if (page_data->selection) {
		g_list_free_full (page_data->selection, _list_page_selection_free_func);
	}

	page_data->selection = NULL;

	const GspdfRectangle area = {
		page_data->start_x,
		page_data->start_y,
		event->x - page_data->start_x,
		event->y - page_data->start_y
	};

	page_data->selection = get_selected_page_area (page_data, &area);

	gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));

	return TRUE;
}

static gboolean
on_page_drawing_button_press (GtkWidget       *widget,
                              GdkEventButton  *event,
                              gpointer         user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	gtk_widget_grab_focus (widget);
	clear_find (page_data);

	if (!page_data->document) {
		return FALSE;
	}

	if (event->button == 1) {
		// link
		if (page_data->pointer_mode == POINTER_MODE_LINK) {
			if (page_data->link_action) {
				process_action (page_data, page_data->link_action);
			}
			return TRUE;
		}

		// selection
		if (page_data->selection) {
			g_list_free_full (
				page_data->selection,
				_list_page_selection_free_func
			);
			gspdf_page_queue_draw (GSPDF_PAGE (page_data->page));
		}

		page_data->selection = NULL;
		page_data->start_x = event->x;
		page_data->start_y = event->y;
		page_data->selecting = TRUE;

		return TRUE;

	} else if (event->button == 3) {
		show_page_popup (page_data, event);

		return TRUE;
	}

	return FALSE;
}

static gboolean
on_page_drawing_button_release (GtkWidget       *widget,
                                GdkEventButton  *event,
                                gpointer         user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	if (!page_data->document) {
		return FALSE;
	}

	if (event->button == 1) {

		page_data->end_x = event->x;
		page_data->end_y = event->y;

		page_data->selecting = FALSE;

		return TRUE;

	}

	return FALSE;
}

static gboolean
on_page_drawing_scroll (GtkWidget       *widget,
                        GdkEventScroll  *event,
                        gpointer         user_data)
{
	GspdfPageData *page_data = (GspdfPageData*) user_data;

	if (!page_data->document) {
		return FALSE;
	}

	if (event->direction == GDK_SCROLL_UP) {

		update_vscroll_value (page_data, get_vscroll_value (page_data) - 50);

		return TRUE;

	} else if (event->direction == GDK_SCROLL_DOWN) {

		update_vscroll_value (page_data, get_vscroll_value (page_data) + 50);

		return TRUE;

	}

	return FALSE;
}

static gboolean
on_window_key_press (GtkWidget   *widget,
                     GdkEventKey *event,
                     gpointer     user_data)
{
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (widget));
	//gdouble val = 0;

	GtkWidget *toolbar = NULL;
	g_object_get (widget, "toolbar", &toolbar, NULL);
	g_object_unref (toolbar);

	GtkWidget *index_entry = NULL;
	g_object_get (toolbar, "index-entry-tool-item", &index_entry, NULL);
	g_object_unref (index_entry);

	GtkWidget *find_entry = NULL;
	g_object_get (toolbar, "find-entry-tool-item", &find_entry, NULL);
	g_object_unref (find_entry);

	if (gtk_widget_is_focus (gtk_bin_get_child (GTK_BIN (index_entry))) |
		gtk_widget_is_focus (gtk_bin_get_child (GTK_BIN (find_entry)))) {

		return FALSE;

	}

	switch (event->keyval) {

		case GDK_KEY_Up:

			if (!page_data->document) {
				break;
			}

			if (!gtk_window_activate_key (GTK_WINDOW (widget), event)) {

				update_vscroll_value (page_data, get_vscroll_value (page_data) - 50);

			}

			return TRUE;

		case GDK_KEY_Down:

			if (!page_data->document) {
				break;
			}

			if (!gtk_window_activate_key (GTK_WINDOW (widget), event)) {

				update_vscroll_value (page_data, get_vscroll_value (page_data) + 50);

			}

			return TRUE;

		case GDK_KEY_Left:

			if (!page_data->document) {
				break;
			}

			if (!gtk_window_activate_key (GTK_WINDOW (widget), event)) {

				update_hscroll_value (page_data, get_hscroll_value (page_data) - 50);

			}

			return TRUE;

		case GDK_KEY_Right:

			if (!page_data->document) {
				break;
			}

			if (!gtk_window_activate_key (GTK_WINDOW (widget), event)) {

				update_hscroll_value (page_data, get_hscroll_value (page_data) + 50);

			}

			return TRUE;

		case GDK_KEY_Home:

			if (!page_data->document) {
				break;
			}

			first_page (page_data);

			return TRUE;

		case GDK_KEY_End:

			if (!page_data->document) {
				break;
			}

			last_page (page_data);

			return TRUE;

		case GDK_KEY_space:

			if (!page_data->document) {
				break;
			}

			next_page (page_data);

			return TRUE;

		case GDK_KEY_BackSpace:

			if (!page_data->document) {
				break;
			}

			prev_page (page_data);

			return TRUE;

		default:
			break;
	};

	return GTK_WIDGET_CLASS (gspdf_app_parent_class)->key_press_event (widget, event);
}

static void
on_copy_popup_menu_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;
	GspdfPageData *page_data = get_current_page_data (GSPDF_APP (window));

	if (!page_data->document) {
		return;
	}

	const GspdfRectangle sel = {
		page_data->start_x,
		page_data->start_y,
		page_data->end_x - page_data->start_x,
		page_data->end_y - page_data->start_y
	};

	gchar *text = get_selected_text_area (page_data, &sel);

	if (text) {

		GtkClipboard *clipboard = gtk_clipboard_get_default (
			gdk_window_get_display (gtk_widget_get_window (window))
		);

		gtk_clipboard_set_text (clipboard, text, -1);

		g_free (text);

	}
}

static void
on_delete_popup_menu_item_activate (GtkMenuItem *menuitem,
                                    gpointer     user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;

	GtkWidget *sidebar = NULL;
	g_object_get (G_OBJECT (window), "sidebar", &sidebar, NULL);
	g_object_unref (sidebar);

	GtkWidget *bookmark = NULL;
	g_object_get (G_OBJECT (sidebar), "bookmark", &bookmark, NULL);
	g_object_unref (bookmark);

	GtkTreeSelection *sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (bookmark));
	GtkTreeModel *model = NULL;
	GtkTreeIter iter;

	if (!gtk_tree_selection_get_selected (sel, &model, &iter)) {
		return;
	}

	gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
	update_config_bookmark (get_current_page_data (GSPDF_APP (window)));
}

static void
on_about_menu_item_activate (GtkMenuItem *menuitem,
														 gpointer     user_data)
{
	GtkWidget *window = (GtkWidget*) user_data;
	GtkWidget *dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_logo_icon_name (
		GTK_ABOUT_DIALOG (dialog),
		"help-about"
	);
	gtk_about_dialog_set_program_name (
		GTK_ABOUT_DIALOG (dialog),
		"GSPDF"
	);
	gtk_about_dialog_set_version (
		GTK_ABOUT_DIALOG (dialog),
		"0.1"
	);
	gtk_about_dialog_set_comments (
		GTK_ABOUT_DIALOG (dialog),
		"GTK-based Simple PDF Reader"
	);
	gtk_about_dialog_set_copyright (
		GTK_ABOUT_DIALOG (dialog),
		"Copyright © 2017-2019 Fajar Dwi Darmanto"
	);
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

/* object's public methods */
GtkWidget *
gspdf_app_new (void)
{
	return GTK_WIDGET (g_object_new (GSPDF_TYPE_APP, NULL));
}

void
gspdf_app_open (GspdfApp *app, const gchar *uri)
{
	open_document (get_current_page_data (app), uri, NULL);
}
