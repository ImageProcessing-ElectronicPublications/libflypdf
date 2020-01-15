#!/usr/bin/env python

import flypdf

pdf = flypdf.Pdf ()
pdf.AddPage ()
pdf.SetFont ( 'Arial', 'B', 16 )
pdf.TextWithDirection ( 40, 10, 'Hello World!', 'D' )
pdf.Output ();
