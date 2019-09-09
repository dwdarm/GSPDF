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

#ifndef GSPDF_PDF_DOCUMENT_H
#define GSPDF_PDF_DOCUMENT_H

#ifndef GSPDF_DOCUMENT_H
#include "gspdf-document.h"
#endif

G_BEGIN_DECLS

#define GSPDF_TYPE_PDF_DOCUMENT gspdf_pdf_document_get_type ()
G_DECLARE_FINAL_TYPE (
	GspdfPdfDocument,
	gspdf_pdf_document,
	GSPDF,
	PDF_DOCUMENT,
	GspdfDocument
)

GspdfDocument *gspdf_pdf_document_new (const gchar  *uri,
									                     const gchar  *password,
							                         GError      **error);

G_END_DECLS

#endif
