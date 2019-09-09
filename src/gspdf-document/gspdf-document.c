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

#include "gspdf-document.h"
#include <math.h>

typedef struct {
	gpointer *handler;
	gchar*    author;
	gint      creation_date;
	gchar*    creator;
	gchar*    keywords;
	gint      mod_date;
	gchar*    producer;
	gchar*    subject;
	gchar*    title;
	gint      page_layout;
	gint      page_mode;
} GspdfDocumentPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GspdfDocument, gspdf_document, G_TYPE_OBJECT)

enum {
	PROP_HANDLER = 1,
	PROP_AUTHOR,
	PROP_CREATION_DATE,
	PROP_CREATOR,
	PROP_KEYWORDS,
	PROP_MOD_DATE,
	PROP_PRODUCER,
	PROP_SUBJECT,
	PROP_TITLE,
	PROP_PAGE_LAYOUT,
	PROP_PAGE_MODE,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0};

static void
gspdf_document_set_property (GObject *object,
	                           guint property_id,
														 const GValue *value,
														 GParamSpec *pspec)
{
	GspdfDocumentPrivate *priv = gspdf_document_get_instance_private (
		GSPDF_DOCUMENT (object)
	);

	switch (property_id) {
		case PROP_HANDLER:
			priv->handler = g_value_get_pointer (value);
			break;
		case PROP_AUTHOR:
			if (priv->author) {
				g_free (priv->author);
				priv->author = NULL;
			}
			priv->author = g_value_dup_string (value);
			break;
		case PROP_CREATION_DATE:
			priv->creation_date = g_value_get_int (value);
			break;
		case PROP_CREATOR:
			if (priv->creator) {
				g_free (priv->creator);
				priv->creator = NULL;
			}
			priv->creator = g_value_dup_string (value);
			break;
		case PROP_KEYWORDS:
			if (priv->keywords) {
				g_free (priv->keywords);
				priv->keywords = NULL;
			}
			priv->keywords = g_value_dup_string (value);
			break;
		case PROP_MOD_DATE:
			priv->mod_date = g_value_get_int (value);
			break;
		case PROP_PRODUCER:
			if (priv->producer) {
				g_free (priv->producer);
				priv->producer = NULL;
			}
			priv->producer = g_value_dup_string (value);
			break;
		case PROP_SUBJECT:
			if (priv->subject) {
				g_free (priv->subject);
				priv->subject = NULL;
			}
			priv->subject = g_value_dup_string (value);
			break;
		case PROP_TITLE:
			if (priv->title) {
				g_free (priv->title);
				priv->title = NULL;
			}
			priv->title = g_value_dup_string (value);
			break;
		case PROP_PAGE_LAYOUT:
			priv->page_layout = g_value_get_int (value);
			break;
		case PROP_PAGE_MODE:
			priv->page_mode = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;

	}
}

static void
gspdf_document_get_property (GObject *object,
	                           guint property_id,
														 GValue *value,
														 GParamSpec *pspec)
{
	GspdfDocumentPrivate *priv = gspdf_document_get_instance_private (
		GSPDF_DOCUMENT (object)
	);

	switch (property_id) {
		case PROP_HANDLER:
			g_value_set_pointer (value, priv->handler);
			break;
		case PROP_AUTHOR:
			g_value_set_string (value, priv->author);
			break;
		case PROP_CREATION_DATE:
			g_value_set_int (value, priv->creation_date);
			break;
		case PROP_CREATOR:
			g_value_set_string (value, priv->creator);
			break;
		case PROP_KEYWORDS:
			g_value_set_string (value, priv->keywords);
			break;
		case PROP_MOD_DATE:
			g_value_set_int (value, priv->mod_date);
			break;
		case PROP_PRODUCER:
			g_value_set_string (value, priv->producer);
			break;
		case PROP_SUBJECT:
			g_value_set_string (value, priv->subject);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_PAGE_LAYOUT:
			g_value_set_int (value, priv->page_layout);
			break;
		case PROP_PAGE_MODE:
			g_value_set_int (value, priv->page_mode);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gspdf_document_dispose (GObject *object)
{
	GspdfDocumentPrivate *priv = gspdf_document_get_instance_private (
		GSPDF_DOCUMENT (object)
	);

	if (priv->author != NULL) {
		g_free (priv->author);
		priv->author = NULL;
	}

	if (priv->creator != NULL) {
		g_free (priv->creator);
		priv->creator = NULL;
	}

	if (priv->keywords != NULL) {
		g_free (priv->keywords);
		priv->keywords = NULL;
	}

	if (priv->producer != NULL) {
		g_free (priv->producer);
		priv->producer = NULL;
	}

	if (priv->subject != NULL) {
		g_free (priv->subject);
		priv->subject = NULL;
	}

	if (priv->title != NULL) {
		g_free (priv->title);
		priv->title = NULL;
	}

	 G_OBJECT_CLASS (gspdf_document_parent_class)->dispose (object);
}

static void
gspdf_document_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_document_parent_class)->finalize (object);
}

static void
gspdf_document_init (GspdfDocument *self)
{
	//GspdfDocumentPrivate *priv = gspdf_document_get_instance_private (self);
}

static void
gspdf_document_class_init (GspdfDocumentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gspdf_document_dispose;
	object_class->finalize = gspdf_document_finalize;
	object_class->set_property = gspdf_document_set_property;
	object_class->get_property = gspdf_document_get_property;

	obj_properties[PROP_HANDLER] = g_param_spec_pointer (
		"handler",
		"Handler",
		"",
		G_PARAM_READWRITE
	);

	obj_properties[PROP_AUTHOR] = g_param_spec_string (
		"author",
		"Author",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_CREATION_DATE] = g_param_spec_int (
		"creation-date",
		"Creation-date",
		"",
		-1,
		(gint) pow (2, sizeof (gint) * 8),
		-1,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_CREATOR] = g_param_spec_string (
		"creator",
		"Creator",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_KEYWORDS] = g_param_spec_string (
		"keywords",
		"Keywords",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_MOD_DATE] = g_param_spec_int (
		"mod-date",
		"Mod-date",
		"",
		-1,
		(gint) pow (2, sizeof (gint) * 8),
		-1,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_PRODUCER] = g_param_spec_string (
		"producer",
		"Producer",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_SUBJECT] = g_param_spec_string (
		"subject",
		"Subject",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_TITLE] = g_param_spec_string (
		"title",
		"Title",
		"",
		NULL,
		G_PARAM_READWRITE
	);

	obj_properties[PROP_PAGE_LAYOUT] = g_param_spec_int (
		"page-layout",
		"Page-layout",
		"",
		(gint) GSPDF_DOCUMENT_PAGE_LAYOUT_NO_LAYOUT,
		(gint) GSPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT,
		(gint) GSPDF_DOCUMENT_PAGE_LAYOUT_NO_LAYOUT,
	    G_PARAM_READWRITE
	);

	obj_properties[PROP_PAGE_MODE] = g_param_spec_int (
		"page-mode",
		"Page-mode",
		"",
		(gint) GSPDF_DOCUMENT_PAGE_MODE_NONE,
		(gint) GSPDF_DOCUMENT_PAGE_MODE_USE_ATTACHMENTS,
		(gint) GSPDF_DOCUMENT_PAGE_MODE_NONE,
		G_PARAM_READWRITE
	);

	g_object_class_install_properties (object_class, N_PROPERTIES, obj_properties);
}

gboolean
gspdf_document_save (GspdfDocument *doc,
					           const gchar   *uri,
					           GError       **error)
{
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc),FALSE);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->save != NULL, FALSE);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->save (doc, uri, error);
}

gboolean
gspdf_document_linearized (GspdfDocument *doc)
{
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), FALSE);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->linearized != NULL, FALSE);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->linearized (doc);
}

gint
gspdf_document_get_n_pages (GspdfDocument *doc)
{
	g_return_val_if_fail (doc != NULL, -1);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), -1);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->get_n_pages != NULL, -1);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->get_n_pages (doc);
}

GspdfDocumentPage *
gspdf_document_get_page (GspdfDocument *doc, gint index)
{
	g_return_val_if_fail (doc != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->get_page != NULL, NULL);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->get_page (doc, index);
}

GspdfDocOutline *
gspdf_document_get_outline (GspdfDocument *doc)
{
	g_return_val_if_fail (doc != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->get_outline != NULL, NULL);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->get_outline (doc);
}

GspdfDocDest *
gspdf_document_find_dest (GspdfDocument *doc,
						              const gchar   *named_dest)
{
	g_return_val_if_fail (doc != NULL, NULL);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), NULL);
	g_return_val_if_fail (named_dest != NULL, NULL);
	g_return_val_if_fail (GSPDF_DOCUMENT_GET_CLASS (doc)->find_dest != NULL, NULL);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->find_dest (doc, named_dest);
}

gboolean
gspdf_document_has_attachments (GspdfDocument *doc)
{
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), FALSE);
	g_return_val_if_fail (
		GSPDF_DOCUMENT_GET_CLASS (doc)->has_attachments != NULL,
		FALSE
	);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->has_attachments (doc);
}

guint
gspdf_document_get_n_attachments (GspdfDocument *doc)
{
	g_return_val_if_fail (doc != NULL, 0);
	g_return_val_if_fail (GSPDF_IS_DOCUMENT (doc), 0);
	g_return_val_if_fail (
		GSPDF_DOCUMENT_GET_CLASS (doc)->get_n_attachments != NULL,
		0
	);

	return GSPDF_DOCUMENT_GET_CLASS (doc)->get_n_attachments (doc);
}

/**
 * GspdfDocOutline
 */

GspdfDocOutline *
gspdf_doc_outline_next (GspdfDocOutline *cur)
{
	return cur->next;
}

GspdfDocOutline *
gspdf_doc_outline_get_child (GspdfDocOutline *cur)
{
	return cur->child;
}

void
gspdf_doc_outline_free (GspdfDocOutline *outline)
{
	GspdfDocOutline *iter = outline;
	GspdfDocOutline *temp = NULL;

	while (iter) {
		if (iter->child) {
			gspdf_doc_outline_free (iter->child);
		}

		gspdf_doc_action_free (iter->action);
		temp = iter->next;
		g_free (iter);
		iter = temp;
	}
}
