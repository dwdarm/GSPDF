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

#include "gspdf-task.h"

typedef struct {
	GMutex              mutex;
	GspdfTaskStatus     status;
	gboolean            cancel;

	gspdf_task_callback finished_cb;
	gpointer            finished_cb_data;
} GspdfTaskPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GspdfTask, gspdf_task, G_TYPE_OBJECT)

struct _GspdfTaskScheduler {
 GThread     *thread;
 GAsyncQueue *queue;
};

static gboolean gspdf_task_run (GspdfTask *task);
static gboolean gspdf_task_get_cancel (GspdfTask *task);
static void gspdf_task_set_cancel (GspdfTask *task, gboolean cancel);
static void gspdf_task_set_status (GspdfTask *task, GspdfTaskStatus status);

static void
gspdf_task_dispose (GObject *object)
{

	G_OBJECT_CLASS (gspdf_task_parent_class)->dispose (object);
}

static void
gspdf_task_finalize (GObject *object)
{
	G_OBJECT_CLASS (gspdf_task_parent_class)->finalize (object);
}

static void
gspdf_task_init (GspdfTask *task)
{
	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	priv->status = GSPDF_TASK_STATUS_IDLE;
	priv->cancel = FALSE;
	priv->finished_cb = NULL;
	priv->finished_cb_data = NULL;
	g_mutex_init (&priv->mutex);
}

static void
gspdf_task_class_init (GspdfTaskClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gspdf_task_dispose;
	object_class->finalize = gspdf_task_finalize;
}

static void
gspdf_task_set_status (GspdfTask *task, GspdfTaskStatus status)
{
	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	g_mutex_lock (&priv->mutex);
	priv->status = status;
	g_mutex_unlock (&priv->mutex);
}

static gboolean
gspdf_task_run (GspdfTask *task)
{
	g_return_val_if_fail (GSPDF_IS_TASK (task), FALSE);

	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);
	gboolean ret = FALSE;

	if (gspdf_task_get_cancel (task)) {
		gspdf_task_set_status (task, GSPDF_TASK_STATUS_STOPPED);
		gspdf_task_set_cancel (task, FALSE);
		return ret;
	}

	gspdf_task_set_status (task, GSPDF_TASK_STATUS_RUNNING);
	ret = GSPDF_TASK_GET_CLASS (task)->run (task);

	if (gspdf_task_get_cancel (task)) {
		gspdf_task_set_status (task, GSPDF_TASK_STATUS_STOPPED);
		gspdf_task_set_cancel (task, FALSE);
		return ret;
	}

	gspdf_task_set_status (task, GSPDF_TASK_STATUS_OK);

	if (priv->finished_cb) {
		priv->finished_cb (task, priv->finished_cb_data);
	}

	return ret;
}

static gboolean
gspdf_task_get_cancel (GspdfTask *task)
{
	g_return_val_if_fail (GSPDF_IS_TASK (task), FALSE);

	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	g_mutex_lock (&priv->mutex);
	gboolean ret = priv->cancel;
	g_mutex_unlock (&priv->mutex);

	return ret;
}

static void
gspdf_task_set_cancel (GspdfTask *task, gboolean cancel)
{
	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	g_mutex_lock (&priv->mutex);
	priv->cancel = cancel;
	g_mutex_unlock (&priv->mutex);
}

void
gspdf_task_cancel (GspdfTask *task)
{
	g_return_if_fail (GSPDF_IS_TASK (task));

	gspdf_task_set_cancel (task, TRUE);
}

GspdfTaskStatus
gspdf_task_get_status (GspdfTask *task)
{
	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	g_mutex_lock (&priv->mutex);
	GspdfTaskStatus ret = priv->status;
	g_mutex_unlock (&priv->mutex);

	return ret;
}

void
gspdf_task_set_finished_callback (GspdfTask *task,
	                                gspdf_task_callback callback,
																	gpointer user_data)
{
	g_return_if_fail (task != NULL);
	g_return_if_fail (GSPDF_IS_TASK (task));

	GspdfTaskPrivate *priv = gspdf_task_get_instance_private (task);

	priv->finished_cb = callback;
	priv->finished_cb_data = user_data;
}

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
													 GspdfTask          *task,
													 gboolean            urgent)
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
