/*
    FlyPDF - Create PDF files on the fly! 
    Copyright (C) 2006-2008 OS3 srl

    Written by Alessandro Molina
    Portions of code by Fabio Rotondo (fabio.rotondo@os3.it)
                        Gabriele Buscone (gabriele.buscone@os3.it)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License ONLY.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef FLYPDF_C_H_
#define FLYPDF_C_H_

typedef void *Pdf_p;

#undef DLL

#ifdef C_LIB
#define DLL		__declspec ( dllexport )
#else
#define DLL
#endif

void flypdf__out ( Pdf_p p, char const *s );

DLL Pdf_p flypdf_newPdf ( char const *orientation, char const *unit, char const *format );
#define flypdf_newDefaultPdf() flypdf_newPdf("P", "mm", "A4");
DLL void flypdf_destroyPdf ( Pdf_p p );

DLL Pdf_p flypdf_SetMargins ( Pdf_p p, float left, float top, float right );
DLL Pdf_p flypdf_SetLeftMargin ( Pdf_p p, float margin );
DLL Pdf_p flypdf_SetTopMargin ( Pdf_p p, float margin );
DLL Pdf_p flypdf_SetRightMargin ( Pdf_p p, float margin );
DLL Pdf_p flypdf_SetAutoPageBreak ( Pdf_p p, int autopb, float margin );
DLL Pdf_p flypdf_SetDisplayMode ( Pdf_p p, char const *zoom, char const *layout );
DLL Pdf_p flypdf_SetTitle ( Pdf_p p, char const *title );
DLL Pdf_p flypdf_SetSubject ( Pdf_p p, char const *subject );
DLL Pdf_p flypdf_SetAuthor ( Pdf_p p, char const *author );
DLL Pdf_p flypdf_SetKeywords ( Pdf_p p, char const *keywords );
DLL Pdf_p flypdf_SetCreator ( Pdf_p p, char const *creator );
DLL Pdf_p flypdf_SetAliasNbPages ( Pdf_p p, char const *alias );
DLL Pdf_p flypdf_AddPage ( Pdf_p p, char const *orientation );
DLL Pdf_p flypdf_Open ( Pdf_p p );
DLL Pdf_p flypdf_Close ( Pdf_p p );
DLL int flypdf_PageNo ( Pdf_p p );
DLL Pdf_p flypdf_Output ( Pdf_p p, char const *name );
DLL Pdf_p flypdf_Line ( Pdf_p p, double x1, double y1, double x2, double y2 );
DLL Pdf_p flypdf_SetLineWidth ( Pdf_p p, float width );
DLL Pdf_p flypdf_SetTextColor ( Pdf_p p, int r, int g, int b );
DLL Pdf_p flypdf_SetFillColor ( Pdf_p p, int r, int g, int b );
DLL Pdf_p flypdf_SetDrawColor ( Pdf_p p, int r, int g, int b );
DLL Pdf_p flypdf_Rect ( Pdf_p p, double x, double y, double w, double h, char const *style );
DLL Pdf_p flypdf_Text ( Pdf_p p, double x, double y, char const *txt );
DLL int flypdf_AcceptPageBreak ( Pdf_p p );
DLL Pdf_p flypdf_Ln ( Pdf_p p, int h );
DLL double flypdf_GetX ( Pdf_p p );
DLL Pdf_p flypdf_SetX ( Pdf_p p, double x );
DLL double flypdf_GetY ( Pdf_p p );
DLL Pdf_p flypdf_SetY ( Pdf_p p, double y );
DLL Pdf_p flypdf_SetXY ( Pdf_p p, double x, double y );
DLL Pdf_p flypdf_Cell ( Pdf_p p, double w, double h, char const *txt, char const *border, double ln, char const *align, int fill, char const *link );
DLL Pdf_p flypdf_SetFont ( Pdf_p p, char const *family, char const *style, double size );
DLL Pdf_p flypdf_AddTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset, char const *ofamily, char const *ostyle );
DLL Pdf_p flypdf_AddType1Font ( Pdf_p p, char const *afm_file, char const *font_file, char const *charset, char const *ofamily, char const * ostyle );
DLL Pdf_p flypdf_SaveReducedTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset, char ** dest_file_name );
DLL double flypdf_GetStringWidth ( Pdf_p p, char const *s );
DLL Pdf_p flypdf_Image ( Pdf_p p, char const *image, double x, double y, double w, double h, char const *type, char const *link );
DLL int flypdf_AddLink ( Pdf_p p );
DLL Pdf_p flypdf_SetLink ( Pdf_p p, int link, double y, int page );
DLL Pdf_p flypdf_ExternalLink ( Pdf_p p, double x, double y, double w, double h, char const *url );
DLL Pdf_p flypdf_InternalLink ( Pdf_p p, double x, double y, double w, double h, int linkid );
DLL Pdf_p flypdf_AddTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset, char const *ofamily, char const *ostyle );
DLL Pdf_p flypdf_AddType1Font ( Pdf_p p, char const *afm_file, char const *font_file, char const *charset, char const *ofamily, char const *ostyle );
DLL Pdf_p flypdf_TextWithDirection ( Pdf_p p, double x, double y, char const *xt, char const *direction );
DLL Pdf_p flypdf_TextWithRotation ( Pdf_p p, double x, double y, char const *text, double txt_angle, double font_angle );
DLL Pdf_p flypdf_RoundedRect ( Pdf_p p, double x, double y, double w, double h, double r, char const *style, char const *angle );
DLL Pdf_p flypdf_Circle ( Pdf_p p, double x, double y, double r, char const *style );
DLL Pdf_p flypdf_Ellipse ( Pdf_p p, double x, double y, double rx, double ry, char const *style );

DLL Pdf_p flypdf_MultiCell ( Pdf_p p, double w, double h, char const *txt, char const *border, char const *align, int fill);
DLL Pdf_p flypdf_Write ( Pdf_p p, double h, char const *txt, char const *link);
DLL Pdf_p flypdf_SetFontSize ( Pdf_p p, double s);

DLL Pdf_p flypdf_SetHeaderDelegate( Pdf_p p, void(*callback)(Pdf_p) );
DLL Pdf_p flypdf_SetFooterDelegate( Pdf_p p, void(*callback)(Pdf_p) );
DLL	Pdf_p flypdf_SetAcceptPageBreakDelegate(Pdf_p, int(*callback)(Pdf_p) );

#endif
