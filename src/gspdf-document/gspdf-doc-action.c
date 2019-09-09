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

#include "gspdf-doc-action.h"

GspdfDocAction *
gspdf_doc_action_copy (GspdfDocAction *action)
{
	GspdfDocAction *ret = NULL;
	GspdfDocActionAny *any = (GspdfDocActionAny*) action;

	if (any->type == GSPDF_DOC_ACTION_UNKNOWN) {
		GspdfDocActionAny *gact = g_malloc0 (sizeof (GspdfDocActionAny));
		gact->type = GSPDF_DOC_ACTION_UNKNOWN;
		gact->title = g_strdup (any->title);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_NONE) {
		GspdfDocActionAny *gact = g_malloc0 (sizeof (GspdfDocActionAny));
		gact->type = GSPDF_DOC_ACTION_NONE;
		gact->title = g_strdup (any->title);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_GOTO_DEST) {
		GspdfDocActionGotoDest *act = (GspdfDocActionGotoDest*) any;
		GspdfDocActionGotoDest *gact = g_malloc0 (sizeof (GspdfDocActionGotoDest));
		gact->type = GSPDF_DOC_ACTION_GOTO_DEST;
		gact->title = g_strdup (act->title);
		gact->dest = gspdf_doc_dest_copy (act->dest);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_GOTO_REMOTE) {
		GspdfDocActionGotoRemote *act = (GspdfDocActionGotoRemote*) any;
		GspdfDocActionGotoRemote *gact = g_malloc0 (sizeof (GspdfDocActionGotoRemote));
		gact->type = GSPDF_DOC_ACTION_GOTO_REMOTE;
		gact->title = g_strdup (act->title);
		gact->file_name = g_strdup (act->file_name);
		gact->dest = gspdf_doc_dest_copy (act->dest);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_LAUNCH) {
		GspdfDocActionLaunch *act = (GspdfDocActionLaunch*) any;
		GspdfDocActionLaunch *gact = g_malloc0 (sizeof (GspdfDocActionLaunch));
		gact->type = GSPDF_DOC_ACTION_LAUNCH;
		gact->title = g_strdup (act->title);
		gact->file_name = g_strdup (act->file_name);
		gact->params = g_strdup (act->params);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_URI) {
		GspdfDocActionUri *act = (GspdfDocActionUri*) any;
		GspdfDocActionUri *gact = g_malloc0 (sizeof (GspdfDocActionUri));
		gact->type = GSPDF_DOC_ACTION_URI;
		gact->title = g_strdup (act->title);
		gact->uri = g_strdup (act->uri);
		ret = (GspdfDocAction*) gact;
	}
	else if (any->type == GSPDF_DOC_ACTION_NAMED) {
		GspdfDocActionNamed *act = (GspdfDocActionNamed*) any;
		GspdfDocActionNamed *gact = g_malloc0 (sizeof (GspdfDocActionNamed));
		gact->type = GSPDF_DOC_ACTION_NAMED;
		gact->title = g_strdup (act->title);
		gact->named_dest = g_strdup (act->named_dest);
		ret = (GspdfDocAction*) gact;
	}

	return ret;
}

void
gspdf_doc_action_free (GspdfDocAction *action)
{
	GspdfDocActionAny *any = (GspdfDocActionAny*) action;

	if (any->type == GSPDF_DOC_ACTION_UNKNOWN) {
		g_free (any->title);
	}
	else if (any->type == GSPDF_DOC_ACTION_NONE) {
		g_free (any->title);
	}
	else if (any->type == GSPDF_DOC_ACTION_GOTO_DEST) {
		GspdfDocActionGotoDest *act = (GspdfDocActionGotoDest*) any;
		g_free (act->title);
		gspdf_doc_dest_free (act->dest);
	}
	else if (any->type == GSPDF_DOC_ACTION_GOTO_REMOTE) {
		GspdfDocActionGotoRemote *act = (GspdfDocActionGotoRemote*) any;
		g_free (act->title);
		g_free (act->file_name);
		gspdf_doc_dest_free (act->dest);
	}
	else if (any->type == GSPDF_DOC_ACTION_LAUNCH) {
		GspdfDocActionLaunch *act = (GspdfDocActionLaunch*) any;
		g_free (act->title);
		g_free (act->file_name);
		g_free (act->params);
	}
	else if (any->type == GSPDF_DOC_ACTION_URI) {
		GspdfDocActionUri *act = (GspdfDocActionUri*) any;
		g_free (act->title);
		g_free (act->uri);
	}
	else if (any->type == GSPDF_DOC_ACTION_NAMED) {
		GspdfDocActionNamed *act = (GspdfDocActionNamed*) any;
		g_free (act->title);
		g_free (act->named_dest);
	}

	g_free (action);
}

GspdfDocDest   *
gspdf_doc_dest_copy (GspdfDocDest *dest)
{
	GspdfDocDest *ret = g_memdup (dest, sizeof (GspdfDocDest));
	ret->named_dest = g_strdup (dest->named_dest);

	return ret;
}

void
gspdf_doc_dest_free (GspdfDocDest *dest)
{
	g_free (dest->named_dest);
	g_free (dest);
}
