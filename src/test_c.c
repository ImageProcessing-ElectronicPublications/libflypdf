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
#include "flypdf_c.h"

int small_test (  )
{
	Pdf_p pdfi = flypdf_newDefaultPdf (  );
	flypdf_AddPage ( pdfi, "P" );
	flypdf_SetFont ( pdfi, "Helvetica", "B", 20 );
	flypdf_Text ( pdfi, 20, 20, "Ciao mondo" );
	flypdf_Output ( pdfi, "dest_c.pdf" );
	flypdf_destroyPdf ( pdfi );

	return 0;
}

int ttf_test()
{
	Pdf_p pdfi = flypdf_newDefaultPdf (  );
	flypdf_AddPage ( pdfi, "P" );
	flypdf_AddTrueTypeFont(pdfi, "..\\src\\examples\\abalc.ttf", "", "abadi", "");
	flypdf_SetFont(pdfi, "abadi", "", 16);
	flypdf_Cell (pdfi, 40, 10, "Hello World!", "", 0, "", 0, "");
    flypdf_Output(pdfi, "example10.pdf");
	flypdf_destroyPdf(pdfi);

	return 0;
}

int main()
{
	small_test();
	ttf_test();
}
