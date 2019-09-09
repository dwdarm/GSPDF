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

#ifndef GSPDF_DOCUMENT_PAGE_H
#define GSPDF_DOCUMENT_PAGE_H

#ifndef __GLIB_GOBJECT_H__
#include <glib-object.h>
#endif

#ifndef GDK_PIXBUF_H
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif

G_BEGIN_DECLS

typedef struct {
	gdouble x;
	gdouble y;
	gdouble width;
	gdouble height;
} GspdfRectangle;

typedef enum {
	GSPDF_SELECTION_GLYPH,
	GSPDF_SELECTION_WORD,
	GSPDF_SELECTION_LINE
} GspdfSelectionStyle;

typedef enum {
	GSPDF_FIND_DEFAULT = 0,
	GSPDF_FIND_CASE_SENSITIVE = 1,
	GSPDF_FIND_BACKWARDS = 2,
	GSPDF_FIND_WHOLE_WORDS_ONLY = 4
} GspdfFindFlags;

/**
 * GspdfDocumentPage
 */

#define GSPDF_TYPE_DOCUMENT_PAGE gspdf_document_page_get_type ()
G_DECLARE_DERIVABLE_TYPE (
	GspdfDocumentPage,
	gspdf_document_page,
	GSPDF,
	DOCUMENT_PAGE,
	GObject
)

struct _GspdfDocumentPageClass {
	GObjectClass parent_class;

	gint (*get_index) (GspdfDocumentPage *doc_page);

	gchar *(*get_label) (GspdfDocumentPage *doc_page);

	gdouble (*get_width) (GspdfDocumentPage *doc_page);

	gdouble (*get_height) (GspdfDocumentPage *doc_page);

	GdkPixbuf *(*render) (GspdfDocumentPage *doc_page,
						            gdouble            sx,
						            gdouble            sy);

	GList *(*get_selected_region) (GspdfDocumentPage    *doc_page,
	                               GspdfSelectionStyle   style,
	                               const GspdfRectangle *selection);

	gchar *(*get_selected_text) (GspdfDocumentPage    *doc_page,
	                             GspdfSelectionStyle   style,
	                             const GspdfRectangle *selection);

	GList *(*get_text_mapping) (GspdfDocumentPage *doc_page);

	GList *(*get_link_mapping) (GspdfDocumentPage *doc_page);

	GList *(*find_text) (GspdfDocumentPage *doc_page,
						           const gchar       *text,
						           GspdfFindFlags     options);

	gpointer padding[12];
};

gint
gspdf_document_page_get_index (GspdfDocumentPage *doc_page);

gchar *
gspdf_document_page_get_label (GspdfDocumentPage *doc_page);

gdouble
gspdf_document_page_get_width (GspdfDocumentPage *doc_page);

gdouble
gspdf_document_page_get_height (GspdfDocumentPage *doc_page);

GdkPixbuf *
gspdf_document_page_render (GspdfDocumentPage *doc_page,
						                gdouble            sx,
						                gdouble            sy);

GList *
gspdf_document_page_get_selected_region (GspdfDocumentPage    *doc_page,
	                                       GspdfSelectionStyle   style,
	                                       const GspdfRectangle *selection);

gchar *
gspdf_document_page_get_selected_text (GspdfDocumentPage    *doc_page,
	                                     GspdfSelectionStyle   style,
	                                     const GspdfRectangle *selection);

GList *
gspdf_document_page_get_text_mapping (GspdfDocumentPage *doc_page);

GList *
gspdf_document_page_get_link_mapping (GspdfDocumentPage *doc_page);

GList *
gspdf_document_page_find_text (GspdfDocumentPage *doc_page,
						                   const gchar       *text,
						                   GspdfFindFlags     options);

/**
 * GspdfDocLinkMapping
 */

#ifndef GSPDF_DOC_ACTION_H
#include "gspdf-doc-action.h"
#endif

typedef struct {
	GspdfRectangle  area;
	GspdfDocAction *action;
} GspdfDocLinkMapping;

GspdfDocLinkMapping *
gspdf_doc_link_mapping_new (void);

GspdfDocLinkMapping *
gspdf_doc_link_mapping_copy (const GspdfDocLinkMapping *linkmapping);

void
gspdf_doc_link_mapping_free (GspdfDocLinkMapping *linkmapping);

G_END_DECLS

#endif
