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

#ifndef GSPDF_DOC_ACTION_H
#define GSPDF_DOC_ACTION_H

#ifndef __GLIB_H__
#include <glib.h>
#endif

typedef struct _GspdfDocDest             GspdfDocDest;
typedef void*                            GspdfDocAction;
typedef struct _GspdfDocActionAny        GspdfDocActionAny;
typedef struct _GspdfDocActionGotoDest   GspdfDocActionGotoDest;
typedef struct _GspdfDocActionGotoRemote GspdfDocActionGotoRemote;
typedef struct _GspdfDocActionLaunch     GspdfDocActionLaunch;
typedef struct _GspdfDocActionUri        GspdfDocActionUri;
typedef struct _GspdfDocActionNamed      GspdfDocActionNamed;

GspdfDocAction *gspdf_doc_action_copy (GspdfDocAction *action);
void            gspdf_doc_action_free (GspdfDocAction *action);
GspdfDocDest   *gspdf_doc_dest_copy (GspdfDocDest *dest);
void            gspdf_doc_dest_free (GspdfDocDest *dest);

typedef enum {
	GSPDF_DOC_DEST_UNKNOWN,
	GSPDF_DOC_DEST_XYZ,
	GSPDF_DOC_DEST_FIT,
	GSPDF_DOC_DEST_FITH,
	GSPDF_DOC_DEST_FITV,
	GSPDF_DOC_DEST_FITR,
	GSPDF_DOC_DEST_FITB,
	GSPDF_DOC_DEST_FITBH,
	GSPDF_DOC_DEST_FITBV,
	GSPDF_DOC_DEST_NAMED,
} GspdfDocDestType;

typedef enum {
	GSPDF_DOC_ACTION_UNKNOWN,
	GSPDF_DOC_ACTION_NONE,
	GSPDF_DOC_ACTION_GOTO_DEST,
	GSPDF_DOC_ACTION_GOTO_REMOTE,
	GSPDF_DOC_ACTION_LAUNCH,
	GSPDF_DOC_ACTION_URI,
	GSPDF_DOC_ACTION_NAMED,
} GspdfDocActionType;

struct _GspdfDocDest {
	GspdfDocDestType type;
	gint             index;
	gdouble          left;
	gdouble          bottom;
	gdouble          right;
	gdouble          top;
	gdouble          zoom;
	gchar           *named_dest;
	guint            change_left : 1;
	guint            change_top : 1;
	guint            change_zoom : 1;
};

struct _GspdfDocActionAny {
	GspdfDocActionType  type;
	gchar              *title;
};

struct _GspdfDocActionGotoDest {
	GspdfDocActionType  type;
	gchar              *title;

	GspdfDocDest       *dest;
};

struct _GspdfDocActionGotoRemote {
	GspdfDocActionType  type;
	gchar              *title;

	gchar              *file_name;
	GspdfDocDest       *dest;
};

struct _GspdfDocActionLaunch {
	GspdfDocActionType  type;
	gchar              *title;

	gchar              *file_name;
	gchar              *params;
};

struct _GspdfDocActionUri {
	GspdfDocActionType  type;
	gchar              *title;

	gchar              *uri;
};

struct _GspdfDocActionNamed {
	GspdfDocActionType  type;
	gchar              *title;

	gchar              *named_dest;
};

#endif
