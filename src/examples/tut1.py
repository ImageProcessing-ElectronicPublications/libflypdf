#!/usr/bin/env python

import flypdf

pdf = flypdf.Pdf ()
pdf.AddPage ()
pdf.SetFont ( 'Arial', 'B', 16 )
pdf.Cell ( 40, 10, 'Hello World!' )
pdf.Output ();
