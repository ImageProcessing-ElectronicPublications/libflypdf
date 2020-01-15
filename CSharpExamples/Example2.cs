using System;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class PdfE2 : Pdf
    {
        public override void Header()
        {
            this.SetFont("Arial", "B", 15);
            this.Cell(80);
            this.Cell(30, 10, "Title", "1", 0, "C");
            this.Ln(20);
        }

        public override void Footer()
        {
    		this.SetY ( -15 );
    		this.SetFont ( "Arial", "I", 8 );
    		this.Cell ( 0, 10, "Page " + this.PageNo().ToString() + "/{nb}", "", 0, "C");
        }
    }

    class Example2 : Pdf
    {
        public static void Run()
        {
            PdfE2 pdf = new PdfE2();
            pdf.SetAliasNbPages ();
            pdf.AddPage ();
            pdf.SetFont ( "Times", "", 12 );

            int i = 0;
            while ( i < 40 ) {
                pdf.Cell(0, 10, "Printing line number " + i.ToString(), "", 1);
	            i += 1;
            }

            pdf.Output("example2.pdf");
        }
    }
}
