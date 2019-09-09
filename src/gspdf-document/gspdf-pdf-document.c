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

#include "gspdf-pdf-document.h"

#ifndef __POPPLER_GLIB_H__
#include <poppler.h>
#endif

#ifndef GSPDF_PDF_DOCUMENT_PAGE_H
#include "gspdf-pdf-document-page.h"
#endif

struct _GspdfPdfDocument {
	GspdfDocument parent;
};

G_DEFINE_TYPE (
	GspdfPdfDocument,
	gspdf_pdf_document,
	GSPDF_TYPE_DOCUMENT
)

static GspdfDocument *
gspdf_pdf_document_new_s (const char *uri,
	                        const char *password,
												  GError     **error)
{
	GError *perror = NULL;
	PopplerDocument *handler = poppler_document_new_from_file (
		uri,
		password,
		&perror
	);

	if (!handler) {
		if (perror) {
			if (error) {
				switch (perror->code) {
					case POPPLER_ERROR_ENCRYPTED:
						*error = g_error_new_literal (
							g_quark_from_static_string (
								"gspdf-document-error-encrypted"
							),
							GSPDF_DOCUMENT_ERROR_ENCRYPTED,
							"Document is encrypted"
						);
						break;
					default:
						*error = g_error_new_literal (
							g_quark_from_static_string (
								"gspdf-document-error-invalid"
							),
							GSPDF_DOCUMENT_ERROR_INVALID,
							"Document is invalid"
						);
						break;
				}
			}

			g_error_free (perror);
 		}

		return NULL;
	}

	GspdfDocument *doc = g_object_new (GSPDF_TYPE_PDF_DOCUMENT, NULL);
	gchar *author = poppler_document_get_author (handler);
	gchar *creator = poppler_document_get_creator (handler);
	gchar *keywords = poppler_document_get_keywords (handler);
	gchar *producer = poppler_document_get_producer (handler);
	gchar *subject = poppler_document_get_subject (handler);
	gchar *title = poppler_document_get_title (handler);
	GspdfDocumentPageLayout page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_NO_LAYOUT;
	GspdfDocumentPageMode page_mode = GSPDF_DOCUMENT_PAGE_MODE_NONE;

	PopplerPageLayout poppler_page_layout = poppler_document_get_page_layout (
		handler
	);

	switch (poppler_page_layout) {
		case POPPLER_PAGE_LAYOUT_SINGLE_PAGE:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE;
			break;
		case POPPLER_PAGE_LAYOUT_ONE_COLUMN:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN;
			break;
		case POPPLER_PAGE_LAYOUT_TWO_COLUMN_LEFT:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT;
			break;
		case POPPLER_PAGE_LAYOUT_TWO_COLUMN_RIGHT:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
			break;
		case POPPLER_PAGE_LAYOUT_TWO_PAGE_LEFT:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT;
			break;
		case POPPLER_PAGE_LAYOUT_TWO_PAGE_RIGHT:
			page_layout = GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT;
			break;
		default:
			break;
	}

	PopplerPageMode poppler_page_mode = poppler_document_get_page_mode (
		handler
	);

	switch (poppler_page_mode) {
		case POPPLER_PAGE_MODE_USE_OUTLINES:
			page_mode = GSPDF_DOCUMENT_PAGE_MODE_USE_OUTLINES;
			break;
		case POPPLER_PAGE_MODE_USE_THUMBS:
			page_mode = GSPDF_DOCUMENT_PAGE_MODE_USE_THUMBS;
			break;
		case POPPLER_PAGE_MODE_FULL_SCREEN:
			page_mode = GSPDF_DOCUMENT_PAGE_MODE_FULL_SCREEN;
			break;
		case POPPLER_PAGE_MODE_USE_OC:
			page_mode = GSPDF_DOCUMENT_PAGE_MODE_USE_OC;
			break;
		case POPPLER_PAGE_MODE_USE_ATTACHMENTS:
			page_mode = GSPDF_DOCUMENT_PAGE_MODE_USE_ATTACHMENTS;
			break;
		default:
			break;
	}

	g_object_set (G_OBJECT (doc),
		"handler", handler,
		"author", author,
		"creator", creator,
		"keywords", keywords,
		"producer", producer,
		"subject", subject,
		"title", title,
		"page-layout", page_layout,
		"page-mode", page_mode,
		NULL);

	g_free (author);
	g_free (creator);
	g_free (keywords);
	g_free (producer);
	g_free (subject);
	g_free (title);

	return doc;
}

static gboolean
gspdf_pdf_document_save (GspdfDocument *doc,
					               const gchar   *uri,
					               GError       **error)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, FALSE);

	return poppler_document_save (handler, uri, error);
}

static gboolean
gspdf_pdf_document_linearized (GspdfDocument *doc)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, FALSE);

	return poppler_document_is_linearized (handler);
}

static gint
gspdf_pdf_document_get_n_pages (GspdfDocument *doc)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, -1);

	return poppler_document_get_n_pages (handler);
}

static GspdfDocumentPage *
gspdf_pdf_document_get_page (GspdfDocument *doc, gint index)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerPage *poppler_page = poppler_document_get_page (handler, index);
	if (!poppler_page) {
		return NULL;
	}

	GspdfDocumentPage *doc_page = g_object_new (
		GSPDF_TYPE_PDF_DOCUMENT_PAGE,
		NULL
	);
	g_object_set (G_OBJECT (doc_page), "handler", poppler_page, NULL);

	return doc_page;
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

static void
_poppler_index_iter_walk (PopplerIndexIter *poppler_index_iter,
	                        GspdfDocOutline **outline)
{
	PopplerIndexIter *iter  = poppler_index_iter;
	GspdfDocOutline  *oiter = NULL;

	*outline = g_malloc0 (sizeof (GspdfDocOutline));
	oiter = *outline;

	while (1) {
		PopplerAction *action = poppler_index_iter_get_action (iter);
		if (action) {
			oiter->action = _get_action (action);
			poppler_action_free (action);
		}

		PopplerIndexIter *child  = poppler_index_iter_get_child (iter);
		if (child) {
			_poppler_index_iter_walk (child, &(oiter->child));
			poppler_index_iter_free (child);
		}

		if (!poppler_index_iter_next (iter)) {
			break;
		}

		oiter->next = g_malloc0 (sizeof (GspdfDocOutline));
		oiter = oiter->next;
	}
}

GspdfDocOutline *
_get_outline (PopplerIndexIter *poppler_index_iter)
{
	GspdfDocOutline *ret = NULL;
	_poppler_index_iter_walk (poppler_index_iter, &ret);

	return ret;
}

static GspdfDocOutline *
gspdf_pdf_document_get_outline (GspdfDocument *doc)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerIndexIter *poppler_index_iter = poppler_index_iter_new (handler);
	g_return_val_if_fail (poppler_index_iter != NULL, NULL);

	GspdfDocOutline *ret = _get_outline (poppler_index_iter);

	poppler_index_iter_free (poppler_index_iter);

	return ret;
}

static GspdfDocDest *
gspdf_pdf_document_find_dest (GspdfDocument *doc,
						                    const gchar   *named_dest)
{
	PopplerDocument *handler = NULL;
	g_object_get (G_OBJECT (doc), "handler", &handler, NULL);
	g_return_val_if_fail (handler != NULL, NULL);

	PopplerDest *poppler_dest = poppler_document_find_dest (handler, named_dest);
	g_return_val_if_fail (poppler_dest != NULL, NULL);

	GspdfDocDest *ret = _get_dest (poppler_dest);

	poppler_dest_free (poppler_dest);

	return ret;
}

static void
gspdf_pdf_document_dispose (GObject *object)
{
	PopplerDocument *handler = NULL;

	g_object_get (G_OBJECT (object), "handler", &handler, NULL);

	if (handler != NULL) {
		g_object_unref (handler);
	}

	G_OBJECT_CLASS (gspdf_pdf_document_parent_class)->dispose (object);
}

static void
gspdf_pdf_document_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_pdf_document_parent_class)->finalize (object);
}

static void
gspdf_pdf_document_init (GspdfPdfDocument *self)
{

}

static void
gspdf_pdf_document_class_init (GspdfPdfDocumentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GspdfDocumentClass *parent = GSPDF_DOCUMENT_CLASS (klass);

	object_class->dispose = gspdf_pdf_document_dispose;
	object_class->finalize = gspdf_pdf_document_finalize;

	parent->save = gspdf_pdf_document_save;
	parent->linearized = gspdf_pdf_document_linearized;
	parent->get_n_pages = gspdf_pdf_document_get_n_pages;
	parent->get_page = gspdf_pdf_document_get_page;
	parent->get_outline = gspdf_pdf_document_get_outline;
	parent->find_dest = gspdf_pdf_document_find_dest;
}

GspdfDocument *
gspdf_pdf_document_new (const gchar *uri,
						            const gchar *password,
						            GError      **error)
{
	return gspdf_pdf_document_new_s (uri, password, error);
}
