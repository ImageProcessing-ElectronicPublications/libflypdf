using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using flypdf;

namespace CSharpExamples
{
    class PdfE5 : Pdf
    {
	    public List<string[]> LoadData ( string fname )
        {
            StreamReader tr = new StreamReader(fname);
            List<string> lines = new List<string>();
            string line;
            
            while ( (line = tr.ReadLine()) != null )
                lines.Add(line);

    		List<string[]> data = new List<string[]>();
            foreach(string l in lines)
                data.Add(l.Trim().Split(';'));
    		
		    return data;
        }

        public void BasicTable(string[] header, List<string[]> data)
        {
            foreach(string col in header)
                this.Cell(40, 7, col, "TLRB");
            this.Ln();

		    foreach(string[] row in data) {
                foreach(string col in row) {
                    this.Cell(40, 6, col, "TLRB");
                }
                this.Ln();
            }
        }

 	    public void ImprovedTable (string[] header, List<string[]> data )
        {
    		int[] w = new int[]{ 40, 35, 40, 45 };

            for(int i = 0; i < 4; ++i)
			    this.Cell ( w[i], 7, header[i], "TLBR", 0, "C" );
    		this.Ln ();
        
            foreach(string[] row in data) {
        		    this.Cell ( w[0], 6, row[0], "LR" );
        		    this.Cell ( w[1], 6, row[1], "LR" );
        		    this.Cell ( w[2], 6, row[2], "LR", 0, "R" );
        		    this.Cell ( w[3], 6, row[3], "LR", 0, "R" );
        		    this.Ln();
            }
        
    		this.Cell ( w[0]+w[1]+w[2]+w[3], 1, "", "TLBR" );
            this.Ln();
        }

        public void FancyTable(string[] header, List<string[]> data)
        {
            this.SetFillColor(255, 0, 0);
            this.SetTextColor(255);
            this.SetDrawColor(128, 0, 0);
            this.SetLineWidth(0.3f);
            this.SetFont("", "B");

            int[] w = new int[]{ 40, 35, 40, 45 };

            for (int i = 0; i < 4; ++i)
                this.Cell(w[i], 7, header[i], "TLBR", 0, "C", 1);
            this.Ln();

            this.SetFillColor(224, 235, 255);
            this.SetTextColor(0);
            this.SetFont("");

            int fill = 0;

            foreach (string[] row in data)
            {
                this.Cell(w[0], 6, row[0], "LR", 0, "L", fill);
                this.Cell(w[1], 6, row[1], "LR", 0, "L", fill);
                this.Cell(w[2], 6, row[2], "LR", 0, "R", fill);
                this.Cell(w[3], 6, row[3], "LR", 0, "R", fill);
                this.Ln();
                fill = (++fill % 2);
            }

            this.Cell(w[0] + w[1] + w[2] + w[3], 1, "", "TLBR");
        }
    }

    class Example5
    {
        public static void Run()
        {
            PdfE5 pdf = new PdfE5();
            string[] header = new string[] { "Country", "Capital", "Area sq km" ,"Pop. thousands" };

            List<string[]> data = pdf.LoadData ( "..\\src\\examples\\countries.txt" );
            pdf.SetFont ( "Arial", "", 14 );
            pdf.AddPage ();
            pdf.BasicTable ( header, data );
            pdf.AddPage () ;
            pdf.ImprovedTable (header, data);
            pdf.AddPage ();
            pdf.FancyTable(header, data);

            pdf.Output("example5.pdf");
        }
    }
}

