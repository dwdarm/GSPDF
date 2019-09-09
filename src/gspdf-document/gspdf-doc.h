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

#ifndef GSPDF_DOC_H
#define GSPDF_DOC_H

#ifndef GSPDF_DOCUMENT_H
#include "gspdf-document.h"
#endif

#ifndef GSPDF_PDF_DOCUMENT_H
#include "gspdf-pdf-document.h"
#endif

GspdfDocument *
gspdf_document_new_from_file (const gchar *uri,
	                            const gchar *password,
							                GError      **error);

#endif
