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

#ifndef GSPDF_PAGE_CACHE_H
#define GSPDF_PAGE_CACHE_H

#ifndef __GLIB_GOBJECT_H__
#include <glib-object.h>
#endif

#ifndef GSPDF_DOC_H
#include "gspdf-document/gspdf-doc.h"
#endif

#ifndef GSPDF_TASK_LIST_H
#include "gspdf-task-list.h"
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_PAGE_CACHE gspdf_page_cache_get_type ()
G_DECLARE_FINAL_TYPE (
	GspdfPageCache,
	gspdf_page_cache,
	GSPDF,
	PAGE_CACHE,
	GObject
)

GspdfPageCache *
gspdf_page_cache_new (void);

void
gspdf_page_cache_open_document (GspdfPageCache *page_cache,
								                const gchar    *uri,
								                const gchar    *password);

gchar *
gspdf_page_cache_get_uri (GspdfPageCache *page_cache);

GspdfDocument *
gspdf_page_cache_get_document (GspdfPageCache  *page_cache,
                               GError         **error);

GPtrArray *
gspdf_page_cache_get_document_map (GspdfPageCache *page_cache);

void
gspdf_page_cache_set_range (GspdfPageCache *page_cache,
							              gint 			      start,
							              gint 			      end,
							              gdouble         scale);

void
gspdf_page_cache_get_range (GspdfPageCache *page_cache,
							              gint 		       *start,
							              gint 		       *end);

void
gspdf_page_cache_set_scale (GspdfPageCache *page_cache,
							              gdouble 	      scale);

GdkPixbuf *
gspdf_page_cache_get_pixbuf (GspdfPageCache *page_cache,
							               gint 			     index);

GList *
gspdf_page_cache_get_selected_region (GspdfPageCache       *page_cache,
									                    gint                  index,
	                                    GspdfSelectionStyle   style,
	                                    const GspdfRectangle *selection);

gchar *
gspdf_page_cache_get_selected_text (GspdfPageCache       *page_cache,
									                  gint                  index,
	                                  GspdfSelectionStyle   style,
	                                  const GspdfRectangle *selection);

GList *
gspdf_page_cache_get_text_mapping (GspdfPageCache *page_cache,
							                     gint 		       index);

void
gspdf_page_cache_clear (GspdfPageCache *page_cache);


G_END_DECLS

#endif
