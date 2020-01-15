#!/usr/bin/env python

import flypdf

pdf = flypdf.Pdf ()
pdf.AddPage ()
fdata = pdf.ReadTrueTypeFont ( "abalc.ttf" )
pdf.AddFont ( fdata, "abadi", "" )
pdf.SetFont ( 'abadi', '', 16 )
pdf.Cell ( 40, 10, 'Hello World!' )
pdf.Output ();
