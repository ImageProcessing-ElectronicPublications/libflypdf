using System;
using System.Text;
using System.IO;

using flypdf;

namespace CSharpExamples
{
    class PdfE4 : Pdf
    {
        private string title;
        private double y0;
        private int col;

        public override void Header()
        {
            this.SetFont("Arial", "B", 15);
            double w = this.GetStringWidth(title) + 6;
            this.SetX((210 - w) / 2);
            this.SetDrawColor(0, 80, 180);
            this.SetFillColor(230, 230, 0);
            this.SetTextColor(220, 50, 50);
            this.SetLineWidth(1);
            this.Cell(w, 9, title, "", 1, "C", 1);
            this.Ln(10);

            y0 = this.GetY();
        }

        public override void Footer()
        {
            this.SetY(-15);
            this.SetFont("Arial", "I", 8);
            this.SetTextColor(128);
            this.Cell(0, 10, "Page " + this.PageNo().ToString(), "0", 0, "C");
        }

        public override int AcceptPageBreak()
        {
            if ( this.col < 2 ) {
        		this.SetCol ( this.col + 1 );
        		this.SetY ( this.y0 );
        		return 0;
    		}
    		else {
        		this.SetCol(0);
                return 1;
            }
        }

	    public void SetCol ( int col )
        {
    		this.col = col;
    		float x = 10 + col * 65;
            this.SetLeftMargin(x);
            this.SetX(x);
        }

        public void ChapterTitle(int num, string label)
        {
            this.SetFont("Arial", "", 12);
            this.SetFillColor(200, 220, 255);
            this.Cell(0, 6, "Chapter " + num.ToString() + " : " + label, "", 1, "L", 1);
            this.Ln(4);

            y0 = this.GetY();
        }

        public void ChapterBody(string fname)
        {
            StreamReader tr = new StreamReader(fname);
            string txt = tr.ReadToEnd();

            txt = txt.Replace("(", "");
            txt = txt.Replace(")", "");

            this.SetFont("Times", "", 12);
            this.MultiCell(0, 5, txt);
            this.Ln();

            this.SetFont("", "I");
            this.Cell(0, 5, " -- end of excerpt -- ");

            this.SetCol(0);
        }


        public void PrintChapter(int num, string title, string file)
        {
            this.AddPage();
            this.ChapterTitle(num, title);
            this.ChapterBody(file);
        }

        public new Pdf SetTitle(string title)
        {
            this.title = title;
            return base.SetTitle(title);
        }
    }

    class Example4
    {
        public static void Run()
        {
            PdfE4 pdf = new PdfE4();
            pdf.SetTitle("20000 Leagues Under the Seas");
            pdf.SetAuthor("Jules Verne");
            pdf.PrintChapter(1, "A RUNAWAY REEF", "..\\src\\examples\\tut3.py");
            pdf.PrintChapter(2, "THE PROS AND CONS", "..\\src\\examples\\tut2.py");
            pdf.Output("example4.pdf");
        }
    }
}

