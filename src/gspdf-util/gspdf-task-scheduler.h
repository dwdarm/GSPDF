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

#ifndef GSPDF_TASK_SCHEDULER_H
#define GSPDF_TASK_SCHEDULER_H

#ifndef GSPDF_TASK_H
#include "gspdf-task.h"
#endif

struct _GspdfTaskScheduler;
typedef struct _GspdfTaskScheduler GspdfTaskScheduler;

GspdfTaskScheduler *gspdf_task_scheduler_new (void);

void gspdf_task_scheduler_free (GspdfTaskScheduler *instance);

void gspdf_task_scheduler_push (GspdfTaskScheduler *instance,
	                              GspdfTask *task,
																gboolean urgent);

#endif
