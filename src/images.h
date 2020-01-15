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
#ifndef __FLYPDF_IMAGES_H__
#define __FLYPDF_IMAGES_H__

#include <string>
#include <vector>

struct ImageData
{
	int w;
	int h;
	  std::string colspace;
	int bpc;
	  std::string f;
	  std::vector < char >data;
	int i;
	  std::string parms;
	  std::vector < char >pal;
	  std::vector < char >trns;
	int n;

	  ImageData (  ):w ( 0 ), h ( 0 ), i ( -1 ), n ( -1 )
	{
	}
	ImageData ( int w_, int h_, std::string const &colspace_, int bpc_, std::string const &f_, std::vector < char >const &data_ ):w ( w_ ),
		h ( h_ ), colspace ( colspace_ ), bpc ( bpc_ ), f ( f_ ), data ( data_ ), i ( -1 ), n ( -1 )
	{
	}
};

ImageData LoadJPG ( std::string const &file );
ImageData LoadPNG ( std::string const &file );

#endif
