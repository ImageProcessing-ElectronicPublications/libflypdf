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
//
// C++ Interface: flypdf
//
// Description: 
//
//
// Author:  <amol@alex>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __FLYPDF_H__
#define __FLYPDF_H__

#ifdef WIN32

#pragma warning (disable : 4290)
#define _CRT_SECURE_NO_DEPRECATE
#define _USE_MATH_DEFINES

#ifdef CPP_LIB
#define DLL		__declspec ( dllexport )
#else
#define DLL
#endif

#include <windows.h>

#else

#include <stdlib.h>

#define DLL

#endif



typedef unsigned short TT_USHORT, * TT_PUSHORT;
typedef unsigned int TT_ULONG;
typedef int TT_LONG, TT_DWORD, * TT_PDWORD;
typedef unsigned int TT_UINT;
typedef int TT_BOOL;
typedef short TT_SHORT, TT_WCHAR;
typedef unsigned char TT_BYTE, * TT_PBYTE;
typedef unsigned char TT_UCHAR, * TT_PUCHAR;

#define MemAlloc malloc
#define MemFree free


#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <sstream>
#include <algorithm>
#include <cstdio>

#include "fonts.h"
#include "images.h"
#include "truetype.h"

struct PdfException:std::exception
{
	std::string desc;

	PdfException ( std::string const &msg ):desc ( msg )
	{
	}

	PdfException ( PdfException const &e ):desc ( e.desc )
	{
	}

	template < typename T > PdfException & operator<< ( T const &i )
	{
		std::ostringstream ss;
		ss << desc;
		ss << i;
		desc = ss.str (  );
		return *this;
	}

	virtual const char *what (  ) const throw (  )
	{
		return desc.c_str (  );
	}

	virtual ~ PdfException (  )throw (  )
	{
	}
};

class Pdf
{
      protected:
	struct Link
	{
		double x, y, h, w;
		int link;
		bool internal;
		std::string url;
	};

	int page;		//current page number
	int n;			//current object number
	std::map < int, unsigned long long int >offsets;	//array of object offsets
	std::string buffer;	//buffer holding in-memory PDF
	std::map < int, std::string > pages;	//array containing pages
	int state;		//current document state
	std::string DefOrientation;	//default orientation
	std::string CurOrientation;	//current orientation
	std::map < int, bool > OrientationChanges;	//array indicating orientation changes
	float k;		//scale factor (number of points in user unit)
	float fwPt, fhPt;	//dimensions of page format in points
	float fw, fh;		//dimensions of page format in user unit
	float wPt, hPt;		//current dimensions of page in points
	float w, h;		//current dimensions of page in user unit
	float lMargin;		//left margin
	float tMargin;		//top margin
	float rMargin;		//right margin
	float bMargin;		//page break margin
	float cMargin;		//cell margin
	float x, y;		//current position in user unit for cell positioning
	float lasth;		//height of last cell printed
	float LineWidth;	//line width in user unit
	std::map < std::string, std::string > CoreFonts;	//array of standard font names
	std::map < std::string, FontData > fonts;	//array of used fonts
	std::map < std::string, FontFileData > FontFiles;
	std::map < std::string, ImageData > images;	//array of used images
	std::vector < std::string > diffs;
	std::map < int, std::vector < Link > >PageLinks;	//array of links in pages
	std::vector < std::pair < int, double > >links;	//array of internal links
	std::string FontFamily;	//current font family
	std::string FontStyle;	//current font style
	int underline;		//underlining flag
	FontData *CurrentFont;	//current font info
	double FontSizePt;	//current font size in points
	double FontSize;	//current font size in user unit
	std::string DrawColor;	//commands for drawing color
	std::string FillColor;	//commands for filling color
	std::string TextColor;	//commands for text color
	bool ColorFlag;		//indicates whether fill and text colors are different
	float ws;		//word spacing
	int AutoPageBreak;	//automatic page breakin
	float PageBreakTrigger;	//threshold used to trigger page breaks
	bool InFooter;		//flag set when processing footer
	std::string ZoomMode;	//zoom display mode
	std::string LayoutMode;	//layout display mode
	std::string title;	//title
	std::string subject;	//subject
	std::string author;	//author
	std::string keywords;	//keywords
	std::string creator;	//creator
	bool compression;	//compression
	std::string AliasNbPages;	//alias for total number of pages
	std::string PDFVersion;	//PDF version number

	std::map < std::string, std::map < unsigned char, int > >fpdf_charwidths;

      protected:
	template < typename T > std::string fval ( char const *format, T const &v )
	{
		char buffer[4096];
		sprintf ( buffer, format, v );
		return buffer;
	}

	template < typename T, typename T2 > std::string fval ( char const *format, T const &v1, T2 const &v2 )
	{
		char buffer[4096];
		sprintf ( buffer, format, v1, v2 );
		return buffer;
	}

	template < typename T, typename T2, typename T3 > std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3 )
	{
		char buffer[4096];
		sprintf ( buffer, format, v1, v2, v3 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4 )
	{
		char buffer[8192];
		sprintf ( buffer, format, v1, v2, v3, v4 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4, typename T5 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4, T5 const &v5 )
	{
		char buffer[8192];
		sprintf ( buffer, format, v1, v2, v3, v4, v5 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4, typename T5, typename T6 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4, T5 const &v5, T6 const &v6 )
	{
		char buffer[8192];
		sprintf ( buffer, format, v1, v2, v3, v4, v5, v6 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4, T5 const &v5, T6 const &v6, T7 const &v7 )
	{
		char buffer[8192];
		sprintf ( buffer, format, v1, v2, v3, v4, v5, v6, v7 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4, T5 const &v5, T6 const &v6, T7 const &v7,
				   T8 const &v8 )
	{
		char buffer[16384];
		sprintf ( buffer, format, v1, v2, v3, v4, v5, v6, v7, v8 );
		return buffer;
	}

	template < typename T, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9 >
		std::string fval ( char const *format, T const &v1, T2 const &v2, T3 const &v3, T4 const &v4, T5 const &v5, T6 const &v6, T7 const &v7,
				   T8 const &v8, T9 const &v9 )
	{
		char buffer[16384];
		sprintf ( buffer, format, v1, v2, v3, v4, v5, v6, v7, v8, v9 );
		return buffer;
	}

	template < typename T > static std::string nval ( T const &v )
	{
		std::ostringstream ss;
		ss << v;
		return ss.str (  );
	}
	
	template <typename T> static T as(std::string const &s)
	{
		T v;
		std::istringstream ss(s);
		ss >> v;
		return v; 
	}

	DLL static std::vector < char >vread ( FILE * f, int size );
	DLL static int iread ( FILE * f );
	DLL static std::string sread ( FILE * f, int size );

	DLL std::string lowerstr ( std::string s );
	DLL std::string upperstr ( std::string s );
	DLL std::string str_replace ( std::string const &what, std::string const &replacement, std::string const &str );
	DLL std::string trimstr(std::string const &s);

	DLL void _beginpage ( std::string const &orientation );
	DLL void _endpage (  );
	DLL void _enddoc (  );
	DLL void _putheader (  );
	DLL void _puttrailer (  );
	DLL void _newobj (  );
	DLL void _putstream ( std::string const &s );
	DLL void _putstream_comp ( std::string const &s );
	DLL void _putstream ( std::vector < char >const &v );
	DLL void _putpages (  );
	DLL void _putinfo (  );
	DLL void _putcatalog (  );
	DLL std::string _textstring ( std::string const &s );
	DLL std::string _escape ( std::string const &os );
	DLL void _putfonts (  );
	DLL void _putresources (  );
	DLL void _putresourcedict (  );
	DLL void _putxobjectdict (  );
	DLL void _putimages (  );
	DLL std::string _dounderline ( double x, double y, std::string const &txt );

	//Fonts loading functions
	DLL void InitInternalFonts (  );
	std::vector< std::pair<std::string, unsigned long> > ReadMap(std::string const &enc);
	FontData ReadAFM(std::string const &file, std::vector< std::pair<std::string, unsigned long> > &map);
	std::string MakeFontEncoding(std::vector< std::pair<std::string, unsigned long> > const &map);

	//Images loading functions
	friend ImageData LoadJPG ( std::string const &file );
	friend ImageData LoadPNG ( std::string const &file );

	//Functions pubblic to be able to extend a bit the library in Python
      public:
	DLL void _out ( std::string const &s );

      public:
	DLL Pdf ( std::string const &orientation = "P", std::string const &unit = "mm", std::string const &format = "A4" ) throw(PdfException);
	DLL virtual ~ Pdf (  );

	DLL Pdf & SetMargins ( float left, float top, float right = -1 );
	DLL Pdf & SetLeftMargin ( float margin );
	DLL Pdf & SetTopMargin ( float margin );
	DLL Pdf & SetRightMargin ( float margin );
	DLL Pdf & SetAutoPageBreak ( bool autopb, float margin = 0 );
	DLL Pdf & SetDisplayMode ( std::string const &zoom, std::string const &layout = "continuous" ) throw(PdfException);
	DLL Pdf & SetTitle ( std::string const &title );
	DLL Pdf & SetSubject ( std::string const &subject );
	DLL Pdf & SetAuthor ( std::string const &author );
	DLL Pdf & SetKeywords ( std::string const keywords );
	DLL Pdf & SetCreator ( std::string const &creator );
	DLL Pdf & SetCompression ( bool const compression );
	DLL Pdf & SetAliasNbPages ( std::string const &alias = "{nb}" );
	DLL Pdf & AddPage ( std::string const &orientation = "" );
	DLL Pdf & Open (  );
	DLL Pdf & Close (  );
	DLL int PageNo (  );
	DLL Pdf & Output ( std::string const &name = "dest.pdf" ) throw(PdfException);
	DLL Pdf & Line ( double x1, double y1, double x2, double y2 );
	DLL Pdf & SetLineWidth ( float width );
	DLL Pdf & SetTextColor ( int r, int g = -1, int b = -1 );
	DLL Pdf & SetFillColor ( int r, int g = -1, int b = -1 );
	DLL Pdf & SetDrawColor ( int r, int g = -1, int b = -1 );
	DLL Pdf & Rect ( double x, double y, double w, double h, std::string const &style = "" );
	DLL Pdf & Text ( double x, double y, std::string const &txt );
	DLL virtual int AcceptPageBreak (  );
	DLL Pdf & Ln ( int h = -1 );
	DLL double GetX (  );
	DLL Pdf & SetX ( double x );
	DLL double GetY (  );
	DLL Pdf & SetY ( double y );
	DLL Pdf & SetXY ( double x, double y );
	DLL Pdf & Cell ( double w, double h = 0, std::string const &txt = "", std::string const &border = "0", double ln = 0, std::string const &align =
		     "", int fill = 0, std::string const &link = "" );
	DLL Pdf & MultiCell ( double w, double h, std::string const &txt, std::string const &border = "", std::string const &align = "J", int fill = 0 ) throw(PdfException);
	DLL Pdf & SetFont ( std::string const &family, std::string const &style = "", double size = 0 ) throw(PdfException);
	DLL Pdf & SetFontSize(double size);
	DLL double GetStringWidth ( std::string const &s );
	DLL Pdf & Image ( std::string const &image, double x, double y, double w = 0, double h = 0, std::string const &type =
		      "", std::string const &link = "" ) throw(PdfException);
	DLL int AddLink (  );
	DLL Pdf & SetLink ( int link, double y = 0, int page = -1 );
	DLL Pdf & ExternalLink ( double x, double y, double w, double h, std::string const &url );
	DLL Pdf & InternalLink ( double x, double y, double w, double h, int linkid );
	DLL FontData ReadTrueTypeFont ( std::string const &font_file, std::string const & charset = "") throw(PdfException);
	DLL FontData ReadType1Font ( std::string const &afm_file, std::string const &font_file, std::string const &charset = "" ) throw(PdfException);
	DLL std::string SaveReducedTrueTypeFont ( std::string const &file, std::string const & charset ) throw(PdfException);
	DLL Pdf & AddFont ( FontData fdata, std::string const &ofamily, std::string const &ostyle = "" ) throw(PdfException);
	DLL Pdf & TextWithDirection ( double x, double y, std::string const &xt, std::string const &direction = "R" );
	DLL Pdf & TextWithRotation ( double x, double y, std::string const &text, double txt_angle, double font_angle = 0 );
	DLL Pdf & RoundedRect ( double x, double y, double w, double h, double r, std::string const &style = "", std::string const &angle = "1234" );
	DLL Pdf & _Arc ( double x1, double y1, double x2, double y2, double x3, double y3 );
	DLL Pdf & Circle ( double x, double y, double r, std::string const &style = "" );
	DLL Pdf & Ellipse ( double x, double y, double rx, double ry, std::string const &style = "D" );
	DLL Pdf & Write(double h, std::string const &txt, std::string const &link = "");

	DLL virtual void Footer (  );
	DLL virtual void Header (  );
};


struct CPDFimageInfo
{
	int process;
	unsigned int width;
	unsigned int height;
	int ncomponents;
	int bitspersample;
};

void _do_log ( const char * s );

//#define do_log(s) _do_log(s)
#define do_log(s)

#endif
