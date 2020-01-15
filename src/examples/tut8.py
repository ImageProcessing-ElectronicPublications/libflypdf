#!/usr/bin/env python

import flypdf

pdf = flypdf.Pdf ()
pdf.AddPage ()
pdf.SetFont ( 'Arial', 'B', 16 )
pdf.TextWithRotation ( 40, 10, '(Hello) World!', 30, 40 )
pdf.Output ();
