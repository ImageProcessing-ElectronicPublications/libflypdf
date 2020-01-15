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
#include <iostream>

class FontsTest : public Pdf
{
	public:
	void run() {
		std::cout << "-- CARICAMENTO TYPE1 -- " << std::endl;
		FontData fd = ReadType1Font("makefont/test_font.afm", "makefont/test_font.pfb", "iso-8859-15");
		
		std::cout << fd.desc << std::endl;
		std::cout << fd.diff << std::endl;
		std::cout << fd.size1 << std::endl;
		std::cout << fd.size2 << std::endl;
		
		std::cout << "-- CARICAMENTO TRUETYPE -- " << std::endl;
		FontData fdt = ReadTrueTypeFont("broadw.ttf", "iso-8859-15");
		
		std::cout << fdt.desc << std::endl;
		std::cout << fdt.diff << std::endl;
		std::cout << fdt.originalsize << std::endl;
		
		std::cout << "-- RIDUZIONE TRUETYPE -- " << std::endl;
		std::string reduced_font = SaveReducedTrueTypeFont("broadw.ttf", "iso-8859-1");
		FontData fdtr = ReadTrueTypeFont(reduced_font);
		
		std::cout << fdtr.desc << std::endl;
		std::cout << fdtr.diff << std::endl;
		std::cout << fdtr.originalsize << std::endl;
	}

	virtual ~FontsTest() {}
};


int main()
{
	FontsTest ft;
	ft.run();
}
