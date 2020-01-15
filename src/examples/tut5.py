#!/usr/bin/env python


from flypdf import Pdf

class my_pdf ( Pdf ):	
	
	#Load data
	def LoadData ( self, fname ):
	
    		#Read file lines
    		lines = file ( fname, 'r' ).readlines ()

    		data = []
		for l in lines:
			data.append ( l.strip().split ( ";" ) )
    		
		return data
	

	#Simple table
	def BasicTable ( self, header, data ):

		#Header
		for col in header:
        		self.Cell (40, 7, col, "TLRB" )
    		self.Ln ()
    
		#Data
		for row in data:
			for col in row:
            			self.Cell ( 40, 6, col, "TLRB" )

        		self.Ln ()
    		
	


	#Better table
	def ImprovedTable ( self, header, data ):

		#Column widths
    		w = ( 40, 35, 40, 45 )
    
		#Header
		i = 0
    		while i < len ( w ):
			self.Cell ( w[i], 7, header[i], "TLBR", 0, 'C' )
			i += 1

    		self.Ln ()
    
		#Data
		for row in data:    	
        		self.Cell ( w[0], 6, row[0], 'LR' )
        		self.Cell ( w[1], 6, row[1], 'LR' )
        		self.Cell ( w[2], 6, "%s" % row [ 2 ], 'LR', 0, 'R' )
        		self.Cell ( w[3], 6, "%s" % row [ 3 ], 'LR', 0, 'R' )
        		self.Ln()
    
		#Closure line
    		self.Cell ( sum ( w ), 0, '', 'T' )


	#Colored table
	def FancyTable ( self, header, data ):
		w = ( 40, 35, 40, 45 )

	    	#Colors, line width and bold font
    		self.SetFillColor ( 255, 0, 0 )
	    	self.SetTextColor ( 255 )
    		self.SetDrawColor ( 128, 0, 0 )
	    	self.SetLineWidth ( .3 )
    		self.SetFont ( '', 'B' )
    	
		#Header
		i = 0
		while i < len ( w ):
        		self.Cell ( w [ i ],7, header[ i ],"TBLR",0,'C',1 );
			i += 1
	    	self.Ln () 
    	
		#Color and font restoration
	    	self.SetFillColor ( 224, 235, 255 )
	    	self.SetTextColor ( 0 ) 
	    	self.SetFont ( '' )
    	
		#Data
	    	fill = 0

		for row in data:
        		self.Cell ( w[0], 6, row[0], 'LR', 0, 'L', fill )
        		self.Cell ( w[1], 6, row[1], 'LR', 0, 'L', fill )
        		self.Cell ( w[2], 6, row[2], 'LR', 0, 'R', fill)
        		self.Cell ( w[3], 6, row[3], 'LR', 0, 'R', fill)
        		self.Ln()
        		fill != fill
    	
    		self.Cell ( sum ( w ), 0, '', 'T' )
	



pdf = my_pdf ( )

#Column titles
header = ( 'Country', 'Capital', 'Area sq km' ,'Pop. thousands' )

#Data loading
data = pdf.LoadData ( 'countries.txt' )
pdf.SetFont ( 'Arial', '', 14 )
pdf.AddPage ()
pdf.BasicTable ( header, data )
pdf.AddPage () 
pdf.ImprovedTable (header, data)
pdf.AddPage ()
pdf.FancyTable (header, data)
pdf.Output ()
