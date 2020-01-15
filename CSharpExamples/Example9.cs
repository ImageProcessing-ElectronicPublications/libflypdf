using System;
using System.Collections.Generic;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class Example9
    {
        static public void Run()
        {
            Pdf p = new Pdf();
            p.AddPage();
            p.SetFont("Arial", "B", 16);
            p.TextWithDirection(40, 10, "Hello World! (mi blocco)", "D");
            p.Output("example9.pdf");
        }
    }
}