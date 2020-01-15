#!/usr/bin/env python

"""
BUGS:

1 - Supportare anche 0 ==> ''
    e 1 = "TLBR" per Cell e MultiCell ecc...
    NOTA: i numeri devono essere interi e non stringa
"""

from flypdf import Pdf
from flypdf import PdfException

class my_pdf ( Pdf ):
	def Header ( self ):
    		# Logo
		#self.Image ( 'logo_pb.png', 10, 8, 33 )

    		# Arial bold 15
    		self.SetFont ( 'Arial', 'B', 15 );

    		# Move to the right
    		self.Cell ( 80 )

    		# Title
    		self.Cell ( 30, 10, 'Title', '1', 0, 'C' );

    		# Line break
    		self.Ln ( 20 );

	def Footer ( self ):
    		# Position at 1.5 cm from bottom
    		self.SetY ( -15 );
    		# Arial italic 8
    		self.SetFont ( 'Arial', 'I', 8 );
    		# Page number
    		self.Cell ( 0, 10, 'Page ' + str(self.PageNo()) + '/{nb}', '', 0, 'C');

# Instanciation of inherited class
pdf = my_pdf ()
pdf.SetAliasNbPages ()
pdf.AddPage ()
pdf.SetFont ( 'Times', '', 12 )

i = 0
while ( i < 40 ):
	pdf.Cell ( 0, 10, 'Printing line number %s' % i, '', 1 )
	i += 1

pdf.Output ()