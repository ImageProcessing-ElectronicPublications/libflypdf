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

#include "images.h"
#include <stdio.h>


extern int read_JPEG_header(const char *filename, CPDFimageInfo *jInfo);


ImageData LoadJPG ( std::string const &file )
{
	CPDFimageInfo cinfo;

	do_log ( "1" );

	if ( read_JPEG_header ( file.c_str(), &cinfo ) == -1 )
	{
		throw PdfException ( "Can't open image " ) << file;
	}

	do_log ( "2" );

	ImageData data;
	data.f = "DCTDecode";
	data.w = cinfo.width;
	data.h = cinfo.height;
	data.bpc = cinfo.bitspersample;

	switch ( cinfo.ncomponents )
	{
		case 3:
			data.colspace = "DeviceRGB";
			break;

		case 4:
			data.colspace = "DeviceCMYK";
			break;

		default:
			data.colspace = "DeviceGray";
			break;
	}

	do_log ( "3" );

	FILE *infile;
	if ( ( infile = fopen ( file.c_str (  ), "rb" ) ) == 0 )
	{
		throw PdfException ( "Can't open image " ) << file;
	}

	do_log ( "4" );

	int v;
	while ( ( v = fgetc ( infile ) ) != EOF )
	{
		data.data.push_back ( v );
	}

	do_log ( "5" );

	fclose ( infile );
	return data;
}


ImageData LoadPNG ( std::string const &file )
{
	std::vector < char >png_signature;

	png_signature.push_back ( (char)137 );
	png_signature.push_back ( 'P' );
	png_signature.push_back ( 'N' );
	png_signature.push_back ( 'G' );
	png_signature.push_back ( 13 );
	png_signature.push_back ( 10 );
	png_signature.push_back ( 26 );
	png_signature.push_back ( 10 );

	//Extract info from a PNG file
	FILE *f = fopen ( file.c_str (  ), "rb" );

	if ( !f )
		throw PdfException ( "Can't open image file: " ) << file;

	//Check signature
	std::vector < char >file_signature = Pdf::vread ( f, 8 );

	if ( png_signature != file_signature )
		throw PdfException ( "Not a PNG file: " ) << file;

	//Read header chunk
	Pdf::vread ( f, 4 );
	if ( Pdf::sread ( f, 4 ) != "IHDR" )
		throw PdfException ( "Incorrect PNG file: " ) << file;

	int w = Pdf::iread ( f );
	int h = Pdf::iread ( f );
	int bpc = int ( Pdf::vread ( f, 1 )[0] );
	if ( bpc > 8 )
		throw PdfException ( "16-bit depth not supported: " ) << file;

	int ct = int ( Pdf::vread ( f, 1 )[0] );
	std::string colspace = "";

	switch ( ct )
	{
		case 0:
			colspace = "DeviceGray";
			break;

		case 2:
			colspace = "DeviceRGB";
			break;

		case 3:
			throw PdfException ( "Indexed PNG not supported: " ) << file;
			break;

		default:
			throw PdfException ( "Alpha channel not supported: " ) << file;
	}

	if ( int ( Pdf::vread ( f, 1 )[0] ) != 0 )
		throw PdfException ( "Unknown compression method: " ) << file;

	if ( int ( Pdf::vread ( f, 1 )[0] ) != 0 )
		throw PdfException ( "Unknown filter method: " ) << file;

	if ( int ( Pdf::vread ( f, 1 )[0] ) != 0 )
		throw PdfException ( "Interlacing not supported: " ) << file;

	Pdf::vread ( f, 4 );

	std::string parms =
		std::string ( "/DecodeParms <</Predictor 15 /Colors " ) + ( ct ==
									    2 ? "3" : "1" ) + " /BitsPerComponent " + Pdf::nval ( bpc ) +
		" /Columns " + Pdf::nval ( w ) + ">>";

	//Scan chunks looking for palette, transparency and image data
	std::vector < char >pal;
	std::vector < char >trns;
	std::vector < char >data;

	int n;
	do
	{
		n = Pdf::iread ( f );
		std::string type = Pdf::sread ( f, 4 );

		if ( type == "PLTE" )
		{
			//Read palette
			pal = Pdf::vread ( f, n );
			Pdf::vread ( f, 4 );
		} else if ( type == "tRNS" )
		{
			//Read transparency info
			std::vector < char >t = Pdf::vread ( f, n );

			if ( ct == 0 )
				trns.push_back ( int ( t[1] ) );
			else if ( ct == 2 )
				trns.push_back ( int ( t[1] ) );
			trns.push_back ( int ( t[3] ) );
			trns.push_back ( int ( t[5] ) );
			Pdf::vread ( f, 4 );
		} else if ( type == "IDAT" )
		{
			//Read image data block
			std::vector < char >ndata = Pdf::vread ( f, n );
			data.insert ( data.end (  ), ndata.begin (  ), ndata.end (  ) );
			Pdf::vread ( f, 4 );
		} else if ( type == "IEND" )
			break;
		else
			Pdf::vread ( f, n + 4 );
	}
	while ( n );

	fclose ( f );

	ImageData i = ImageData ( w, h, colspace, bpc, "FlateDecode", data );
	i.parms = parms;
	i.pal = pal;
	i.trns = trns;
	return i;
}
