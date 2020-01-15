using System;
using System.Text;

using flypdf;

namespace CSharpExamples
{
    class Example1
    {
        static public void Run()
        {
            Pdf p = new Pdf();
            p.AddPage();
            p.SetFont("Arial", "B", 16);
            p.Cell(40, 10, "Hello World!");
            p.Output("example1.pdf");
        }
    }
}
