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
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <zlib.h>
#include <cstring>

void _do_log ( const char * s )
{
	FILE * fh = fopen ( "c:\\log.txt", "a" );
	fprintf ( fh, "%s\n", s );
	fclose ( fh );
}


Pdf::Pdf ( std::string const &orientation, std::string const &unit, std::string const &format ) throw(PdfException)
:page ( 0 ), n ( 2 ), state ( 0 ), lasth ( 0 ),
underline ( false ), FontSizePt ( 12 ), DrawColor ( "0 G" ), FillColor ( "0 g" ), TextColor ( "0 g" ), ColorFlag ( false ), ws ( 0 ),
InFooter ( false )
{
	//Fill Standard Fonts
	CoreFonts["courier"] = "Courier";
	CoreFonts["courierB"] = "Courier-Bold";
	CoreFonts["courierI"] = "Courier-Oblique";
	CoreFonts["courierBI"] = "Courier-BoldOblique";
	CoreFonts["helvetica"] = "Helvetica";
	CoreFonts["helveticaB"] = "Helvetica-Bold";
	CoreFonts["helveticaI"] = "Helvetica-Oblique";
	CoreFonts["helveticaBI"] = "Helvetica-BoldOblique";
	CoreFonts["times"] = "Times-Roman";
	CoreFonts["timesB"] = "Times-Bold";
	CoreFonts["timesI"] = "Times-Italic";
	CoreFonts["timesBI"] = "Times-BoldItalic";
	CoreFonts["symbol"] = "Symbol";
	CoreFonts["zapfdingbats"] = "ZapfDingbats";

	InitInternalFonts (  );

	//Scale Factor
	if ( unit == "pt" )
		k = 1;
	else if ( unit == "mm" )
		k = (float)( 72 / 25.4 );
	else if ( unit == "cm" )
		k = (float)( 72 / 2.54 );
	else if ( unit == "in" )
		k = 72;
	else
		throw PdfException ( "Incorrect unit: " ) << unit;

	//Page format
	std::string lformat = format;
	std::transform ( format.begin (  ), format.end (  ), lformat.begin (  ), tolower );

	if ( lformat == "a3" )
	{
		fwPt = (float)( 841.89 );
		fhPt = (float)( 1190.55 );
	} else if ( lformat == "a4" )
	{
		fwPt = (float)( 595.28 );
		fhPt = (float)( 841.89 );
	} else if ( lformat == "a5" )
	{
		fwPt = (float)( 420.94 );
		fhPt = (float)( 595.28 );
	} else if ( lformat == "letter" )
	{
		fwPt = (float)( 612.0 );
		fhPt = (float)( 792.0 );
	} else if ( lformat == "legal" )
	{
		fwPt = (float)( 612.0 );
		fhPt = (float)( 1008.0 );
	} else
		throw PdfException ( "Unknown page format: " ) << format;

	fw = fwPt / k;
	fh = fhPt / k;

	//Page orientation
	std::string lorientation = orientation;
	std::transform ( orientation.begin (  ), orientation.end (  ), lorientation.begin (  ), tolower );

	if ( lorientation == "p" || lorientation == "portrait" )
	{
		DefOrientation = "P";
		wPt = fwPt;
		hPt = fhPt;
	} else if ( lorientation == "l" || lorientation == "landscape" )
	{
		DefOrientation = "L";
		wPt = fhPt;
		hPt = fwPt;
	} else
		throw PdfException ( "Incorrect Orientation: " ) << orientation;

	CurOrientation = DefOrientation;
	w = wPt / k;
	h = hPt / k;

	//Page margins (1 cm)
	float margin = (float) ( 28.35 / k );
	SetMargins ( margin, margin );

	//Interior cell margin (1 mm)
	cMargin = margin / 10;
	//Line width (0.2 mm)
	LineWidth = (float) ( 0.567 / k );
	//Automatic page break
	SetAutoPageBreak ( true, 2 * margin );
	//Full width display mode
	SetDisplayMode ( "fullwidth" );
	//Set default PDF version number
	PDFVersion = "1.3";
	//set default compression mode
	compression = true;
}

Pdf::~Pdf (  )
{
}

Pdf & Pdf::SetMargins ( float left, float top, float right )
{
	//Set left, top and right margins
	lMargin = left;
	tMargin = top;

	if ( right == -1 )
		right = left;

	rMargin = right;
	return *this;
}

Pdf & Pdf::SetLeftMargin ( float margin )
{
	//Set left margin
	this->lMargin = margin;
	if ( this->page > 0 && this->x < margin )
		this->x = margin;

	return *this;
}

Pdf & Pdf::SetTopMargin ( float margin )
{
	//Set top margin
	this->tMargin = margin;
	return *this;
}

Pdf & Pdf::SetRightMargin ( float margin )
{
	//Set right margin
	this->rMargin = margin;
	return *this;
}

Pdf & Pdf::SetAutoPageBreak ( bool autopb, float margin )
{
	//Set auto page break mode and triggering margin
	AutoPageBreak = autopb;
	bMargin = margin;
	PageBreakTrigger = h - margin;

	return *this;
}

Pdf & Pdf::SetDisplayMode ( std::string const &zoom, std::string const &layout ) throw(PdfException)
{
	//Set display mode in viewer
	if ( zoom == "fullpage" || zoom == "fullwidth" || zoom == "real" || zoom == "default" )
		ZoomMode = zoom;
	else
		throw PdfException ( "Incorrect zoom display mode: " ) << zoom;

	if ( layout == "single" || layout == "continuous" || layout == "two" || layout == "default" )
		LayoutMode = layout;
	else
		throw PdfException ( "Incorrect layout display mode: " ) << layout;

	return *this;
}

int Pdf::AddLink (  )
{
	//Create a new internal link
	int n = static_cast<int>(this->links.size());
	this->links.push_back ( std::make_pair ( 0, 0 ) );
	return n;
}

Pdf & Pdf::SetLink ( int link, double y, int page )
{
	//Set destination of internal link
	if ( y == -1 )
		y = this->y;
	if ( page == -1 )
		page = this->page;
	this->links[link] = std::make_pair ( page, y );

	return *this;
}

Pdf & Pdf::InternalLink ( double x, double y, double w, double h, int link )
{
	//Put a link on the page
	Link l;
	l.x = x * this->k;
	l.y = this->hPt - y * this->k;
	l.w = w * this->k;
	l.h = h * this->k;
	l.link = link;
	l.internal = true;

	this->PageLinks[this->page].push_back ( l );

	return *this;
}

Pdf & Pdf::ExternalLink ( double x, double y, double w, double h, std::string const &url )
{
	//Put a link on the page
	Link l;
	l.x = x * this->k;
	l.y = this->hPt - y * this->k;
	l.w = w * this->k;
	l.h = h * this->k;
	l.url = url;
	l.internal = false;

	this->PageLinks[this->page].push_back ( l );

	return *this;
}

Pdf & Pdf::SetTitle ( std::string const &title )
{
	//Title of document
	this->title = title;
	return *this;
}

Pdf & Pdf::SetSubject ( std::string const &subject )
{
	//Subject of document
	this->subject = subject;
	return *this;
}

Pdf & Pdf::SetAuthor ( std::string const &author )
{
	//Author of document
	this->author = author;
	return *this;
}

Pdf & Pdf::SetKeywords ( std::string const keywords )
{
	//Keywords of document
	this->keywords = keywords;
	return *this;
}

Pdf & Pdf::SetCreator ( std::string const &creator )
{
	//Creator of document
	this->creator = creator;
	return *this;
}

Pdf & Pdf::SetCompression ( bool const compression )
{
	this->compression = compression;
	return *this;
}

Pdf & Pdf::SetAliasNbPages ( std::string const &alias )
{
	//Define an alias for total number of pages
	this->AliasNbPages = alias;
	return *this;
}

void Pdf::Footer (  )
{
}
void Pdf::Header (  )
{
}

void Pdf::_beginpage ( std::string const &orient )
{
	pages[++page] = "";
	state = 2;
	x = lMargin;
	y = tMargin;
	FontFamily = "";

	//Page orientation
	std::string orientation = DefOrientation;
	if ( !orient.empty (  ) )
	{
		orientation = toupper ( orient[0] );
		if ( orientation != DefOrientation )
			OrientationChanges[page] = true;
	}

	if ( orientation != CurOrientation )
	{
		//Change orientation
		if ( orientation == "P" )
		{
			wPt = fwPt;
			hPt = fhPt;
			w = fw;
			h = fh;
		} else
		{
			wPt = fhPt;
			hPt = fwPt;
			w = fh;
			h = fw;
		}

		PageBreakTrigger = h - bMargin;
		CurOrientation = orientation;
	}
}

void Pdf::_endpage (  )
{
	state = 1;
}

Pdf & Pdf::Open (  )
{
	//Begin document
	state = 1;
	return *this;
}

Pdf & Pdf::Close (  )
{
	//Terminate document
	if ( state == 3 )
		return *this;
	if ( page == 0 )
		AddPage (  );
	//Page footer
	InFooter = true;
	this->Footer (  );
	InFooter = false;
	//Close page
	_endpage (  );
	//Close document
	_enddoc (  );

	return *this;
}

int Pdf::PageNo (  )
{
	//Get current page number
	return this->page;
}


void Pdf::_out ( std::string const &s )
{
	std::string tmp = s;
	tmp += "\n";

	//Add a line to the document
	if ( state == 2 )
		pages[page] += tmp;
	else
		buffer += tmp;
}

std::string Pdf::lowerstr ( std::string s )
{
	std::transform ( s.begin (  ), s.end (  ), s.begin (  ), tolower );
	return s;
}

std::string Pdf::upperstr ( std::string s )
{
	std::transform ( s.begin (  ), s.end (  ), s.begin (  ), toupper );
	return s;
}

std::string Pdf::str_replace ( std::string const &what, std::string const &replacement, std::string const &str )
{
	std::string s = str;
	std::string::size_type fp;

	while ( ( fp = s.find ( what ) ) != std::string::npos )
		s.replace ( fp, what.size (  ), replacement );

	return s;
}

		
std::string Pdf::trimstr(std::string const &s)
{
	std::string cp = s;
	if (cp.empty())
		return cp;
	
	while(isspace(*(cp.end()-1)))
		cp.erase(cp.end()-1);
		
	return cp;
}

Pdf & Pdf::AddPage ( std::string const &orientation )
{
	//Start a new page
	if ( state == 0 )
		Open (  );

	std::string family = FontFamily;
	std::string style = FontStyle;
	style += ( underline ? "U" : "" );
	double size = this->FontSizePt;
	float lw = LineWidth;
	std::string dc = DrawColor;
	std::string fc = FillColor;
	std::string tc = TextColor;
	bool cf = ColorFlag;

	if ( page > 0 )
	{
		//Page footer
		InFooter = true;
		this->Footer (  );
		InFooter = false;
		//Close page
		_endpage (  );
	}

	//Start new page
	_beginpage ( orientation );
	//Set line cap style to square
	_out ( "2 J" );

	//Set line width
	LineWidth = lw;
	_out ( fval ( "%.2f w", lw * k ) );

	if ( !family.empty (  ) )
		SetFont ( family, style, size );

	//Set colors
	DrawColor = dc;
	if ( dc != "0 G" )
		_out ( dc );
	FillColor = fc;
	if ( fc != "0 g" )
		_out ( fc );
	TextColor = tc;
	ColorFlag = cf;

	//Page header
	this->Header (  );

	//Restore line width
	if ( LineWidth != lw )
	{
		LineWidth = lw;

		_out ( fval ( "%.2f w", lw * k ) );
	}

	if ( !family.empty (  ) )
		SetFont ( family, style, size );

	//Restore colors
	if ( DrawColor != dc )
	{
		DrawColor = dc;
		_out ( dc );
	}

	if ( FillColor != fc )
	{
		FillColor = fc;
		_out ( fc );
	}

	TextColor = tc;
	ColorFlag = cf;

	return *this;
}

Pdf & Pdf::Output ( std::string const &name ) throw(PdfException)
{
	//Output PDF to some destination
	//Finish document if necessary
	if ( state < 3 )
		Close (  );

	//Save to local file
	FILE *f = fopen ( name.c_str (  ), "wb" );
	if ( !f )
		throw PdfException ( "Unable to create output file: " ) << name;
	fwrite ( buffer.c_str (  ), sizeof ( char ), buffer.size (  ), f );
	fclose ( f );

	return *this;
}

void Pdf::_putheader (  )
{
	std::string tmp = "%PDF-";
	tmp += PDFVersion;

	_out ( tmp );
}

void Pdf::_puttrailer (  )
{
	std::string tmp;

	tmp = "/Size ";
	tmp += nval ( n + 1 );
	_out ( tmp );

	tmp = "/Root ";
	tmp += nval ( n );
	tmp += " 0 R";
	_out ( tmp );

	tmp = "/Info ";
	tmp += nval ( n - 1 );
	tmp += " 0 R";
	_out ( tmp );
}

void Pdf::_putinfo (  )
{
	std::string tmp;

	_out ( "/Producer (FLYPDF 1.0)" );
	if ( !title.empty (  ) )
	{
		tmp = "/Title ";
		tmp += _textstring ( title );
		_out ( tmp );
	}
	if ( !subject.empty (  ) )
	{
		tmp = "/Subject ";
		tmp += _textstring ( subject );
		_out ( tmp );
	}
	if ( !author.empty (  ) )
	{
		tmp = "/Author ";
		tmp += _textstring ( author );
		_out ( tmp );
	}
	if ( !keywords.empty (  ) )
	{
		tmp = "/Keywords ";
		tmp += _textstring ( keywords );
		_out ( tmp );
	}
	if ( !creator.empty (  ) )
	{
		tmp = "/Creator ";
		tmp += _textstring ( creator );
		_out ( tmp );
	}

	tmp = "/CreationDate ";
	tmp += _textstring ( "D: 10000101000000" );
	_out ( tmp );
}

std::string Pdf::_textstring ( std::string const &s )
{
	//Format a text string
	std::string tmp = "(";
	tmp += _escape ( s );
	tmp += ")";

	return tmp;
}

std::string Pdf::_escape ( std::string const &os )
{
	std::string s = os;

	//Add \ before \, ( and )
	std::string::size_type fp = 0;
	while ( ( fp = s.find ( '\\', fp ) ) != std::string::npos ) {
		s.replace ( fp, 1, "\\\\" );
		fp += 2;
	}
	
	fp = 0;
	while ( ( fp = s.find ( ')', fp ) ) != std::string::npos ) {
		s.replace ( fp, 1, "\\)" );
		fp+=2;
	}

	fp = 0;
	while ( ( fp = s.find ( '(', fp ) ) != std::string::npos ) {
		s.replace ( fp, 1, "\\(" );
		fp+=2;
	}
	
	return s;
}

std::string Pdf::_dounderline ( double x, double y, std::string const &txt )
{
	//Underline text
	double up = this->CurrentFont->up;
	double ut = this->CurrentFont->ut;
	int num_of_spaces = static_cast<int>(std::count ( txt.begin (  ), txt.end (  ), ' ' ));
	double w = this->GetStringWidth ( txt ) + this->ws * num_of_spaces;

	return fval ( "%.2f %.2f %.2f %.2f re f", x * this->k, ( this->h - ( y - up / 1000 * this->FontSize ) ) * this->k, w * this->k, -ut / 1000 * this->FontSizePt );
}


void Pdf::_putcatalog (  )
{
	_out ( "/Type /Catalog" );
	_out ( "/Pages 1 0 R" );
	if ( ZoomMode == "fullpage" )
		_out ( "/OpenAction [3 0 R /Fit]" );
	else if ( ZoomMode == "fullwidth" )
		_out ( "/OpenAction [3 0 R /FitH null]" );
	else if ( ZoomMode == "real" )
		_out ( "/OpenAction [3 0 R /XYZ null null 1]" );

	if ( LayoutMode == "single" )
		_out ( "/PageLayout /SinglePage" );
	else if ( LayoutMode == "continuous" )
		_out ( "/PageLayout /OneColumn" );
	else if ( LayoutMode == "two" )
		_out ( "/PageLayout /TwoColumnLeft" );
}

void Pdf::_putfonts (  )
{
	std::string tmp;
	int nf = this->n;
	for ( std::vector < std::string >::iterator diff = this->diffs.begin (  ); diff != this->diffs.end (  ); ++diff )
	{
		//Encodings
		this->_newobj (  );
		tmp = "<</Type /Encoding /BaseEncoding /WinAnsiEncoding /Differences [";
		tmp += *diff;
		tmp += "]>>";
		this->_out ( tmp );
		this->_out ( "endobj" );
	}

	for ( std::map < std::string, FontFileData >::iterator i = this->FontFiles.begin (  ); i != this->FontFiles.end (  ); ++i )
	{
		std::string file = i->first;
		FontFileData info = i->second;

		//Font file embedding
		this->_newobj (  );
		this->FontFiles[file].n = this->n;

		std::string font;
		FILE *f = fopen ( file.c_str (  ), "rb" );
		if ( f == 0 )
			throw PdfException ( "Font file to embedd not found: " ) << file;
		while ( !feof ( f ) )
			font += sread ( f, 8192 );
		fclose ( f );

		bool compressed = ( file[file.size (  ) - 1] == 'z' && file[file.size (  ) - 2] == '.' );
		if ( !compressed && ( info.length2 != 0 ) )
		{
			bool header = ( ( static_cast < unsigned char >( font[0] ) ) == 128 );
			if ( header )
			{
				//Strip first binary header
				font = font.substr ( 6 );
			}
			if ( header && ( static_cast < unsigned char >( font[info.length1] ) == 128 ) )
			{
				//Strip second binary header
				font = font.substr ( 0, info.length1 ) + font.substr ( info.length1 + 6 );
			}
		}

		tmp = "<</Length ";
		tmp += nval ( font.size (  ) );
		this->_out ( tmp );

		if ( compressed )
			this->_out ( "/Filter /FlateDecode" );

		tmp = "/Length1 ";
		tmp += nval ( info.length1 );
		this->_out ( tmp );

		if ( info.length2 )
		{
			tmp = "/Length2 ";
			tmp += nval ( info.length2 );
			tmp += " /Length3 0";
			this->_out ( tmp );
		}
		this->_out ( ">>" );
		this->_putstream ( font );
		this->_out ( "endobj" );
	}

	for ( std::map < std::string, FontData >::iterator i = this->fonts.begin (  ); i != this->fonts.end (  ); ++i )
	{
		std::string k = i->first;
		FontData font = i->second;

		//Font objects
		i->second.n = this->n + 1;

		std::string type = font.type;
		std::string name = font.name;

		if ( type == "core" )
		{
			//Standard font
			this->_newobj (  );
			this->_out ( "<</Type /Font" );

			tmp = "/BaseFont /";
			tmp += name;
			this->_out ( tmp );

			this->_out ( "/Subtype /Type1" );
			if ( name != "Symbol" && name != "ZapfDingbats" )
				this->_out ( "/Encoding /WinAnsiEncoding" );
			this->_out ( ">>" );
			this->_out ( "endobj" );
		} else if ( type == "Type1" || type == "TrueType" )	//TODO: END FONT SUPPORT
		{
			//Additional Type1 or TrueType font
			this->_newobj (  );
			this->_out ( "<</Type /Font" );

			tmp = "/BaseFont /";
			tmp += name;
			this->_out ( tmp );

			tmp = "/Subtype /";
			tmp += type;
			this->_out ( tmp );

			this->_out ( "/FirstChar 32 /LastChar 255" );

			tmp = "/Widths ";
			tmp += nval ( this->n + 1 );
			tmp += " 0 R";
			this->_out ( tmp );

			tmp = "/FontDescriptor ";
			tmp += nval ( this->n + 2 );
			tmp += " 0 R";
			this->_out ( tmp );

			if ( !font.enc.empty (  ) )
			{
				if ( font.diff_id != -1 )
				{
					tmp = "/Encoding ";
					tmp += nval ( nf + font.diff_id + 1 );
					tmp += " 0 R";
					this->_out ( tmp );
				} else
				{
					this->_out ( "/Encoding /WinAnsiEncoding" );
				}
			}
			this->_out ( ">>" );
			this->_out ( "endobj" );

			//Widths
			this->_newobj (  );
			tmp = "[";
			for ( int i = 32; i < 256; ++i )
			{
				tmp += nval ( ( *font.cw )[i] );
				tmp += " ";
			}
			tmp += "]";
			this->_out ( tmp );
			this->_out ( "endobj" );

			//Descriptor
			this->_newobj (  );
			tmp = "<</Type /FontDescriptor /FontName /";
			tmp += name;
			tmp += " ";
			tmp += font.desc;

			if ( !font.file.empty (  ) )
			{
				tmp += " /FontFile";
				tmp += ( type == "Type1" ? "" : "2" );
				tmp += " ";
				tmp += nval ( this->FontFiles[font.file].n );
				tmp += " 0 R";
			}
			tmp += ">>";
			this->_out ( tmp );
			this->_out ( "endobj" );
		}
	}
}

void Pdf::_putimages (  )
{
	std::string tmp;

	for ( std::map < std::string, ImageData >::iterator image = this->images.begin (  ); image != this->images.end (  ); ++image )
	{

		ImageData & info = image->second;

		this->_newobj (  );
		info.n = this->n;
		this->_out ( "<</Type /XObject" );
		this->_out ( "/Subtype /Image" );

		tmp = "/Width ";
		tmp += nval ( image->second.w );
		this->_out ( tmp );

		tmp = "/Height ";
		tmp += nval ( image->second.h );
		this->_out ( tmp );

		if ( info.colspace == "Indexed" )
		{
			tmp = "/ColorSpace [/Indexed /DeviceRGB ";
			tmp += nval ( image->second.pal.size (  ) / 3 - 1 );
			tmp += " ";
			tmp += nval ( this->n + 1 );
			tmp += " 0 R]";
			this->_out ( tmp );
		} else
		{
			tmp = "/ColorSpace /";
			tmp += info.colspace;
			this->_out ( tmp );

			if ( info.colspace == "DeviceCMYK" )
				this->_out ( "/Decode [1 0 1 0 1 0 1 0]" );
		}

		tmp = "/BitsPerComponent ";
		tmp += nval ( info.bpc );
		this->_out ( tmp );

		if ( info.f.size (  ) )
		{
			tmp = "/Filter /";
			tmp += info.f;
			this->_out ( tmp );
		}
		if ( info.parms != "" )
		{
			this->_out ( info.parms );
		}
		if ( info.trns.size (  ) )
		{
			std::string trns = "/Mask [";
			for ( unsigned int i = 0; i < info.trns.size (  ); ++i )
			{
				trns += info.trns[i];
				trns += " ";
				trns += info.trns[i];
				trns += " ";
			}
			trns += "]";
			this->_out ( trns );
		}

		tmp = "/Length ";
		tmp += nval ( info.data.size (  ) );
		tmp += ">>";
		this->_out ( tmp );
		this->_putstream ( info.data );
		this->_out ( "endobj" );
	}
}

void Pdf::_putxobjectdict (  )
{
	std::string tmp;

	for ( std::map < std::string, ImageData >::iterator image = this->images.begin (  ); image != this->images.end (  ); ++image )
	{
		tmp = "/I";
		tmp += nval ( image->second.i );
		tmp += " ";
		tmp += nval ( image->second.n );
		tmp += " 0 R";

		this->_out ( tmp );
	}
}

void Pdf::_putresourcedict (  )
{
	_out ( "/ProcSet [/PDF /Text /ImageB /ImageC /ImageI]" );
	_out ( "/Font <<" );

	for ( std::map < std::string, FontData >::iterator i = this->fonts.begin (  ); i != this->fonts.end (  ); ++i )
	{
		std::string code = "/F";
		code += fval ( "%i", i->second.i );
		code += " ";
		code += fval ( "%i", i->second.n );
		code += " 0 R";

		this->_out ( code );
	}

	this->_out ( ">>" );
	this->_out ( "/XObject <<" );
	this->_putxobjectdict (  );
	this->_out ( ">>" );
}

void Pdf::_putresources (  )
{
	this->_putfonts (  );
	this->_putimages (  );
	//Resource dictionary
	this->offsets[2] = this->buffer.size (  );
	_out ( "2 0 obj" );
	_out ( "<<" );
	_putresourcedict (  );
	_out ( ">>" );
	_out ( "endobj" );
}

void Pdf::_enddoc (  )
{
	std::string tmp;

	_putheader (  );
	_putpages (  );
	_putresources (  );
	//Info
	_newobj (  );
	_out ( "<<" );

	_putinfo (  );
	_out ( ">>" );
	_out ( "endobj" );
	//Catalog
	_newobj (  );
	_out ( "<<" );
	_putcatalog (  );
	_out ( ">>" );
	_out ( "endobj" );

	//Cross-ref
	unsigned long long int o = buffer.size (  );
	_out ( "xref" );

	tmp = "0 ";
	tmp += nval ( n + 1 );
	_out ( tmp );

	_out ( "0000000000 65535 f " );
	for ( int i = 1; i <= n; i++ )
		_out ( fval ( "%010d 00000 n ", offsets[i] ) );

	//Trailer
	_out ( "trailer" );
	_out ( "<<" );
	_puttrailer (  );
	_out ( ">>" );
	_out ( "startxref" );
	_out ( nval ( o ) );
	_out ( "%%EOF" );
	state = 3;
}

void Pdf::_newobj (  )
{
	//Begin a new object
	++n;
	offsets[n] = buffer.size (  );
	_out ( nval ( n ) + " 0 obj" );
}

void Pdf::_putstream ( std::string const &s )
{
	_out ( "stream" );
	_out ( s );
	_out ( "endstream" );
}

void Pdf::_putstream_comp ( std::string const &p )
{
/*
			tmp = "<<";
			tmp += filter;
			tmp += "/Length ";
			tmp += nval ( p.size (  ) );
			tmp += ">>";
			_out ( tmp );
			_putstream_comp ( p );
			_out ( "endobj" );
*/
	unsigned char * data = ( unsigned char * ) p.c_str();
	unsigned int dlen = static_cast<unsigned int>(p.size());
	z_stream zstream;
	unsigned char * buffer = ( unsigned char * ) MemAlloc ( dlen + 128 );
	int status = 0;
	std::string * tmp;

	memset ( &zstream, 0, sizeof ( zstream ) );
	deflateInit ( &zstream, 9 );

	zstream.avail_in = static_cast<unsigned int>(dlen);
	zstream.avail_out = static_cast<unsigned int>(dlen + 128);
	zstream.next_in = data;
	zstream.next_out = buffer;

	status = deflate(&zstream, Z_FINISH);

	if ( status ) {
		data = buffer;
		dlen = static_cast<unsigned int>(zstream.next_out - buffer);
	}

	deflateEnd ( & zstream );

	_out ( (( std::string ) "<< /Filter /FlateDecode /Length " ) + nval ( dlen ) + ">>" );
	_out ( "stream" );

	tmp = new std::string ( ( char * ) data, static_cast<unsigned int>(dlen) );
	_out ( * tmp );
	_out ( "endstream" );

	MemFree ( buffer );
	delete tmp;
}

void Pdf::_putstream ( std::vector < char >const &v )
{
	std::string s;

	for ( std::vector < char >::const_iterator i = v.begin (  ); i != v.end (  ); ++i )
		s += *i;

	_out ( "stream" );
	_out ( s );
	_out ( "endstream" );
}

void Pdf::_putpages (  )
{
	int nb = page;
	if ( !AliasNbPages.empty (  ) )
	{
		//Replace number of pages
		for ( int n = 1; n <= nb; ++n )
		{
			std::string::size_type fp;
			while ( ( fp = pages[n].find ( AliasNbPages ) ) != std::string::npos )
				pages[n].replace ( fp, AliasNbPages.size (  ), nval ( nb ) );
		}
	}

	if ( DefOrientation == "P" )
	{
		wPt = fwPt;
		hPt = fhPt;
	} else
	{
		wPt = fhPt;
		hPt = fwPt;
	}

	std::string filter = "";

	for ( int n = 1; n <= nb; ++n )
	{
		//Page
		_newobj (  );
		_out ( "<</Type /Page" );
		_out ( "/Parent 1 0 R" );

		if ( OrientationChanges.find ( n ) != OrientationChanges.end (  ) )
		{
			std::string tmp = fval ( "/MediaBox [0 0 %.2f", hPt );
			tmp += fval ( " %.2f]", wPt );
			_out ( tmp );
		}

		_out ( "/Resources 2 0 R" );

		if ( this->PageLinks.find ( n ) != this->PageLinks.end (  ) )
		{
			std::string annots = "/Annots [";

			std::vector < Link > &pls = this->PageLinks[n];
			for ( std::vector < Link >::iterator pl = pls.begin (  ); pl != pls.end (  ); ++pl )
			{
				std::string rect = fval ( "%.2f %.2f %.2f %.2f", pl->x, pl->y, pl->x + pl->w, pl->y - pl->h );

				annots += "<</Type /Annot /Subtype /Link /Rect [";
				annots += rect;
				annots += "] /Border [0 0 0] ";

				if ( !pl->internal )
				{
					annots += "/A <</S /URI /URI ";
					annots += _textstring ( pl->url );
					annots += ">>>>";
				} else
				{
					std::pair < int, double >l = this->links[pl->link];
					double h = ( this->OrientationChanges.find ( l.first ) != this->OrientationChanges.end (  ) )? wPt : hPt;
					annots += fval ( "/Dest [%d 0 R /XYZ 0 %.2f null]>>", 1 + 2 * l.first, h - l.second * this->k );
				}
			}

			annots += "]";
			this->_out ( annots );
		}

		std::string tmp = "/Contents ";
		tmp += nval ( this->n + 1 );
		tmp += " 0 R>>";

		_out ( tmp );
		_out ( "endobj" );

		//Page content
		std::string p = pages[n];
		_newobj (  );

		if ( ! this->compression )
		{
			tmp = "<<";
			tmp += filter;
			tmp += "/Length ";
			tmp += nval ( p.size (  ) );
			tmp += ">>";
			_out ( tmp );
			_putstream ( p );
			_out ( "endobj" );
		} else {
			_putstream_comp ( p );
		}
	}

	//Pages root
	offsets[1] = buffer.size (  );
	_out ( "1 0 obj" );
	_out ( "<</Type /Pages" );

	std::string kids = "/Kids [";
	for ( int i = 0; i < nb; ++i )
	{
		kids += nval ( 3 + 2 * i );
		kids += " 0 R ";
	}
	kids += "]";
	_out ( kids );

	std::string tmp = "/Count ";
	tmp += nval ( nb );
	_out ( tmp );

	tmp = fval ( "/MediaBox [0 0 %.2f ", wPt );
	tmp += fval ( "%.2f]", hPt );
	_out ( tmp );

	_out ( ">>" );
	_out ( "endobj" );
}

Pdf & Pdf::SetDrawColor ( int r, int g, int b )
{
	//Set color for all stroking operations
	if ( ( r == 0 && g == 0 && b == 0 ) || g == -1 )
		this->DrawColor = fval ( "%.3f G", static_cast<double>(r) / 255 );
	else
		this->DrawColor = fval ( "%.3f %.3f %.3f RG", static_cast<double>(r) / 255, static_cast<double>(g) / 255, static_cast<double>(b) / 255  );
	if ( this->page > 0 )
		this->_out ( this->DrawColor );

	return *this;
}

Pdf & Pdf::SetFillColor ( int r, int g, int b )
{
	//Set color for all filling operations
	if ( ( r == 0 && g == 0 && b == 0 ) || g == -1 )
		this->FillColor = fval ( "%.3f g", static_cast<double>(r) / 255 );
	else
		this->FillColor = fval ( "%.3f %.3f %.3f rg", static_cast<double>(r) / 255, static_cast<double>(g) / 255, static_cast<double>(b) / 255  );
	this->ColorFlag = ( this->FillColor != this->TextColor );
	if ( this->page > 0 )
		this->_out ( this->FillColor );

	return *this;
}

Pdf & Pdf::SetTextColor ( int r, int g, int b )
{
	//Set color for text
	if ( ( r == 0 && g == 0 && b == 0 ) || g == -1 )
		this->TextColor = fval ( "%.3f g", static_cast<double>(r) / 255 );
	else
		this->TextColor = fval ( "%.3f %.3f %.3f rg", static_cast<double>(r) / 255, static_cast<double>(g) / 255, static_cast<double>(b) / 255 );
	this->ColorFlag = ( this->FillColor != this->TextColor );

	return *this;
}

double Pdf::GetStringWidth ( std::string const &s )
{
	//Get width of a string in the current font
	std::map < unsigned char, int >&cw = *( this->CurrentFont->cw );
	double w = 0;
	unsigned int l = static_cast<unsigned int>(s.size( ));

	for ( unsigned int i = 0; i < l; ++i )
		w += cw[s[i]];

	double ris = w * this->FontSize;
	ris /= 1000;
	return ris;
}

Pdf & Pdf::SetFont ( std::string const &ofamily, std::string const &ostyle, double size ) throw(PdfException)
{
	//Select a font; size given in points
	std::string family = ofamily;
	std::transform ( ofamily.begin (  ), ofamily.end (  ), family.begin (  ), tolower );

	if ( family == "" )
		family = FontFamily;
	else if ( family == "arial" )
		family = "helvetica";

	std::string style = ostyle;
	std::transform ( ostyle.begin (  ), ostyle.end (  ), style.begin (  ), toupper );

	if ( family == "symbol" || family == "zapfdingbats" )
		style = "";

	if ( style.find ( 'U' ) != std::string::npos )
	{
		this->underline = true;

		std::string::size_type fp;
		while ( ( fp = style.find ( 'U' ) ) != std::string::npos )
			style.erase ( fp );
	} else
		this->underline = false;

	if ( style == "IB" )
		style = "BI";

	if ( size == 0 )
		size = this->FontSizePt;

	//Test if font is already selected
	if ( this->FontFamily == family && this->FontStyle == style && this->FontSizePt == size )
		return *this;

	//Test if used for the first time
	std::string fontkey = family;
	fontkey += style;

	if ( this->fonts.find ( fontkey ) == this->fonts.end (  ) )
	{
		//Check if one of the standard fonts
		if ( ( this->CoreFonts.find ( fontkey ) ) != this->CoreFonts.end (  ) )
		{
			int i = static_cast<int>(this->fonts.size() + 1);
			this->fonts[fontkey] = FontData ( i, "core", CoreFonts[fontkey], -100, 50, &fpdf_charwidths[fontkey] );
		} else
		{
			throw PdfException ( "Undefined font: " ) << family << ' ' << style;
		}
	}
	//Select it
	this->FontFamily = family;
	this->FontStyle = style;
	this->FontSizePt = size;
	this->FontSize = size / this->k;
	this->CurrentFont = &this->fonts[fontkey];

	if ( this->page > 0 )
		_out ( fval ( "BT /F%d %.2f Tf ET", this->CurrentFont->i, this->FontSizePt ) );

	return *this;
}

Pdf & Pdf::SetFontSize(double size)
{
	//Set font size in points
	if(this->FontSizePt == size)
		return *this;

	this->FontSizePt = size;
	this->FontSize = size / this->k;

	if( this->page>0 )
		_out ( fval ( "BT /F%d %.2f Tf ET", this->CurrentFont->i, this->FontSizePt ) );

	return *this;
}


Pdf & Pdf::SetLineWidth ( float width )
{
	//Set line width
	this->LineWidth = width;
	if ( this->page > 0 )
		this->_out ( fval ( "%.2f w", width * this->k ) );

	return *this;
}

Pdf & Pdf::Line ( double x1, double y1, double x2, double y2 )
{
	//Draw a line
	_out ( fval ( "%.2f ", x1 * k ) + fval ( "%.2f m", ( h - y1 ) * k ) + fval ( " %.2f ", x2 * k ) + fval ( "%.2f l S", ( h - y2 ) * k ) );
	return *this;
}

Pdf & Pdf::Rect ( double x, double y, double w, double h, std::string const &style )
{
	std::string op = "S";

	//Draw a rectangle
	if ( style == "F" )
		op = "f";
	else if ( style == "FD" || style == "DF" )
		op = "B";

	_out ( fval ( "%.2f %.2f %.2f %.2f re %s", x * k, ( this->h - y ) * k, w * k, -h * k, op.c_str (  ) ) );

	return *this;
}

Pdf & Pdf::Text ( double x, double y, std::string const &txt )
{
	//Output a string
	std::string s = fval ( "BT %.2f %.2f Td ", x * k, ( this->h - y ) * k, _escape ( txt ).c_str (  ) );
	s += std::string ( "(" ) + _escape ( txt ) + ") Tj ET";

	if ( this->underline && txt != "" )
		s += std::string ( " " ) + this->_dounderline ( x, y, txt );
	if ( this->ColorFlag )
		s = std::string ( "q " ) + TextColor + " " + s + " Q";
	this->_out ( s );

	return *this;
}

int Pdf::AcceptPageBreak (  )
{
	//Accept automatic page break or not
	return this->AutoPageBreak;
}

Pdf & Pdf::Ln ( int h )
{
	//Line feed; default value is last cell height
	this->x = this->lMargin;

	if ( h == -1 )
		this->y += this->lasth;
	else
		this->y += h;

	return *this;
}

double Pdf::GetX (  )
{
	//Get x position
	return this->x;
}

Pdf & Pdf::SetX ( double x )
{
	//Set x position
	if ( x >= 0 )
		this->x = (float) ( x );
	else
		this->x = (float) ( this->w + x );

	return *this;
}

double Pdf::GetY (  )
{
	//Get y position
	return this->y;
}

Pdf & Pdf::SetY ( double y )
{
	//Set y position and reset x
	this->x = this->lMargin;
	if ( y >= 0 )
		this->y = (float) ( y );
	else
		this->y = (float) ( this->h + y );

	return *this;
}

Pdf & Pdf::SetXY ( double x, double y )
{
	//Set x and y positions
	this->SetY ( y );
	this->SetX ( x );

	return *this;
}

Pdf & Pdf::Cell ( double w, double h, std::string const &txt, std::string const &border, double ln, std::string const &align, int fill,
		  std::string const &link )
{
	//Output a cell
	float k = this->k;
	if ( ((this->y + h) > this->PageBreakTrigger) && (!this->InFooter) && (this->AcceptPageBreak()) )
	{
		//Automatic page break
		float x = this->x;
		float ws = this->ws;
		if ( ws > 0 )
		{
			this->ws = 0;
			this->_out ( "0 Tw" );
		}
		this->AddPage ( this->CurOrientation );
		this->x = x;
		if ( ws > 0 )
		{
			this->ws = ws;
			this->_out ( fval ( "%.3f Tw", ws * k ) );
		}
	}
	if ( w == 0 )
		w = this->w - this->rMargin - this->x;

	std::string s = "";
	if ( fill == 1 || border == "1" )
	{
		std::string op;

		if ( fill == 1 )
			op = ( border == "1" ) ? "B" : "f";
		else
			op = "S";

		s = fval ( "%.2f %.2f %.2f %.2f re %s ", this->x * k, ( this->h - this->y ) * k, w * k, -h * k, op.c_str (  ) );
	}

	if ( border.size (  ) > 1 )
	{
		double x = this->x;
		double y = this->y;

		if ( border.find ( 'L' ) != std::string::npos )
			s += fval ( "%.2f %.2f m %.2f %.2f l S ", x * k, ( this->h - y ) * k, x * k, ( this->h - ( y + h ) ) * k );
		if ( border.find ( 'T' ) != std::string::npos )
			s += fval ( "%.2f %.2f m %.2f %.2f l S ", x * k, ( this->h - y ) * k, ( x + w ) * k, ( this->h - y ) * k );
		if ( border.find ( 'R' ) != std::string::npos )
			s += fval ( "%.2f %.2f m %.2f %.2f l S ", ( x + w ) * k, ( this->h - y ) * k, ( x + w ) * k, ( this->h - ( y + h ) ) * k );
		if ( border.find ( 'B' ) != std::string::npos )
			s += fval ( "%.2f %.2f m %.2f %.2f l S ", x * k, ( this->h - ( y + h ) ) * k, ( x + w ) * k, ( this->h - ( y + h ) ) * k );
	}

	if ( txt != "" )
	{
		double dx = 0;

		if ( align == "R" )
			dx = w - this->cMargin - this->GetStringWidth ( txt );
		else if ( align == "C" )
			dx = (( w - this->GetStringWidth ( txt ) ) / 2);
		else
			dx = this->cMargin;

		if ( this->ColorFlag )
		{
			s += std::string ( "q " );
			s += this->TextColor;
			s += " ";
		}

		std::string txt2 = _escape ( txt );

		s += fval ( "BT %.2f %.2f Td (", ( this->x + dx ) * k, ( this->h - ( this->y + .5 * h + .3 * this->FontSize ) ) * k );
		s += txt2;
		s += ") Tj ET";

		if ( this->underline )
		{
			s += " ";
			s += this->_dounderline ( this->x + dx, this->y + .5 * h + .3 * this->FontSize, txt );
		}
		if ( this->ColorFlag )
			s += " Q";
		if ( !link.empty (  ) )	{
			bool intlink = true;
			for (unsigned int i=0; i < link.size(); ++i) {
				if (!isdigit(link[i])) {
					intlink = false;
					break;
				}
			}

			if (intlink) {
				int linkid = atoi(link.c_str());
				this->InternalLink ( this->x + dx, this->y + 0.5 * h - 0.5 * this->FontSize, this->GetStringWidth ( txt ), this->FontSize,
					     linkid );
			}
			else
				this->ExternalLink ( this->x + dx, this->y + 0.5 * h - 0.5 * this->FontSize, this->GetStringWidth ( txt ), this->FontSize,
					     link );
		}
	}

	if ( !s.empty (  ) )
		this->_out ( s );

	this->lasth = (float) ( h );

	if ( ln > 0 )
	{
		//Go to next line
		this->y += (float) ( h );
		if ( ln == 1 )
			this->x = this->lMargin;
	} else
		this->x += (float) ( w );

	return *this;
}

Pdf &Pdf::Write(double h, std::string const &txt, std::string const &link)
{
	std::map < unsigned char, int >&cw = *( this->CurrentFont->cw );
	double w = this->w - this->rMargin - this->x;
	double wmax = (w - 2*this->cMargin)*1000.0/this->FontSize;
	std::string s = str_replace ( "\r", "", txt );
	unsigned int nb = static_cast<unsigned int>(s.size());
	int sep=-1;
	unsigned int i=0;
	int j=0;
	int l=0;
	int nl=1;

	while(i < nb)
	{
		//Get next character
		char c = s[i];
		if(c=='\n')
		{
			//Explicit line break
			this->Cell(w, h, s.substr(j, i - j), "", 2, "", 0, link);
			++i;
			sep=-1;
			j=i;
			l=0;

			if(nl == 1)
			{
				this->x=this->lMargin;
				w=this->w - this->rMargin - this->x;
				wmax=(w - 2*this->cMargin)*1000.0/this->FontSize;
			}

			++nl;
			continue;
		}

		if(c == ' ')
			sep=i;

		l+=cw[c];
		if(l > wmax)
		{
			//Automatic line break
			if(sep == -1)
			{
				if(this->x > this->lMargin)
				{
					//Move to next line
					this->x=this->lMargin;
					this->y+=static_cast<float>(h);
					w=this->w - this->rMargin - this->x;
					wmax=(w - 2*this->cMargin)*1000.0/this->FontSize;
					++i;
					++nl;
					continue;
				}

				if(i == j)
					++i;

				this->Cell(w, h, s.substr(j, i - j), "", 2, "", 0, link);
			}
			else
			{
				this->Cell(w, h, s.substr(j, sep - j), "", 2, "", 0, link);
				i=sep+1;
			}

			sep=-1;
			j=i;
			l=0;

			if(nl == 1)
			{
				this->x=this->lMargin;
				w=this->w - this->rMargin - this->x;
				wmax=(w - 2*this->cMargin)*1000.0/this->FontSize;
			}
			++nl;
		}
		else
			++i;
	}

	//Last chunk
	if (i != j) {
		double w = (l / 1000.0) * this->FontSize;
		this->Cell(w, h, s.substr(j), "", 0, "", 0, link);
	}
	
	return *this;
}

static std::string resolve_file_type ( const char * fname )
{
	//do_log ( ((std::string)"Leggo il file: " + fname + "\n").c_str () );

	FILE * fh = fopen ( fname, "rb" );

	//do_log ( "resolve_file_type: 1\n" );

	unsigned char buf [ 10 ];
	int c1, c2, c3, c4;
	if ( ! fh ) return "";

	//do_log ( "resolve_file_type: 2\n" );

	if ( fread ( buf, 7, 1, fh ) >= 1 )
	{
		do_log ( "Lettura OK!" );
		fclose ( fh );
		fh = NULL;

		char txt [ 1024 ];
		sprintf ( txt, "%x %x %x %x %x\n", buf [ 0 ], buf [ 1 ], buf [ 2 ], buf [ 3 ], buf [ 4 ] );
		do_log ( txt );

		c1 = buf [ 0 ];
		c2 = buf [ 1 ];
		c3 = buf [ 2 ];
		c4 = buf [ 3 ];

		// The fil
		if ( ( c1 == 0xFF ) && (  c2 ==  0xD8 ) ) 
		{
			do_log ( "JPG\n" );
			return "jpg";
		}
		if ( ( c2 == 'P' ) && ( c3 == 'N' ) && ( c4 == 'G' ) )
		{
			do_log ( "PNG\n" );
			return "png";
		}
	}

	if ( fh ) fclose ( fh );
	return "";
}


Pdf & Pdf::Image ( std::string const &file, double x, double y, double w, double h, std::string const &otype, std::string const &link ) throw(PdfException)
{
	ImageData info;
	//Put an image on the page
	if ( this->images.find ( file ) == this->images.end (  ) )
	{
		std::string type = otype;
		//First use of image, get info
		if ( type == "" )
		{
			type = resolve_file_type ( file.c_str () );
		} else
			std::transform ( otype.begin (  ), otype.end (  ), type.begin (  ), tolower );

		if ( type == "jpg" || type == "jpeg" )
			info = LoadJPG ( file );
		else if ( type == "png" )
			info = LoadPNG ( file );
		else
			throw PdfException ( "Unsupported image type: " ) << type;

		info.i = static_cast<int>(this->images.size() + 1);
		this->images[file] = info;
	} else
		info = this->images[file];

	//Automatic width and height calculation if needed
	if ( w == 0 && h == 0 )
	{
		//Put image at 72 dpi
		w = info.w / this->k;
		h = info.h / this->k;
	}

	if ( w == 0 )
		w = h * info.w / info.h;
	if ( h == 0 )
		h = w * info.h / info.w;

	this->_out ( fval
		     ( "q %.2f 0 0 %.2f %.2f %.2f cm /I%d Do Q", w * this->k, h * this->k, x * this->k, ( this->h - ( y + h ) ) * this->k, info.i ) );

	if ( !link.empty (  ) )
		this->ExternalLink ( x, y, w, h, link );

	return *this;
}

std::vector < char >Pdf::vread ( FILE * f, int size )
{
	std::vector < char >ris;

	int v;
	for ( int count = 0; count < size; ++count )
	{
		v = fgetc ( f );
		if ( v == EOF )
			return ris;
		ris.push_back ( v );
	}

	return ris;
}

int Pdf::iread ( FILE * f )
{
	int v = 0;
	char buf[4];
	char buf2[4];

	fread ( buf, 4, 1, f );
	
#ifdef PPC
	buf2[0] = buf[0];
	buf2[1] = buf[1];
	buf2[2] = buf[2];
	buf2[3] = buf[3];
#else
	buf2[0] = buf[3];
	buf2[1] = buf[2];
	buf2[2] = buf[1];
	buf2[3] = buf[0];
#endif

	v = *( ( int * ) buf2 );
	return v;
}

std::string Pdf::sread ( FILE * f, int size )
{
	std::string ris;

	int v;
	for ( int count = 0; count < size; ++count )
	{
		v = fgetc ( f );
		if ( v == EOF )
			return ris;
		ris += ( char ) v;
	}

	return ris;
}

Pdf & Pdf::AddFont ( FontData fdata, std::string const &ofamily, std::string const &ostyle ) throw(PdfException)
{
	//Add a TrueType or Type1 font
	std::string family = ofamily;
	std::transform ( ofamily.begin (  ), ofamily.end (  ), family.begin (  ), tolower );

	if ( family == "arial" )
		family = "helvetica";

	std::string style = ostyle;
	std::transform ( ostyle.begin (  ), ostyle.end (  ), style.begin (  ), toupper );

	if ( style == "IB" )
		style = "BI";

	std::string fontkey = family;
	fontkey += style;

	if ( this->fonts.find ( fontkey ) != this->fonts.end (  ) )
		throw PdfException ( "Font already added: " ) << family << " " << style;

	unsigned int i = static_cast<unsigned int>(this->fonts.size());
	fdata.i = i;
	this->fonts[fontkey] = fdata;

	if ( !fdata.diff.empty (  ) )
	{
		//Search existing encodings
		int d = 0;
		unsigned int nb = static_cast<unsigned int>(this->diffs.size());

		for ( unsigned int i = 0; i < nb; i++ )
		{
			if ( this->diffs[i] == fdata.diff )
			{
				d = i;
				break;
			}
		}

		if ( d == 0 )
		{
			d = static_cast<int>(this->diffs.size());
			this->diffs.push_back ( fdata.diff );
		}

		this->fonts[fontkey].diff_id = d;
	}

	if ( fdata.type == "TrueType" )
		this->FontFiles[fdata.file] = FontFileData ( fdata.originalsize, 0 );
	else
		this->FontFiles[fdata.file] = FontFileData ( fdata.size1, fdata.size2 );

	return *this;
}

Pdf & Pdf::MultiCell ( double w, double h, std::string const &txt, std::string const &border, std::string const &align, int fill ) throw(PdfException)
{
	//Output text with automatic or explicit line breaks
	if ( !this->CurrentFont )
		throw PdfException ( "No font setted up for text" );

	std::map < unsigned char, int >&cw = *( this->CurrentFont->cw );
	if ( w == 0 )
		w = this->w - this->rMargin - this->x;

	double wmax = ( w - 2 * this->cMargin ) * 1000 / this->FontSize;

	std::string s = str_replace ( "\r", "", txt );
	unsigned int nb = static_cast<unsigned int>(s.size());

	if ( ( nb > 0 ) && ( s[nb - 1] == '\n' ) )
		nb--;

	std::string b = "";
	std::string b2 = "";
	if ( !border.empty (  ) )
	{
		if ( border.find ( "L" ) != std::string::npos )
			b2 += "L";
		if ( border.find ( "R" ) != std::string::npos )
			b2 += "R";
		b = ( border.find ( "T" ) != std::string::npos ) ? b2 + "T" : b2;
	}

	int sep = -1;
	unsigned int i = 0;
	int j = 0;
	int l = 0;
	int ns = 0;
	int nl = 1;
	int ls = 0;

	while ( i < nb )
	{
		//Get next character
		char c = s[i];
		if ( c == '\n' )
		{
			//Explicit line break
			if ( this->ws > 0 )
			{
				this->ws = 0;
				this->_out ( "0 Tw" );
			}

			this->Cell ( w, h, s.substr ( j, i - j ), b, 2, align, fill );
			++i;

			sep = -1;
			j = i;
			l = 0;
			ns = 0;
			++nl;

			if ( ( !border.empty (  ) ) && nl == 2 )
				b = b2;

			continue;
		}

		if ( c == ' ' )
		{
			sep = i;
			ls = l;
			++ns;
		}
		l += cw[c];

		if ( l > wmax )
		{
			//Automatic line break
			if ( sep == -1 )
			{
				if ( i == j )
					i++;
				if ( this->ws > 0 )
				{
					this->ws = 0;
					this->_out ( "0 Tw" );
				}
				this->Cell ( w, h, s.substr ( j, i - j ), b, 2, align, fill );
			} else
			{
				if ( align == "J" )
				{
					this->ws = (float) ( ( ns > 1 ) ? ( wmax - ls ) / 1000 * this->FontSize / ( ns - 1 ) : 0 );
					this->_out ( fval ( "%.3f Tw", this->ws * this->k ) );
				}
				this->Cell ( w, h, s.substr ( j, sep - j ), b, 2, align, fill );
				i = sep + 1;
			}

			sep = -1;
			j = i;
			l = 0;
			ns = 0;
			nl++;
			if ( ( !border.empty (  ) ) && nl == 2 )
				b = b2;
		} else
			i++;
	}

	//Last chunk
	if ( this->ws > 0 )
	{
		this->ws = 0;
		this->_out ( "0 Tw" );
	}

	if ( ( !border.empty (  ) ) && ( border.find ( "B" ) != std::string::npos ) )
		b += "B";

	this->Cell ( w, h, s.substr ( j, i - j ), b, 2, align, fill );
	this->x = this->lMargin;

	return *this;
}

Pdf & Pdf::TextWithDirection ( double x, double y, std::string const &text, std::string const &direction )
{
	// std::string txt = str_replace ( ")", "\\)", str_replace ( "(", "\\(", str_replace ( "\\", "\\\\", text ) ) );
	std::string txt = _escape ( text );
	std::string s;

	if ( direction == "R" )
	{
		s = fval ( "BT %.2f %.2f %.2f %.2f", 1.0, 0.0, 0.0, 1.0 );
		s += fval ( "%.2f %.2f Tm (", x * this->k, ( this->h - y ) * this->k );
		s += txt;
		s += ") Tj ET";
	} else if ( direction == "L" )
	{
		s = fval ( "BT %.2f %.2f %.2f %.2f %.2f %.2f Tm (", -1.0, 0.0, 0.0, -1.0, x * this->k, ( this->h - y ) * this->k );
		s += txt;
		s += ") Tj ET";
	} else if ( direction == "U" )
	{
		s = fval ( "BT %.2f %.2f %.2f %.2f %.2f %.2f Tm (", 0.0, 1.0, -1.0, 0.0, x * this->k, ( this->h - y ) * this->k );
		s += txt;
		s += ") Tj ET";
	} else if ( direction == "D" )
	{
		s = fval ( "BT %.2f %.2f %.2f %.2f %.2f %.2f Tm (", 0.0, -1.0, 1.0, 0.0, x * this->k, ( this->h - y ) * this->k );
		s += txt;
		s += ") Tj ET";
	} else
	{
		s = fval ( "BT %.2f %.2f Td (", x * this->k, ( this->h - y ) * this->k );
		s += txt;
		s += ") Tj ET";
	}

	std::string tmp = s;
	if ( this->ColorFlag )
	{
		tmp = "q ";
		tmp += this->TextColor;
		tmp += " ";
		tmp += s;
		tmp += " Q";
	}

	this->_out ( tmp );
	return *this;
}

Pdf & Pdf::TextWithRotation ( double x, double y, std::string const &text, double txt_angle, double font_angle )
{
	// std::string txt = str_replace ( ")", "\\)", str_replace ( "(", "\\(", str_replace ( "\\", "\\\\", text ) ) );
	std::string txt = _escape ( text );

	font_angle += 90 + txt_angle;
	txt_angle *= M_PI / 180;
	font_angle *= M_PI / 180;

	double txt_dx = cos ( txt_angle );
	double txt_dy = sin ( txt_angle );
	double font_dx = cos ( font_angle );
	double font_dy = sin ( font_angle );

	std::string s = fval ( "BT %.2f %.2f %.2f %.2f %.2f %.2f Tm (", txt_dx, txt_dy, font_dx, font_dy, x * this->k, ( this->h - y ) * this->k );
	s += txt;
	s += ") Tj ET";

	std::string tmp = s;
	if ( this->ColorFlag )
	{
		tmp = "q ";
		tmp += this->TextColor;
		tmp += " ";
		tmp += s;
		tmp += " Q";
	}

	this->_out ( tmp );
	return *this;
}

Pdf & Pdf::RoundedRect ( double x, double y, double w, double h, double r, std::string const &style, std::string const &angle )
{
	double k = this->k;
	double hp = this->h;
	std::string op;

	if ( style == "F" )
		op = "f";
	else if ( ( style == "FD" ) || ( style == "DF" ) )
		op = "B";
	else
		op = "S";

	double MyArc = 4 / 3 * ( sqrt ( 2.0 ) - 1 );
	this->_out ( fval ( "%.2f %.2f m", ( x + r ) * k, ( hp - y ) * k ) );

	double xc = x + w - r;
	double yc = y + r;
	this->_out ( fval ( "%.2f %.2f l", xc * k, ( hp - y ) * k ) );

	if ( angle.find ( "2" ) == std::string::npos )
		this->_out ( fval ( "%.2f %.2f l", ( x + w ) * k, ( hp - y ) * k ) );
	else
		this->_Arc ( xc + r * MyArc, yc - r, xc + r, yc - r * MyArc, xc + r, yc );

	xc = x + w - r;
	yc = y + h - r;
	this->_out ( fval ( "%.2f %.2f l", ( x + w ) * k, ( hp - yc ) * k ) );
	if ( angle.find ( "3" ) == std::string::npos )
		this->_out ( fval ( "%.2f %.2f l", ( x + w ) * k, ( hp - ( y + h ) ) * k ) );
	else
		this->_Arc ( xc + r, yc + r * MyArc, xc + r * MyArc, yc + r, xc, yc + r );

	xc = x + r;
	yc = y + h - r;
	this->_out ( fval ( "%.2f %.2f l", xc * k, ( hp - ( y + h ) ) * k ) );
	if ( angle.find ( "4" ) == std::string::npos )
		this->_out ( fval ( "%.2f %.2f l", ( x ) * k, ( hp - ( y + h ) ) * k ) );
	else
		this->_Arc ( xc - r * MyArc, yc + r, xc - r, yc + r * MyArc, xc - r, yc );

	xc = x + r;
	yc = y + r;
	this->_out ( fval ( "%.2f %.2f l", ( x ) * k, ( hp - yc ) * k ) );
	if ( angle.find ( "1" ) == std::string::npos )
	{
		this->_out ( fval ( "%.2f %.2f l", ( x ) * k, ( hp - y ) * k ) );
		this->_out ( fval ( "%.2f %.2f l", ( x + r ) * k, ( hp - y ) * k ) );
	} else
		this->_Arc ( xc - r, yc - r * MyArc, xc - r * MyArc, yc - r, xc, yc - r );
	this->_out ( op );
	return *this;
}

Pdf & Pdf::_Arc ( double x1, double y1, double x2, double y2, double x3, double y3 )
{
	double h = this->h;
	this->_out ( fval ( "%.2f %.2f %.2f %.2f %.2f %.2f c ", x1 * this->k, ( h - y1 ) * this->k,
			    x2 * this->k, ( h - y2 ) * this->k, x3 * this->k, ( h - y3 ) * this->k ) );
	return *this;
}

Pdf & Pdf::Circle ( double x, double y, double r, std::string const &style )
{
	return this->Ellipse ( x, y, r, r, style );
}

Pdf & Pdf::Ellipse ( double x, double y, double rx, double ry, std::string const &style )
{
	std::string op;

	if ( style == "F" )
		op = "f";
	else if ( ( style == "FD" ) || ( style == "DF" ) )
		op = "B";
	else
		op = "S";

	double lx = 4 / 3 * ( M_SQRT2 - 1 ) * rx;
	double ly = 4 / 3 * ( M_SQRT2 - 1 ) * ry;
	double k = this->k;
	double h = this->h;

	this->_out ( fval ( "%.2f %.2f m %.2f %.2f %.2f %.2f %.2f %.2f c",
			    ( x + rx ) * k, ( h - y ) * k,
			    ( x + rx ) * k, ( h - ( y - ly ) ) * k, ( x + lx ) * k, ( h - ( y - ry ) ) * k, x * k, ( h - ( y - ry ) ) * k ) );
	this->_out ( fval ( "%.2f %.2f %.2f %.2f %.2f %.2f c",
			    ( x - lx ) * k, ( h - ( y - ry ) ) * k, ( x - rx ) * k, ( h - ( y - ly ) ) * k, ( x - rx ) * k, ( h - y ) * k ) );
	this->_out ( fval ( "%.2f %.2f %.2f %.2f %.2f %.2f c",
			    ( x - rx ) * k, ( h - ( y + ly ) ) * k, ( x - lx ) * k, ( h - ( y + ry ) ) * k, x * k, ( h - ( y + ry ) ) * k ) );
	this->_out ( fval ( "%.2f %.2f %.2f %.2f %.2f %.2f c %s",
			    ( x + lx ) * k, ( h - ( y + ry ) ) * k,
			    ( x + rx ) * k, ( h - ( y + ly ) ) * k, ( x + rx ) * k, ( h - y ) * k, op.c_str (  ) ) );
	return *this;
}
