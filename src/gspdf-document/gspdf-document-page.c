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

#include "gspdf-document-page.h"

typedef struct {
	gpointer *handler;
} GspdfDocumentPagePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GspdfDocumentPage, gspdf_document_page, G_TYPE_OBJECT)

enum {
	PROP_HANDLER = 1,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_document_page_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GspdfDocumentPagePrivate *priv = gspdf_document_page_get_instance_private (
		GSPDF_DOCUMENT_PAGE (object)
	);

	switch (property_id) {
		case PROP_HANDLER:
			priv->handler = g_value_get_pointer (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_document_page_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GspdfDocumentPagePrivate *priv = gspdf_document_page_get_instance_private (
		GSPDF_DOCUMENT_PAGE (object)
	);

	switch (property_id) {
		case PROP_HANDLER:
			g_value_set_pointer (value, priv->handler);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_document_page_dispose (GObject *object)
{
	 G_OBJECT_CLASS (gspdf_document_page_parent_class)->dispose (object);
}

static void
gspdf_document_page_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_document_page_parent_class)->finalize (object);
}

static void
gspdf_document_page_init (GspdfDocumentPage *self)
{
	//GspdfDocumentPagePrivate *priv = gspdf_document_page_get_instance_private (self);
}

static void
gspdf_document_page_class_init (GspdfDocumentPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gspdf_document_page_dispose;
	object_class->finalize = gspdf_document_page_finalize;
	object_class->set_property = gspdf_document_page_set_property;
	object_class->get_property = gspdf_document_page_get_property;

	obj_properties[PROP_HANDLER] = g_param_spec_pointer (
		"handler",
		"Handler",
		"",
		G_PARAM_READWRITE
	);

	 g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

gint
gspdf_document_page_get_index (GspdfDocumentPage *doc_page)
{
	g_return_val_if_fail (doc_page != NULL, -1);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), -1);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_index != NULL, -1);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_index (doc_page);
}

gchar *
gspdf_document_page_get_label (GspdfDocumentPage *doc_page)
{
	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_label != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_label (doc_page);
}

gdouble
gspdf_document_page_get_width (GspdfDocumentPage *doc_page)
{
	g_return_val_if_fail (doc_page != NULL, -1);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), -1);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_width != NULL, -1);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_width (doc_page);
}

gdouble
gspdf_document_page_get_height (GspdfDocumentPage *doc_page)
{
	g_return_val_if_fail (doc_page != NULL, -1);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), -1);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_height != NULL, -1);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_height (doc_page);
}

GdkPixbuf *
gspdf_document_page_render (GspdfDocumentPage *doc_page, gdouble sx, gdouble sy)
{

	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->render != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->render (doc_page, sx, sy);
}

GList *
gspdf_document_page_get_selected_region (GspdfDocumentPage    *doc_page,
	                                       GspdfSelectionStyle   style,
	                                       const GspdfRectangle *selection)
{
	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_selected_region != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_selected_region (doc_page, style, selection);
}

gchar *
gspdf_document_page_get_selected_text (GspdfDocumentPage    *doc_page,
	                                     GspdfSelectionStyle   style,
	                                     const GspdfRectangle *selection)
{
	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_selected_text != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_selected_text (doc_page, style, selection);
}

GList *
gspdf_document_page_get_link_mapping (GspdfDocumentPage *doc_page)
{
	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_link_mapping != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->get_link_mapping (doc_page);
}

GList *
gspdf_document_page_find_text (GspdfDocumentPage *doc_page,
						                   const gchar       *text,
						                   GspdfFindFlags     options)
{
	g_return_val_if_fail (doc_page != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT_PAGE (doc_page), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->find_text != NULL, NULL);

	return GSPDF_DOCUMENT_PAGE_GET_CLASS (doc_page)->find_text (doc_page, text, options);
}

/**
 * GspdfDocLinkMapping
 */

GspdfDocLinkMapping *
gspdf_doc_link_mapping_new ()
{
	return g_malloc0 (sizeof (GspdfDocLinkMapping));
}

GspdfDocLinkMapping *
gspdf_doc_link_mapping_copy (const GspdfDocLinkMapping *linkmapping)
{
	return (GspdfDocLinkMapping *) g_memdup (linkmapping, sizeof (GspdfDocLinkMapping));
}

void
gspdf_doc_link_mapping_free (GspdfDocLinkMapping *linkmapping)
{
	if (linkmapping->action) {
		gspdf_doc_action_free (linkmapping->action);
	}

	g_free (linkmapping);
}
