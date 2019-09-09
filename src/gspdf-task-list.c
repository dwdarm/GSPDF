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

#include "gspdf-task-list.h"

/**
 * GspdfTaskLoader
 */

typedef struct {

	gchar         *uri;
	gchar         *password;
	GspdfDocument *document;
	GError        *error;

	GPtrArray     *doc_map;

} GspdfTaskLoaderPrivate;

struct _GspdfTaskLoader {
	GspdfTask parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (
	GspdfTaskLoader,
	gspdf_task_loader,
	GSPDF_TYPE_TASK
)

static void
_doc_map_free_func (gpointer data)
{
	GspdfDocMap *doc_map = (GspdfDocMap*) data;
	g_free (doc_map);
}

static GPtrArray *
_doc_map_init (GspdfDocument *doc)
{
	GPtrArray *ret = g_ptr_array_new_with_free_func (_doc_map_free_func);
	gint pages = gspdf_document_get_n_pages (doc);
	GspdfDocumentPage *page = NULL;
	GspdfDocMap *map = NULL;

	for (gint i = 0; i < pages; i++) {
		page = gspdf_document_get_page (doc, i);
		map = g_malloc0 (sizeof (GspdfDocMap));
		map->width = gspdf_document_page_get_width (page);
		map->height = gspdf_document_page_get_height (page);
		g_ptr_array_add (ret, map);
		g_object_unref (page);
	}

	return ret;
}

static gboolean
gspdf_task_loader_run (GspdfTask *task)
{
	GspdfTaskLoader *task_loader = GSPDF_TASK_LOADER (task);
	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (
		task_loader
	);

	g_return_val_if_fail (priv->uri != NULL, FALSE);

	if (priv->document) {
		g_object_unref (priv->document);
		priv->document = NULL;
	}

	if (priv->error) {
		g_error_free (priv->error);
		priv->error = NULL;
	}

	if (priv->doc_map) {
		g_ptr_array_unref (priv->doc_map);
		priv->doc_map = NULL;
	}

	priv->document = gspdf_document_new_from_file (
		priv->uri,
		priv->password,
		&priv->error
	);

	if (!priv->document) {
		g_print ("%s\n", priv->error->message);
	} else {
		priv->doc_map = _doc_map_init (priv->document);
	}

	return FALSE;
}

static void
gspdf_task_loader_dispose (GObject *object)
{
	GspdfTaskLoader *task_loader = GSPDF_TASK_LOADER (object);
	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task_loader);

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

	if (priv->doc_map) {
		g_ptr_array_unref (priv->doc_map);
	}

	G_OBJECT_CLASS (gspdf_task_loader_parent_class)->dispose (object);
}

static void
gspdf_task_loader_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_task_loader_parent_class)->finalize (object);
}

static void
gspdf_task_loader_init (GspdfTaskLoader *task)
{

}

static void
gspdf_task_loader_class_init (GspdfTaskLoaderClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GspdfTaskClass *task_class = GSPDF_TASK_CLASS (klass);

	object_class->dispose = gspdf_task_loader_dispose;
	object_class->finalize = gspdf_task_loader_finalize;
	task_class->run = gspdf_task_loader_run;
}

GspdfTask *
gspdf_task_loader_new (void)
{
	return g_object_new (GSPDF_TYPE_TASK_LOADER, NULL);
}

void
gspdf_task_loader_set (GspdfTaskLoader *task,
	                     const gchar     *uri,
	                     const gchar     *password)
{
	g_return_if_fail (task != NULL);
	g_return_if_fail (GSPDF_IS_TASK_LOADER (task));
	g_return_if_fail (uri != NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

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

	priv->uri = g_strdup (uri);
	priv->password = g_strdup (password);
}

void
gspdf_task_loader_set_uri (GspdfTaskLoader *task,
	                         const gchar     *uri)
{
	g_return_if_fail (task != NULL);
	g_return_if_fail (GSPDF_IS_TASK_LOADER (task));

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	if (priv->uri) {
		g_free (priv->uri);
		priv->uri = NULL;
	}

	priv->uri = g_strdup (uri);
}

gchar *
gspdf_task_loader_get_uri (GspdfTaskLoader *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_LOADER (task), NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	return priv->uri;
}

void
gspdf_task_loader_set_password (GspdfTaskLoader *task,
	                              const gchar     *password)
{
	g_return_if_fail (task != NULL);
	g_return_if_fail (GSPDF_IS_TASK_LOADER (task));

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	if (priv->password) {
		g_free (priv->password);
		priv->password = NULL;
	}

	priv->password = g_strdup (password);
}

gchar *
gspdf_task_loader_get_password (GspdfTaskLoader *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_LOADER (task), NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	return priv->password;
}

GspdfDocument *
gspdf_task_loader_get_document (GspdfTaskLoader *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_LOADER (task), NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	g_return_val_if_fail (priv->document != NULL, NULL);

	g_object_ref (priv->document);

	return priv->document;
}

GPtrArray *
gspdf_task_loader_get_document_map (GspdfTaskLoader *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_LOADER (task), NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	g_return_val_if_fail (priv->doc_map != NULL, NULL);

	g_ptr_array_ref (priv->doc_map);

	return priv->doc_map;
}

GError *
gspdf_task_loader_get_gerror (GspdfTaskLoader *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_LOADER (task), NULL);

	GspdfTaskLoaderPrivate *priv = gspdf_task_loader_get_instance_private (task);

	if (!priv->error) {
		return NULL;
	}

	return g_error_copy (priv->error);
}

/**
 * GspdfTaskRender
 */

typedef struct {
	GspdfDocument     *document;
	GspdfDocumentPage *page;
	GdkPixbuf         *pixbuf;
	gint               index;
	gdouble            scale;
	GList             *text_mapping;
} GspdfTaskRenderPrivate;

struct _GspdfTaskRender {
	GspdfTask parent;
};

G_DEFINE_TYPE_WITH_PRIVATE (
	GspdfTaskRender,
	gspdf_task_render,
	GSPDF_TYPE_TASK
)

static void
_list_rectangle_free_func (gpointer data)
{
	g_free ((GspdfRectangle*)data);
}

static gboolean
gspdf_task_render_run (GspdfTask *task)
{
	GspdfTaskRender *task_render = GSPDF_TASK_RENDER (task);
	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (task_render);

	g_return_val_if_fail (priv->document != NULL, FALSE);

	if (priv->page) {
		g_object_unref (priv->page);
		priv->page = NULL;
	}

	priv->page = gspdf_document_get_page (priv->document, priv->index);

	g_return_val_if_fail (priv->page != NULL, FALSE);

	if (priv->pixbuf) {
		g_object_unref (priv->pixbuf);
		priv->pixbuf = NULL;
	}

	priv->pixbuf = gspdf_document_page_render (
		priv->page,
		priv->scale,
		priv->scale
	);

	if (priv->text_mapping) {
		g_list_free_full (priv->text_mapping, _list_rectangle_free_func);
		priv->text_mapping = NULL;
	}

	const GspdfRectangle rect = {
		0,
		0,
		gspdf_document_page_get_width (priv->page),
		gspdf_document_page_get_height (priv->page)
	};

	priv->text_mapping = gspdf_document_page_get_selected_region (
		priv->page,
		GSPDF_SELECTION_LINE,
		&rect
	);

	return FALSE;
}

static void
gspdf_task_render_dispose (GObject *object)
{
	GspdfTaskRender *task_render = GSPDF_TASK_RENDER (object);
	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (
		task_render
	);

	if (priv->document) {
		g_object_unref (priv->document);
		priv->document = NULL;
	}

	if (priv->page) {
		g_object_unref (priv->page);
		priv->page = NULL;
	}

	if (priv->pixbuf) {
		g_object_unref (priv->pixbuf);
		priv->pixbuf = NULL;
	}

	if (priv->text_mapping) {
		g_list_free_full (priv->text_mapping, _list_rectangle_free_func);
		priv->text_mapping = NULL;
	}

	G_OBJECT_CLASS (gspdf_task_loader_parent_class)->dispose (object);
}

static void
gspdf_task_render_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_task_render_parent_class)->finalize (object);
}

static void
gspdf_task_render_init (GspdfTaskRender *task)
{

}

static void
gspdf_task_render_class_init (GspdfTaskRenderClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GspdfTaskClass *task_class = GSPDF_TASK_CLASS (klass);

	object_class->dispose = gspdf_task_render_dispose;
	object_class->finalize = gspdf_task_render_finalize;

	task_class->run = gspdf_task_render_run;
}

GspdfTask *
gspdf_task_render_new (void)
{
	return g_object_new (GSPDF_TYPE_TASK_RENDER, NULL);
}

void
gspdf_task_render_set (GspdfTaskRender *task,
	                     GspdfDocument *doc,
											 gint index,
											 gdouble scale)
{
	g_return_if_fail (task != NULL);
	g_return_if_fail (GSPDF_IS_TASK_RENDER (task));
	g_return_if_fail (doc != NULL);
	g_return_if_fail (GSPDF_IS_DOCUMENT (doc));

	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (task);

	if (priv->document) {
		g_object_unref (priv->document);
		priv->document = NULL;
	}

	if (priv->page) {
		g_object_unref (priv->page);
		priv->page = NULL;
	}

	if (priv->pixbuf) {
		g_object_unref (priv->pixbuf);
		priv->pixbuf = NULL;
	}

	if (priv->text_mapping) {
		g_list_free_full (priv->text_mapping, _list_rectangle_free_func);
		priv->text_mapping = NULL;
	}

	priv->index = index;
	priv->scale = scale;
	priv->document = doc;

	g_object_ref (priv->document);
}

gint
gspdf_task_render_get_index (GspdfTaskRender *task)
{
	g_return_val_if_fail (task != NULL, -1);
	g_return_val_if_fail (GSPDF_IS_TASK_RENDER (task), -1);

	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (task);

	return priv->index;
}

GdkPixbuf *
gspdf_task_render_get_pixbuf (GspdfTaskRender *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_RENDER (task), NULL);

	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (task);

	g_return_val_if_fail (priv->pixbuf != NULL, NULL);

	g_object_ref (priv->pixbuf);

	return priv->pixbuf;
}

GList *
gspdf_task_render_get_text_mapping (GspdfTaskRender *task)
{
	g_return_val_if_fail (task != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_TASK_RENDER (task), NULL);

	GspdfTaskRenderPrivate *priv = gspdf_task_render_get_instance_private (task);

	return priv->text_mapping;
}
