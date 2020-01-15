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
#include "flypdf.h"

class PDF_CWrapper : public Pdf
{
	public:
		typedef void(*PdfCallback)();
		typedef int(*PBreakCallback)();

		PdfCallback header_callback;
		PdfCallback footer_callback;
		PBreakCallback pagebreak_callback;

		PDF_CWrapper(char const *orientation, char const *unit, char const *format)
		: Pdf(orientation, unit, format), header_callback(0), footer_callback(0), pagebreak_callback(0) {}
		virtual ~PDF_CWrapper() {}

		virtual void Header()
		{
			if (header_callback)
				header_callback();
			else
				Pdf::Header();
		}

		virtual void Footer()
		{
			if (footer_callback)
				footer_callback();
			else
				Pdf::Footer();
		}

		virtual int AcceptPageBreak()
		{
			if (pagebreak_callback)
				return pagebreak_callback();
			else
				return Pdf::AcceptPageBreak();
		}
};

extern "C"
{

#include "flypdf_c.h"

	Pdf_p flypdf_newPdf ( char const *orientation, char const *unit, char const *format )
	{
		return new PDF_CWrapper ( orientation, unit, format );
	}

	void flypdf_destroyPdf ( Pdf_p p )
	{
		delete ( PDF_CWrapper * ) p;
	}

	Pdf_p flypdf_SetMargins ( Pdf_p p, float left, float top, float right )
	{
		( ( Pdf * ) p )->SetMargins ( left, top, right );
		return p;
	}

	Pdf_p flypdf_SetLeftMargin ( Pdf_p p, float margin )
	{
		( ( Pdf * ) p )->SetLeftMargin ( margin );
		return p;
	}

	Pdf_p flypdf_SetTopMargin ( Pdf_p p, float margin )
	{
		( ( Pdf * ) p )->SetTopMargin ( margin );
		return p;
	}

	Pdf_p flypdf_SetRightMargin ( Pdf_p p, float margin )
	{
		( ( Pdf * ) p )->SetRightMargin ( margin );
		return p;
	}

	Pdf_p flypdf_SetAutoPageBreak ( Pdf_p p, int autopb, float margin )
	{
		( ( Pdf * ) p )->SetAutoPageBreak ( autopb != 0, margin );
		return p;
	}

	Pdf_p flypdf_SetDisplayMode ( Pdf_p p, char const *zoom, char const *layout )
	{
		( ( Pdf * ) p )->SetDisplayMode ( zoom, layout );
		return p;
	}

	Pdf_p flypdf_SetTitle ( Pdf_p p, char const *title )
	{
		( ( Pdf * ) p )->SetTitle ( title );
		return p;
	}

	Pdf_p flypdf_SetSubject ( Pdf_p p, char const *subject )
	{
		( ( Pdf * ) p )->SetSubject ( subject );
		return p;
	}

	Pdf_p flypdf_SetAuthor ( Pdf_p p, char const *author )
	{
		( ( Pdf * ) p )->SetAuthor ( author );
		return p;
	}

	Pdf_p flypdf_SetKeywords ( Pdf_p p, char const *keywords )
	{
		( ( Pdf * ) p )->SetKeywords ( keywords );
		return p;
	}

	Pdf_p flypdf_SetCreator ( Pdf_p p, char const *creator )
	{
		( ( Pdf * ) p )->SetCreator ( creator );
		return p;
	}

	Pdf_p flypdf_SetAliasNbPages ( Pdf_p p, char const *alias )
	{
		( ( Pdf * ) p )->SetAliasNbPages ( alias );
		return p;
	}

	Pdf_p flypdf_AddPage ( Pdf_p p, char const *orientation )
	{
		( ( Pdf * ) p )->AddPage ( orientation );
		return p;
	}

	Pdf_p flypdf_Open ( Pdf_p p )
	{
		( ( Pdf * ) p )->Open (  );
		return p;
	}

	Pdf_p flypdf_Close ( Pdf_p p )
	{
		( ( Pdf * ) p )->Close (  );
		return p;
	}

	int flypdf_PageNo ( Pdf_p p )
	{
		return ( ( Pdf * ) p )->PageNo (  );
	}

	Pdf_p flypdf_Output ( Pdf_p p, char const *name )
	{
		( ( Pdf * ) p )->Output ( name );
		return p;
	}

	Pdf_p flypdf_Line ( Pdf_p p, double x1, double y1, double x2, double y2 )
	{
		( ( Pdf * ) p )->Line ( x1, y1, x2, y2 );
		return p;
	}

	Pdf_p flypdf_SetLineWidth ( Pdf_p p, float width )
	{
		( ( Pdf * ) p )->SetLineWidth ( width );
		return p;
	}

	Pdf_p flypdf_SetTextColor ( Pdf_p p, int r, int g, int b )
	{
		( ( Pdf * ) p )->SetTextColor ( r, g, b );
		return p;
	}

	Pdf_p flypdf_SetFillColor ( Pdf_p p, int r, int g, int b )
	{
		( ( Pdf * ) p )->SetFillColor ( r, g, b );
		return p;
	}

	Pdf_p flypdf_SetDrawColor ( Pdf_p p, int r, int g, int b )
	{
		( ( Pdf * ) p )->SetDrawColor ( r, g, b );
		return p;
	}

	Pdf_p flypdf_Rect ( Pdf_p p, double x, double y, double w, double h, char const *style )
	{
		( ( Pdf * ) p )->Rect ( x, y, w, h, style );
		return p;
	}

	Pdf_p flypdf_Text ( Pdf_p p, double x, double y, char const *txt )
	{
		( ( Pdf * ) p )->Text ( x, y, txt );
		return p;
	}

	int flypdf_AcceptPageBreak ( Pdf_p p )
	{
		return ( ( Pdf * ) p )->AcceptPageBreak (  );
	}

	Pdf_p flypdf_Ln ( Pdf_p p, int h )
	{
		( ( Pdf * ) p )->Ln ( h );
		return p;
	}

	double flypdf_GetX ( Pdf_p p )
	{
		return ( ( Pdf * ) p )->GetX (  );
	}

	Pdf_p flypdf_SetX ( Pdf_p p, double x )
	{
		( ( Pdf * ) p )->SetX ( x );
		return p;
	}

	double flypdf_GetY ( Pdf_p p )
	{
		return ( ( Pdf * ) p )->GetY (  );
	}

	Pdf_p flypdf_SetY ( Pdf_p p, double y )
	{
		( ( Pdf * ) p )->SetY ( y );
		return p;
	}

	Pdf_p flypdf_SetXY ( Pdf_p p, double x, double y )
	{
		( ( Pdf * ) p )->SetXY ( x, y );
		return p;
	}

	Pdf_p flypdf_Cell ( Pdf_p p, double w, double h, char const *txt, char const *border, double ln, char const *align, int fill,
			    char const *link )
	{
		( ( Pdf * ) p )->Cell ( w, h, txt, border, ln, align, fill, link );
		return p;
	}

	Pdf_p flypdf_SetFont ( Pdf_p p, char const *family, char const *style, double size )
	{
		( ( Pdf * ) p )->SetFont ( family, style, size );
		return p;
	}

	double flypdf_GetStringWidth ( Pdf_p p, char const *s )
	{
		return ( ( Pdf * ) p )->GetStringWidth ( s );
	}

	Pdf_p flypdf_Image ( Pdf_p p, char const *image, double x, double y, double w, double h, char const *type, char const *link )
	{
		( ( Pdf * ) p )->Image ( image, x, y, w, h, type, link );
		return p;
	}

	int flypdf_AddLink ( Pdf_p p )
	{
		return ( ( Pdf * ) p )->AddLink (  );
	}
	Pdf_p flypdf_SetLink ( Pdf_p p, int link, double y, int page )
	{
		( ( Pdf * ) p )->SetLink ( link, y, page );
		return p;
	}

	Pdf_p flypdf_ExternalLink ( Pdf_p p, double x, double y, double w, double h, char const *url )
	{
		( ( Pdf * ) p )->ExternalLink ( x, y, w, h, url );
		return p;
	}

	Pdf_p flypdf_InternalLink ( Pdf_p p, double x, double y, double w, double h, int linkid )
	{
		( ( Pdf * ) p )->InternalLink ( x, y, w, h, linkid );
		return p;
	}

	Pdf_p flypdf_AddTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset, char const *ofamily, char const *ostyle )
	{
		( ( Pdf * ) p )->AddFont ( ( ( Pdf * ) p )->ReadTrueTypeFont ( font_file, charset ), ofamily, ostyle );	
		return p;
	}
	
	Pdf_p flypdf_SaveReducedTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset, char ** dest_file_name )
	{
		std::string cpp_dest_f_name = ( ( Pdf * ) p )->SaveReducedTrueTypeFont(font_file, charset);
		*dest_file_name = (char*)malloc(cpp_dest_f_name.size()+1);
		for(unsigned int i=0; i<cpp_dest_f_name.size(); ++i)
			(*dest_file_name)[i] = cpp_dest_f_name[i];
		(*dest_file_name)[cpp_dest_f_name.size()] = '\0';
		return p;
	}
	
	Pdf_p flypdf_AddType1Font ( Pdf_p p, char const *afm_file, char const *font_file, char const *charset, char const *ofamily, char const * ostyle )
	{
		( ( Pdf * ) p )->AddFont ( ( ( Pdf * ) p )->ReadType1Font ( afm_file, font_file, charset ), ofamily, ostyle );
		return p;
	}

	Pdf_p flypdf_TextWithDirection ( Pdf_p p, double x, double y, char const *xt, char const *direction )
	{
		( ( Pdf * ) p )->TextWithDirection ( x, y, xt, direction );
		return p;
	}

	Pdf_p flypdf_TextWithRotation ( Pdf_p p, double x, double y, char const *text, double txt_angle, double font_angle )
	{
		( ( Pdf * ) p )->TextWithRotation ( x, y, text, txt_angle, font_angle );
		return p;
	}

	Pdf_p flypdf_RoundedRect ( Pdf_p p, double x, double y, double w, double h, double r, char const *style, char const *angle )
	{
		( ( Pdf * ) p )->RoundedRect ( x, y, w, h, r, style, angle );
		return p;
	}

	Pdf_p flypdf_Circle ( Pdf_p p, double x, double y, double r, char const *style )
	{
		( ( Pdf * ) p )->Circle ( x, y, r, style );
		return p;
	}

	Pdf_p flypdf_Ellipse ( Pdf_p p, double x, double y, double rx, double ry, char const *style )
	{
		( ( Pdf * ) p )->Ellipse ( x, y, rx, ry, style );
		return p;
	}

	void flypdf__out ( Pdf_p p, char const *s )
	{
		( ( Pdf * ) p )->_out ( s );
	}

	Pdf_p flypdf_MultiCell ( Pdf_p p, double w, double h, char const *txt, char const *border, char const *align, int fill)
	{
		( ( Pdf * ) p )->MultiCell(w, h, txt, border, align, fill);
		return p;
	}

	Pdf_p flypdf_Write ( Pdf_p p, double h, char const *txt, char const *link)
	{
		( ( Pdf * ) p )->Write(h, txt, link);
		return p;
	}

	Pdf_p flypdf_SetFontSize ( Pdf_p p, double s)
	{
		( ( Pdf * ) p )->SetFontSize(s);
		return p;
	}

	Pdf_p flypdf_SetHeaderDelegate( Pdf_p p, void(*callback)(Pdf_p) )
	{
		((PDF_CWrapper*)p)->header_callback = (PDF_CWrapper::PdfCallback)callback;
		return p;
	}

	Pdf_p flypdf_SetFooterDelegate( Pdf_p p, void(*callback)(Pdf_p) )
	{
		((PDF_CWrapper*)p)->footer_callback = (PDF_CWrapper::PdfCallback)callback;
		return p;
	}

	Pdf_p flypdf_SetAcceptPageBreakDelegate(Pdf_p p, int(*callback)(Pdf_p) )
	{
		((PDF_CWrapper*)p)->pagebreak_callback = (PDF_CWrapper::PBreakCallback)callback;
		return p;
	}

	/* CSharp Utility Functions */
	DLL void *flypdf_csharp_SaveReducedTrueTypeFont ( Pdf_p p, char const *font_file, char const * charset)
	{
		char ** result = (char**)malloc(sizeof(char*));
		flypdf_SaveReducedTrueTypeFont(p, font_file, charset, result);
		return result;
	}

	DLL char * flypdf_csharp_get_string_from_ss(void *s)
	{
		char ** r = (char**)s;
		return *r;
	}

	DLL void flypdf_csharp_dealloc_string_from_ss(void *s)
	{
		char ** r = (char**)s;
		free(*r);
		free(r);
	}
}
