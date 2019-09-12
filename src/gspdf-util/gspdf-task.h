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

#ifndef GSPDF_TASK_H
#define GSPDF_TASK_H

#ifndef __GLIB_GOBJECT_H__
#include <glib-object.h>
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_TASK gspdf_task_get_type ()
G_DECLARE_DERIVABLE_TYPE (GspdfTask, gspdf_task, GSPDF, TASK, GObject)

struct _GspdfTaskClass {
	GObjectClass parent_class;

	gboolean (*run) (GspdfTask *task);

	gpointer padding[12];
};

typedef enum {
	GSPDF_TASK_STATUS_IDLE,
	GSPDF_TASK_STATUS_RUNNING,
	GSPDF_TASK_STATUS_OK,
	GSPDF_TASK_STATUS_FAILED,
	GSPDF_TASK_STATUS_STOPPED
} GspdfTaskStatus;

struct _GspdfTaskScheduler;
typedef struct _GspdfTaskScheduler GspdfTaskScheduler;

typedef void (*gspdf_task_callback) (GspdfTask *task, gpointer user_data);

void gspdf_task_cancel (GspdfTask *task);

GspdfTaskStatus gspdf_task_get_status (GspdfTask *task);

void gspdf_task_set_finished_callback (
	GspdfTask *task, gspdf_task_callback callback, gpointer user_data);

GspdfTaskScheduler *gspdf_task_scheduler_new (void);

void gspdf_task_scheduler_free (GspdfTaskScheduler *instance);

void gspdf_task_scheduler_push (
	GspdfTaskScheduler *instance, GspdfTask *task, gboolean urgent);

G_END_DECLS

#endif
