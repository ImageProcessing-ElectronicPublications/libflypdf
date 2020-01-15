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
// C++ Interface: fonts
//
// Description: 
//
//
// Author:  <amol@alex>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __FLYPDF_FONTS_H__
#define __FLYPDF_FONTS_H__

#include <string>
#include <map>
#include <fstream>

struct FontFileData
{
	int n;
	int length1;
	int length2;

	  FontFileData ( int l1, int l2 ):n ( -1 ), length1 ( l1 ), length2 ( l2 )
	{
	}
	FontFileData (  ):n ( -1 ), length1 ( 0 ), length2 ( 0 )
	{
	}
};

struct FontData
{
	int i;
	  std::string type;
	  std::string name;
	int up;
	int ut;
	  std::map < unsigned char, int >*cw;
	int n;

	  std::string desc;
	  std::string enc;
	  std::string file;
	int size1;
	int size2;
	int originalsize;
	  std::string diff;
	int diff_id;

	  FontData ( int i_, std::string const &type_, std::string const &name_, int up_, int ut_, std::map < unsigned char, int >*cw_ ):i ( i_ ),
		type ( type_ ), name ( name_ ), up ( up_ ), ut ( ut_ ), cw ( cw_ ), n ( 0 ), size1 ( 0 ), size2 ( 0 ), originalsize ( 0 ),
		diff_id ( -1 )
	{
	}

	FontData (  ):cw ( 0 ), size1 ( 0 ), size2 ( 0 ), originalsize ( 0 ), diff_id ( -1 )
	{
	}
};

#endif
