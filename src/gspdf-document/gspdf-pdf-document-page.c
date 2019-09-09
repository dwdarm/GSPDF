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

#include "gspdf-pdf-document-page.h"

#ifndef __POPPLER_GLIB_H__
#include <poppler.h>
#endif

#include <math.h>

struct _GspdfPdfDocumentPage {
	GspdfDocumentPage parent;
};

G_DEFINE_TYPE (
	GspdfPdfDocumentPage,
	gspdf_pdf_document_page,
	GSPDF_TYPE_DOCUMENT_PAGE
)

static void
on_g_list_poppler_rect_free_func (gpointer data)
{
	poppler_rectangle_free ((PopplerRectangle*)data);
}

static gint
gspdf_pdf_document_page_get_index (GspdfDocumentPage *doc_page)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, -1);

	return poppler_page_get_index (handler);
}

static gchar *
gspdf_pdf_document_page_get_label (GspdfDocumentPage *doc_page)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	return poppler_page_get_label (handler);
}

static gdouble
gspdf_pdf_document_page_get_width (GspdfDocumentPage *doc_page)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, -1);

	gdouble ret = -1;
	poppler_page_get_size (handler, &ret, NULL);

	return ret;
}

static gdouble
gspdf_pdf_document_page_get_height (GspdfDocumentPage *doc_page)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, -1);

	gdouble ret = -1;
	poppler_page_get_size (handler, NULL, &ret);

	return ret;
}

static void
_pixbuf_destroy_notify_func (guchar *pixels, gpointer data)
{
	g_free (pixels);
}

static GdkPixbuf *
gspdf_pdf_document_page_render (GspdfDocumentPage *doc_page,
	                              gdouble            sx,
															  gdouble            sy)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	const gdouble original_width = gspdf_document_page_get_width (doc_page);
	const gdouble original_height = gspdf_document_page_get_height (doc_page);
	const gdouble scaled_width = ceil (original_width * sx);
	const gdouble scaled_height = ceil (original_height * sy);
	const gdouble stride = scaled_width * 4;
	const gsize scaled_width_blk = (gsize) scaled_width;
	const gsize scaled_height_blk = (gsize) scaled_height;
	const gsize stride_blk = (gsize) stride;

	guchar *data = g_malloc0 (stride_blk * scaled_height_blk);
	cairo_surface_t *surface = cairo_image_surface_create_for_data(
		data,
		CAIRO_FORMAT_ARGB32,
		scaled_width_blk,
		scaled_height_blk,
		stride_blk
	);
	cairo_t *ctx = cairo_create (surface);

	cairo_set_source_rgb (ctx, 1.0, 1.0, 1.0);
	cairo_rectangle (ctx, 0, 0, scaled_width_blk, scaled_height_blk);
	cairo_fill (ctx);
	cairo_scale (ctx, sx, sy);
	poppler_page_render (handler, ctx);

	cairo_surface_destroy (surface);
	cairo_destroy (ctx);

	return gdk_pixbuf_new_from_data (
		data,
		GDK_COLORSPACE_RGB,
		TRUE,
		8,
		scaled_width_blk,
		scaled_height_blk,
		stride_blk,
		_pixbuf_destroy_notify_func,
		NULL
	);
}

static GList *
gspdf_pdf_document_page_get_selected_region (GspdfDocumentPage    *doc_page,
	                                           GspdfSelectionStyle   style,
	                                           const GspdfRectangle *selection)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerSelectionStyle poppler_style = POPPLER_SELECTION_GLYPH;
	if (style == GSPDF_SELECTION_LINE) {
		poppler_style = POPPLER_SELECTION_LINE;
	}
	else if (style == GSPDF_SELECTION_WORD) {
		poppler_style = POPPLER_SELECTION_WORD;
	}

	PopplerRectangle selection_area = {
		selection->x,
		selection->y,
		selection->x + selection->width,
		selection->y + selection->height
	};

	cairo_region_t *cairo_regions = poppler_page_get_selected_region (
		handler,
		1.0,
		poppler_style,
		&selection_area
	);

	if (!cairo_regions) {
		return NULL;
	}

	GList *ret = NULL;
	GspdfRectangle *temp_area = NULL;
	cairo_rectangle_int_t cairo_rect;

	for (int i = 0; i < cairo_region_num_rectangles (cairo_regions); i++) {
		cairo_region_get_rectangle (cairo_regions, i, &cairo_rect);
		temp_area = g_malloc (sizeof (GspdfRectangle));
		temp_area->x = cairo_rect.x;
		temp_area->y = cairo_rect.y;
		temp_area->width = cairo_rect.width;
		temp_area->height = cairo_rect.height;
		ret = g_list_append (ret, temp_area);
	}

	cairo_region_destroy (cairo_regions);

	return ret;
}

static gchar *
gspdf_pdf_document_page_get_selected_text (GspdfDocumentPage    *doc_page,
	                                         GspdfSelectionStyle   style,
	                                         const GspdfRectangle *selection)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerSelectionStyle poppler_style = POPPLER_SELECTION_GLYPH;
	if (style == GSPDF_SELECTION_LINE) {
		poppler_style = POPPLER_SELECTION_LINE;
	}
	else if (style == GSPDF_SELECTION_WORD) {
		poppler_style = POPPLER_SELECTION_WORD;
	}

	PopplerRectangle selection_area = {
		selection->x,
		selection->y,
		selection->x + selection->width,
		selection->y + selection->height
	};

	return poppler_page_get_selected_text (
		handler,
		poppler_style,
		&selection_area
	);
}


static GspdfDocDest *
_get_dest (PopplerDest *dest)
{
	GspdfDocDest *ret = g_malloc0 (sizeof (GspdfDocDest));
	ret->index = dest->page_num;
	ret->left = dest->left;
	ret->bottom = dest->bottom;
	ret->right = dest->right;
	ret->top = dest->top;
	ret->zoom = dest->zoom;
	ret->change_left = dest->change_left;
	ret->top = dest->top;
	ret->zoom = dest->zoom;
	ret->named_dest = g_strdup (dest->named_dest);

	return ret;
}

static GspdfDocAction *
_get_action (PopplerAction *action)
{
	PopplerActionAny *act = (PopplerActionAny*) action;
	GspdfDocAction *ret = NULL;

	if (act->type == POPPLER_ACTION_UNKNOWN) {
		GspdfDocActionAny *gact = g_malloc0 (sizeof (GspdfDocActionAny));
		gact->type = GSPDF_DOC_ACTION_UNKNOWN;
		gact->title = g_strdup (act->title);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_NONE) {
		GspdfDocActionAny *gact = g_malloc0 (sizeof (GspdfDocActionAny));
		gact->type = GSPDF_DOC_ACTION_NONE;
		gact->title = g_strdup (act->title);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_GOTO_DEST) {
		GspdfDocActionGotoDest *gact = g_malloc0 (sizeof (GspdfDocActionGotoDest));
		gact->type = GSPDF_DOC_ACTION_GOTO_DEST;
		gact->title = g_strdup (act->title);
		gact->dest = _get_dest (((PopplerActionGotoDest*)act)->dest);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_GOTO_REMOTE) {
		GspdfDocActionGotoRemote *gact = g_malloc0 (sizeof (GspdfDocActionGotoRemote));
		gact->type = GSPDF_DOC_ACTION_GOTO_REMOTE;
		gact->title = g_strdup (act->title);
		gact->file_name = g_strdup (((PopplerActionGotoRemote*)act)->file_name);
		gact->dest = _get_dest (((PopplerActionGotoRemote*)act)->dest);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_LAUNCH) {
		GspdfDocActionLaunch  *gact = g_malloc0 (sizeof (GspdfDocActionLaunch));
		gact->type = GSPDF_DOC_ACTION_LAUNCH;
		gact->title = g_strdup (act->title);
		gact->file_name = g_strdup (((PopplerActionLaunch*)act)->file_name);
		gact->params = g_strdup (((PopplerActionLaunch*)act)->params);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_URI) {
		GspdfDocActionUri   *gact = g_malloc0 (sizeof (GspdfDocActionUri));
		gact->type = GSPDF_DOC_ACTION_URI;
		gact->title = g_strdup (act->title);
		gact->uri = g_strdup (((PopplerActionUri*)act)->uri);
		ret = (GspdfDocAction*) gact;
	}
	else if (act->type == POPPLER_ACTION_NAMED) {
		GspdfDocActionNamed   *gact = g_malloc0 (sizeof (GspdfDocActionNamed));
		gact->type = GSPDF_DOC_ACTION_NAMED;
		gact->title = g_strdup (act->title);
		gact->named_dest = g_strdup (((PopplerActionNamed*)act)->named_dest);
		ret = (GspdfDocAction*) gact;
	}

	return ret;
}

static GList *
gspdf_pdf_document_page_get_link_mapping (GspdfDocumentPage *doc_page)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	GList *links = poppler_page_get_link_mapping (handler);
	if (!links) {
		return NULL;
	}

	gdouble width = -1, height = -1;
	poppler_page_get_size (handler, &width, &height);

	GList *ret = NULL;
	GspdfDocLinkMapping *link_mapping = NULL;
	PopplerLinkMapping *poppler_link_mapping = NULL;
	GList *iter = links;

	while (iter) {
		poppler_link_mapping = (PopplerLinkMapping*) iter->data;

		link_mapping = g_malloc0 (sizeof (GspdfDocLinkMapping));
		link_mapping->area.x = poppler_link_mapping->area.x1;
		link_mapping->area.y = height - poppler_link_mapping->area.y2;
		link_mapping->area.width =
			poppler_link_mapping->area.x2 - poppler_link_mapping->area.x1;
		link_mapping->area.height =
			poppler_link_mapping->area.y2 - poppler_link_mapping->area.y1;
		link_mapping->action = _get_action (poppler_link_mapping->action);

		ret = g_list_append (ret, link_mapping);

		iter = iter->next;
	}

	poppler_page_free_link_mapping (links);

	return ret;
}

static GList *
gspdf_pdf_document_page_find_text (GspdfDocumentPage *doc_page,
						                       const gchar       *text,
						                       GspdfFindFlags     options)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (doc_page), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerFindFlags poppler_find_flags = 0;

	if (options & (~GSPDF_FIND_DEFAULT)) {
		poppler_find_flags |= POPPLER_FIND_DEFAULT;
	}

	if (options & (~GSPDF_FIND_CASE_SENSITIVE)) {
		poppler_find_flags |= POPPLER_FIND_CASE_SENSITIVE;
	}

	if (options & (~GSPDF_FIND_BACKWARDS)) {
		poppler_find_flags |= POPPLER_FIND_BACKWARDS;
	}

	if (options & (~GSPDF_FIND_WHOLE_WORDS_ONLY)) {
		poppler_find_flags |= POPPLER_FIND_WHOLE_WORDS_ONLY;
	}

	GList *texts = poppler_page_find_text_with_options (
		handler,
		text,
		poppler_find_flags
	);

	if (!texts) {
		return NULL;
	}

	gdouble width = -1, height = -1;
	poppler_page_get_size (handler, &width, &height);

	GList *iter = texts;
	GList *ret = NULL;
	PopplerRectangle *poppler_rect = NULL;
	GspdfRectangle *temp_rect = NULL;

	while (iter) {
		poppler_rect = (PopplerRectangle*) iter->data;

		temp_rect = g_malloc0 (sizeof (GspdfRectangle));
		temp_rect->x = poppler_rect->x1;
		temp_rect->y = height - poppler_rect->y2;
		temp_rect->width = poppler_rect->x2 - poppler_rect->x1;
		temp_rect->height = poppler_rect->y2 - poppler_rect->y1;

		ret = g_list_append (ret, temp_rect);
		iter = iter->next;
	}

	g_list_free_full (texts, on_g_list_poppler_rect_free_func);

	return ret;
}

static void
gspdf_pdf_document_page_dispose (GObject *object)
{
	PopplerPage *handler = NULL;
	g_object_get (G_OBJECT (object), "handler", &handler, NULL);

	if (handler != NULL) {
		g_object_unref (handler);
	}

	G_OBJECT_CLASS (gspdf_pdf_document_page_parent_class)->dispose (object);
}

static void
gspdf_pdf_document_page_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_pdf_document_page_parent_class)->finalize (object);
}

static void
gspdf_pdf_document_page_init (GspdfPdfDocumentPage *self)
{

}

static void
gspdf_pdf_document_page_class_init (GspdfPdfDocumentPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GspdfDocumentPageClass *parent = GSPDF_DOCUMENT_PAGE_CLASS (klass);

	object_class->dispose = gspdf_pdf_document_page_dispose;
	object_class->finalize = gspdf_pdf_document_page_finalize;

	parent->get_index = gspdf_pdf_document_page_get_index;
	parent->get_label = gspdf_pdf_document_page_get_label;
	parent->get_width = gspdf_pdf_document_page_get_width;
	parent->get_height = gspdf_pdf_document_page_get_height;
	parent->render = gspdf_pdf_document_page_render;
	parent->get_selected_region = gspdf_pdf_document_page_get_selected_region;
	parent->get_selected_text = gspdf_pdf_document_page_get_selected_text;
	parent->get_link_mapping = gspdf_pdf_document_page_get_link_mapping;
	parent->find_text = gspdf_pdf_document_page_find_text;
}
