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

#ifndef GSPDF_DOCUMENT_H
#define GSPDF_DOCUMENT_H

#ifndef __GLIB_GOBJECT_H__
#include <glib-object.h>
#endif

#ifndef GSPDF_DOCUMENT_PAGE_H
#include "gspdf-document-page.h"
#endif

#ifndef GSPDF_DOC_ACTION_H
#include "gspdf-doc-action.h"
#endif

G_BEGIN_DECLS

typedef struct _GspdfDocOutline GspdfDocOutline;

typedef enum {
	GSPDF_DOCUMENT_PAGE_LAYOUT_NO_LAYOUT,
	GSPDF_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE,
	GSPDF_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN,
	GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT,
	GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT,
	GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT,
	GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT
} GspdfDocumentPageLayout;

typedef enum {
	GSPDF_DOCUMENT_PAGE_MODE_NONE,
	GSPDF_DOCUMENT_PAGE_MODE_USE_OUTLINES,
	GSPDF_DOCUMENT_PAGE_MODE_USE_THUMBS,
	GSPDF_DOCUMENT_PAGE_MODE_FULL_SCREEN,
	GSPDF_DOCUMENT_PAGE_MODE_USE_OC,
	GSPDF_DOCUMENT_PAGE_MODE_USE_ATTACHMENTS
} GspdfDocumentPageMode;

typedef enum {
	GSPDF_DOCUMENT_ERROR_INVALID,
	GSPDF_DOCUMENT_ERROR_ENCRYPTED,
} GspdfDocumentErrorCode;

#define GSPDF_TYPE_DOCUMENT gspdf_document_get_type ()
G_DECLARE_DERIVABLE_TYPE (
	GspdfDocument,
	gspdf_document,
	GSPDF,
	DOCUMENT,
	GObject
)

struct _GspdfDocumentClass {
	GObjectClass parent_class;

	gboolean           (*save) (GspdfDocument *doc,
								              const gchar   *uri,
								              GError       **error);

	gboolean           (*linearized) (GspdfDocument *doc);

	gint               (*get_n_pages) (GspdfDocument *doc);

	GspdfDocumentPage *(*get_page) (GspdfDocument *doc,
									                gint           index);

	GspdfDocOutline   *(*get_outline) (GspdfDocument *doc);

	GspdfDocDest      *(*find_dest) (GspdfDocument *doc,
									                 const gchar   *named_dest);

	gboolean (*has_attachments) (GspdfDocument *doc);

	guint (*get_n_attachments) (GspdfDocument *doc);

	gpointer padding[12];
};

gboolean
gspdf_document_save (GspdfDocument *doc,
					           const gchar   *uri,
					           GError       **error);

gboolean
gspdf_document_linearized (GspdfDocument *doc);

gint
gspdf_document_get_n_pages (GspdfDocument *doc);

GspdfDocumentPage *
gspdf_document_get_page (GspdfDocument *doc,
					               gint           index);

GspdfDocOutline *
gspdf_document_get_outline (GspdfDocument *doc);

GspdfDocDest *
gspdf_document_find_dest (GspdfDocument *doc,
						              const gchar   *named_dest);

gboolean
gspdf_document_has_attachments (GspdfDocument *doc);

guint
gspdf_document_get_n_attachments (GspdfDocument *doc);



/**
 * GspdfDocOutline
 */

struct _GspdfDocOutline {
	GspdfDocAction  *action;
	GspdfDocOutline *child;
	GspdfDocOutline *next;
};

GspdfDocOutline *
gspdf_doc_outline_next (GspdfDocOutline *cur);

GspdfDocOutline *
gspdf_doc_outline_get_child (GspdfDocOutline *cur);

void
gspdf_doc_outline_free (GspdfDocOutline *outline);

G_END_DECLS


#endif
