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

#include "gspdf-page-cache.h"

typedef struct {
	gchar              *uri;
	gchar              *password;
	GspdfDocument      *document;
	GError             *error;

	gint                start;
	gint                end;
	gdouble             scale;

	GspdfTaskScheduler *task_scheduler;
	GspdfTask 		     *task_loader;
	GSList             *task_renders;
} GspdfPageCachePrivate;

struct _GspdfPageCache {
	GObject parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (GspdfPageCache, gspdf_page_cache, G_TYPE_OBJECT)

enum {
	SIGNAL_DOCUMENT_LOAD_FINISHED = 0,
	SIGNAL_DOCUMENT_RENDER_FINISHED,
	N_SIGNALS
};

static guint obj_signals[N_SIGNALS] = {0};

static gboolean
task_loader_finished (gpointer user_data)
{
	GspdfPageCache *page_cache = (GspdfPageCache*) user_data;
	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	priv->document = gspdf_task_loader_get_document (
		GSPDF_TASK_LOADER (priv->task_loader)
	);

	if (priv->document) {
		g_signal_emit (
			G_OBJECT (page_cache),
			obj_signals[SIGNAL_DOCUMENT_LOAD_FINISHED],
			0
		);
	}
	else {
		g_signal_emit (
			G_OBJECT (page_cache),
			obj_signals[SIGNAL_DOCUMENT_LOAD_FINISHED],
			0
		);
	}

	return FALSE;
}

static gboolean
task_render_finished (gpointer user_data)
{
	GspdfPageCache *page_cache = (GspdfPageCache*) user_data;
	//GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (page_cache);

	g_signal_emit (
			G_OBJECT (page_cache),
			obj_signals[SIGNAL_DOCUMENT_RENDER_FINISHED],
			0
		);

	return FALSE;
}

static void
task_loader_finished_cb (GspdfTask *task,
						             gpointer   user_data)
{
	if (gspdf_task_get_status (task) == GSPDF_TASK_STATUS_OK) {
		g_idle_add (task_loader_finished, user_data);
	}
}

static void
task_render_finished_cb (GspdfTask *task,
						             gpointer   user_data)
{
	if (gspdf_task_get_status (task) == GSPDF_TASK_STATUS_OK) {
		g_idle_add (task_render_finished, user_data);
	}
}

static void
gspdf_page_cache_init (GspdfPageCache *self)
{
	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (self);
	priv->task_scheduler = gspdf_task_scheduler_new ();
	priv->task_loader = gspdf_task_loader_new ();
	priv->scale = 1.0;

	gspdf_task_set_finished_callback (
		priv->task_loader,
		task_loader_finished_cb,
		self
	);
}

static void
gspdf_page_cache_class_init (GspdfPageCacheClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	//obj_signal_document_render_finished_params[0] = G_TYPE_INT;

	obj_signals[SIGNAL_DOCUMENT_LOAD_FINISHED] =  g_signal_newv (
		"document-load-finished",
		 G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		  NULL, NULL, NULL, NULL,
		  G_TYPE_NONE,
		  0, NULL
		  //1, obj_signal_document_load_finished_params
	);

	obj_signals[SIGNAL_DOCUMENT_RENDER_FINISHED] =  g_signal_newv (
		"document-render-finished",
		 G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		  NULL, NULL, NULL, NULL,
		  G_TYPE_NONE,
		  0, NULL
		  //1, obj_signal_document_render_finished_params
	);
}

static void
_task_renders_list_free_func (gpointer data)
{
	GspdfTaskRender *task = (GspdfTaskRender*) data;
	g_object_unref (task);
}

static void
gspdf_page_cache_task_renders_clear (GSList *list)
{
	g_slist_free_full (list, _task_renders_list_free_func);
}

GspdfPageCache *
gspdf_page_cache_new ()
{
	return g_object_new (GSPDF_TYPE_PAGE_CACHE, NULL);
}

void
gspdf_page_cache_open_document (GspdfPageCache *page_cache,
								                const gchar    *uri,
								                const gchar    *password)
{
	g_return_if_fail (page_cache != NULL);
	g_return_if_fail (GSPDF_PAGE_CACHE (page_cache));
	g_return_if_fail (uri != NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	if (priv->document) {
		g_object_unref (priv->document);
		priv->document = NULL;
	}

	if (priv->error) {
		g_error_free (priv->error);
		priv->error = NULL;
	}

	if (priv->uri) {
		g_free (priv->uri);
		priv->uri = NULL;
	}

	if (priv->password) {
		g_free (priv->password);
		priv->password = NULL;
	}

	if (priv->task_renders) {
		GSList *iter = priv->task_renders;

		while (iter != NULL) {
			if (iter->data != NULL) {
				g_object_unref (G_OBJECT (iter->data));
				iter->data = NULL;
			}

			iter = iter->next;
		}

		g_slist_free (priv->task_renders);
		priv->task_renders = NULL;
	}

	priv->uri = g_strdup (uri);
	priv->password = g_strdup (password);
	priv->start = 0;
	priv->end = 0;
	priv->scale = 1.0;

	gspdf_task_loader_set (
		GSPDF_TASK_LOADER (priv->task_loader),
		priv->uri,
		priv->password
	);

	gspdf_task_scheduler_push (priv->task_scheduler, priv->task_loader, TRUE);
}

gchar *
gspdf_page_cache_get_uri (GspdfPageCache *page_cache)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	return g_strdup (priv->uri);
}

GspdfDocument *
gspdf_page_cache_get_document (GspdfPageCache  *page_cache,
                               GError         **error)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	if (!priv->document) {
		if (error) {
			*error = gspdf_task_loader_get_gerror (
				GSPDF_TASK_LOADER (priv->task_loader)
			);
		}

		return NULL;
	}

	g_object_ref (priv->document);

	return priv->document;
}

GPtrArray *
gspdf_page_cache_get_document_map (GspdfPageCache *page_cache)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	if (!priv->task_loader) {
		return NULL;
	}

	if (gspdf_task_get_status (priv->task_loader) != GSPDF_TASK_STATUS_OK) {
		return NULL;
	}

	return gspdf_task_loader_get_document_map (
		GSPDF_TASK_LOADER (priv->task_loader)
	);
}

void
gspdf_page_cache_set_range (GspdfPageCache *page_cache,
							              gint 			      start,
							              gint 			      end,
							              gdouble         scale)
{
	g_return_if_fail (page_cache != NULL);
	g_return_if_fail (GSPDF_PAGE_CACHE (page_cache));

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_if_fail (priv->document != NULL);
	g_return_if_fail ((start >= 0) && (start < gspdf_document_get_n_pages (priv->document)));
	g_return_if_fail ((end >= 0) && (end < gspdf_document_get_n_pages (priv->document)));
	g_return_if_fail (end >= start);

	priv->start = start;
	priv->end = end;
	priv->scale = scale;

	GSList *temp = NULL;
	GspdfTask *task = NULL;
	gboolean found = FALSE;

	for (gint i = priv->start; i <= priv->end; i++) {
		if (priv->task_renders) {
			GSList *iter = priv->task_renders;

			while (iter) {
				task = (GspdfTask*) iter->data;

				if (gspdf_task_render_get_index (GSPDF_TASK_RENDER (task)) == i) {
					temp = g_slist_append (temp, task);
					g_object_ref (task);
					found = TRUE;
					break;
				}

				iter = iter->next;
			}
		}

		if (found) {
			found = FALSE;
			continue;
		}

		task = gspdf_task_render_new ();

		gspdf_task_render_set (
			GSPDF_TASK_RENDER (task),
			priv->document,
			i,
			priv->scale
		);

		gspdf_task_set_finished_callback (
			task,
			task_render_finished_cb,
			page_cache
		);

		temp = g_slist_append (temp, task);

		gspdf_task_scheduler_push (priv->task_scheduler, task, TRUE);
	}

	if (priv->task_renders) {
		gspdf_page_cache_task_renders_clear (priv->task_renders);
		priv->task_renders = NULL;
	}

	priv->task_renders = temp;
}

void
gspdf_page_cache_get_range (GspdfPageCache *page_cache,
							              gint 		       *start,
							              gint 		       *end)
{
	g_return_if_fail (page_cache != NULL);
	g_return_if_fail (GSPDF_PAGE_CACHE (page_cache));

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	if (start) {
		*start = priv->start;
	}

	if (end) {
		*end = priv->end;
	}
}

void
gspdf_page_cache_set_scale (GspdfPageCache *page_cache,
							              gdouble 	      scale)
{
	g_return_if_fail (page_cache != NULL);
	g_return_if_fail (GSPDF_PAGE_CACHE (page_cache));

	//priv->scale = scale;
}

GdkPixbuf *
gspdf_page_cache_get_pixbuf (GspdfPageCache *page_cache,
							               gint 			     index)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_val_if_fail (priv->document != NULL, NULL);
	g_return_val_if_fail ((index >= priv->start) && (index <= priv->end), NULL);
	g_return_val_if_fail (priv->task_renders != NULL, NULL);

	GSList *iter = priv->task_renders;
	GspdfTask *task = NULL;
	GdkPixbuf *ret = NULL;

	while (iter) {
		task = (GspdfTask*) iter->data;

		if (gspdf_task_render_get_index (GSPDF_TASK_RENDER (task)) == index) {
			if (gspdf_task_get_status (task) == GSPDF_TASK_STATUS_OK) {
				ret = gspdf_task_render_get_pixbuf (GSPDF_TASK_RENDER (task));
			}

			break;
		}

		iter = iter->next;
	}

	return ret;
}

GList *
gspdf_page_cache_get_selected_region (GspdfPageCache       *page_cache,
									                    gint                  index,
	                                    GspdfSelectionStyle   style,
	                                    const GspdfRectangle *selection)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_val_if_fail (priv->document != NULL, NULL);
	g_return_val_if_fail ((index >= priv->start) && (index <= priv->end), NULL);

	GspdfDocumentPage *page = gspdf_document_get_page (priv->document, index);
	GList *ret = gspdf_document_page_get_selected_region (page, style, selection);

	g_object_unref (page);

	return ret;
}

gchar *
gspdf_page_cache_get_selected_text (GspdfPageCache       *page_cache,
									                  gint                  index,
	                                  GspdfSelectionStyle   style,
	                                  const GspdfRectangle *selection)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_val_if_fail (priv->document != NULL, NULL);
	g_return_val_if_fail ((index >= priv->start) && (index <= priv->end), NULL);

	GspdfDocumentPage *page = gspdf_document_get_page (priv->document, index);
	gchar *ret = gspdf_document_page_get_selected_text (page, style, selection);

	g_object_unref (page);

	return ret;
}

GList *
gspdf_page_cache_get_text_mapping (GspdfPageCache *page_cache,
							                     gint 		       index)
{
	g_return_val_if_fail (page_cache != NULL, NULL);
	g_return_val_if_fail (GSPDF_PAGE_CACHE (page_cache), NULL);

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_val_if_fail (priv->document != NULL, NULL);
	g_return_val_if_fail ((index >= priv->start) && (index <= priv->end), NULL);

	GList *iter = priv->task_renders;
	GspdfTask *task_render = NULL;

	while (iter) {
		task_render = (GspdfTask*) iter->data;

		if (task_render) {
			if (gspdf_task_render_get_index (GSPDF_TASK_RENDER (task_render)) == index) {
				return gspdf_task_render_get_text_mapping (
					GSPDF_TASK_RENDER (task_render)
				);
			}
		}

		iter = iter->next;
	}
}

void
gspdf_page_cache_clear (GspdfPageCache *page_cache)
{
	g_return_if_fail (page_cache != NULL);
	g_return_if_fail (GSPDF_PAGE_CACHE (page_cache));

	GspdfPageCachePrivate *priv = gspdf_page_cache_get_instance_private (
		page_cache
	);

	g_return_if_fail (priv->document != NULL);

	gspdf_page_cache_task_renders_clear (priv->task_renders);
	priv->task_renders = NULL;
}
