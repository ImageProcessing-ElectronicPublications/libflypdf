using System;
using System.Collections.Generic;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class Example10
    {
        static public void Run()
        {
            Pdf p = new Pdf();
            p.AddPage();
            p.AddTrueTypeFont("..\\src\\examples\\abalc.ttf", "abadi");
            p.SetFont("abadi", "", 16);
            p.Cell ( 40, 10, "Hello World!" );
            p.Output("example10.pdf");
        }
    }
}
