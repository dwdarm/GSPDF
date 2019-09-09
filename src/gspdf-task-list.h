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

#ifndef GSPDF_TASK_LIST_H
#define GSPDF_TASK_LIST_H

#ifndef GSPDF_TASK_H
#include "gspdf-util/gspdf-task.h"
#endif

#ifndef GSPDF_DOC_H
#include "gspdf-document/gspdf-doc.h"
#endif

G_BEGIN_DECLS

typedef struct {
	gdouble width;
	gdouble height;
} GspdfDocMap;

/**
 * GspdfTaskLoader
 */

#define GSPDF_TYPE_TASK_LOADER gspdf_task_loader_get_type ()
G_DECLARE_FINAL_TYPE (
	GspdfTaskLoader,
	gspdf_task_loader,
	GSPDF,
	TASK_LOADER,
	GspdfTask
)

GspdfTask *
gspdf_task_loader_new (void);

void
gspdf_task_loader_set (GspdfTaskLoader *task,
	                     const gchar     *uri,
	                     const gchar     *password);

void
gspdf_task_loader_set_uri (GspdfTaskLoader *task,
	                         const gchar     *uri);

gchar *
gspdf_task_loader_get_uri (GspdfTaskLoader *task);

void
gspdf_task_loader_set_password (GspdfTaskLoader *task,
	                              const gchar     *password);

gchar *
gspdf_task_loader_get_password (GspdfTaskLoader *task);

GspdfDocument *
gspdf_task_loader_get_document (GspdfTaskLoader *task);

GPtrArray *
gspdf_task_loader_get_document_map (GspdfTaskLoader *task);

GError *
gspdf_task_loader_get_gerror (GspdfTaskLoader *task);

/**
 * GspdfTaskRender
 */

#define GSPDF_TYPE_TASK_RENDER gspdf_task_render_get_type ()
G_DECLARE_FINAL_TYPE (
	GspdfTaskRender,
	gspdf_task_render,
	GSPDF,
	TASK_RENDER,
	GspdfTask
)

GspdfTask *
gspdf_task_render_new (void);

void
gspdf_task_render_set (GspdfTaskRender *task,
	                     GspdfDocument *doc,
											 gint index,
											 gdouble scale);

gint
gspdf_task_render_get_index (GspdfTaskRender *task);

GdkPixbuf *
gspdf_task_render_get_pixbuf (GspdfTaskRender *task);

GList *
gspdf_task_render_get_text_mapping (GspdfTaskRender *task);


G_END_DECLS

#endif
