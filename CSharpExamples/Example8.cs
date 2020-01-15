using System;
using System.Collections.Generic;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class Example8
    {
        static public void Run()
        {
            Pdf p = new Pdf();
            p.AddPage();
            p.SetFont("Arial", "B", 16);
            p.TextWithRotation(40, 10, "(Hello) World!", 30, 40);
            p.Output("example8.pdf");
        }
    }
}