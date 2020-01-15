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

class MyPdf : public Pdf
{
public:
	MyPdf() : Pdf("P", "mm", "Letter") {}

	void Header() {
		SetFont ( "Helvetica", "U", 20 );
		Cell(80);
		Cell(30, 10, "Title", "0", 0, "C");
		Ln(20);
	}

	void Footer() {
		SetFont ( "Helvetica", "U", 20 );
		Cell(0, 10, "Page X");
	}

	virtual ~MyPdf() {}
};

int big_test (  )
{
	MyPdf p;
	p.AddPage (  );
	p.AddFont ( p.ReadTrueTypeFont ( p.SaveReducedTrueTypeFont("broadw.ttf", "iso-8859-1"), "" ), "FuturaLightBT" );
	p.SetFont ( "FuturaLightBT", "U", 10 );
	p.Cell ( 20, 20, "Prova ISO-1: אטלעש", "TBLR" );
	p.SetFont ( "Helvetica", "U", 20 );
	p.SetTextColor(128, 5, 5);
	p.Cell ( 80, 40, "Ciao Mondo (prova) \\ (prova)", "TB" );
	p.Line ( p.GetX (  ), p.GetY (  ), p.GetX (  ) + 10, p.GetY (  ) + 10 );
	p.Rect( p.GetX()+10, p.GetY(), 20, 20 );
	p.Image ( "image.jpg", 40, 60 );
	p.Image ( "cal.png", 100, 100 );
	p.MultiCell(20, 20, "ciao\nciao\nciao\n", "TBLR");

	p.Output("big.pdf");
	return 0;
}

int small_test()
{
	Pdf p;
	p.AddPage();
    p.SetFont("Arial", "B", 16);
	p.Write(5, "Hello World");
	p.Output("small.pdf");
	return 0;
}

int main()
{
	small_test();
	big_test();
}