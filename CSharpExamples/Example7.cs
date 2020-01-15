using System;
using System.Collections.Generic;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class Example7
    {
        static public void Run()
        {
            Pdf p = new Pdf();
            p.AddPage();
            p.SetFont("Arial", "B", 16);
            p.TextWithDirection(40, 10, "Hello World!", "D");
            p.Output("example7.pdf");
        }
    }
}