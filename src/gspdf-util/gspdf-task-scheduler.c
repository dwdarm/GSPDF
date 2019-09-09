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

 #include "gspdf-task-scheduler.h"

 struct _GspdfTaskScheduler {
 	GThread     *thread;
 	GAsyncQueue *queue;
 };

 static gpointer
 _gspdf_task_scheduler_func (gpointer data)
 {
 	GAsyncQueue *queue = (GAsyncQueue*) data;
 	GspdfTask *task = NULL;

 	while (1) {
 		task = (GspdfTask*) g_async_queue_pop (queue);
 		if (gspdf_task_run (task)) {
 			g_async_queue_push (queue, task);
 		} else {
 			g_object_unref (task);
 		}
 	}

 	return NULL;
 }

 GspdfTaskScheduler *
 gspdf_task_scheduler_new ()
 {
 	GAsyncQueue *queue = g_async_queue_new ();
 	GThread *thread = g_thread_new (NULL, _gspdf_task_scheduler_func, queue);
 	GspdfTaskScheduler *ret = NULL;
 	ret = g_malloc0 (sizeof (GspdfTaskScheduler));
 	ret->thread = thread;
 	ret->queue = queue;

 	return ret;
 }

 void
 gspdf_task_scheduler_free (GspdfTaskScheduler *instance)
 {
 	g_return_if_fail (instance != NULL);
 }

 void
 gspdf_task_scheduler_push (GspdfTaskScheduler *instance,
 	                         GspdfTask *task,
 													 gboolean urgent)
 {
 	g_return_if_fail (instance != NULL);
 	g_return_if_fail (task != NULL);
 	g_return_if_fail (GSPDF_IS_TASK (task));

 	g_object_ref (task);

 	if (urgent) {
 		g_async_queue_push_front (instance->queue, task);
 	} else {
 		g_async_queue_push (instance->queue, task);
 	}
 }
