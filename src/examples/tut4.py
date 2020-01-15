#!/usr/bin/env python

from flypdf import Pdf

class my_pdf ( Pdf ):	

	#Current column
	def __init__ ( self ):
		super ( my_pdf, self ).__init__ ()
		self.col = 0

	#Ordinate of column start
	def Header ( self ):

		#Page header
    		global title

    		self.SetFont ( 'Arial', 'B', 15 )
    		w = self.GetStringWidth ( title ) + 20
    		self.SetX ( ( 210 - w) /2 ) 
    		self.SetDrawColor ( 0, 80, 180 )
    		self.SetFillColor ( 230, 230, 0 )
    		self.SetTextColor ( 220, 50, 50 )
    		self.SetLineWidth ( 1 )
    		self.Cell (w, 9, title, "1", 1, 'C', 1 )
    		self.Ln ( 10 )
    		
		#Save ordinate
    		self.y0 = self.GetY()


	def Footer ( self ):

    		#Page footer
    		self.SetY ( -15 )
    		self.SetFont ( 'Arial', 'I', 8 )
    		self.SetTextColor ( 128 )
    		self.Cell ( 0, 10, 'Page ' + str(self.PageNo()), "0", 0, 'C')


	def SetCol ( self, col ):
		#Set position at a given column
    		self.col = col
    		x = 10 + col * 65
    		self.SetLeftMargin ( x )
    		self.SetX ( x )


	def AcceptPageBreak (self):
	    #Method accepting or not automatic page break
    		if ( self.col < 2 ):
    		
        		#Go to next column
        		self.SetCol ( self.col + 1 ) 
        		
			#Set ordinate to top
        		self.SetY ( self.y0 )
        		
			#Keep on page
        		return 0
    		
    		else:
    		
        		#Go back to first column
        		self.SetCol(0)
        
			#Page break
        		return 1
	

	def ChapterTitle ( self, num, label ):

    		#Title
    		self.SetFont ('Arial', '', 12 )
    		self.SetFillColor( 200, 220, 255 )
    		self.Cell( 0, 6, "Chapter $num : $label", "0", 1, 'L', 1)
   		self.Ln ( 4 )
    		
		#Save ordinate
    		self.y0 = self.GetY ()
    
   

	def ChapterBody ( self, fname ):

   		#Read text file
    		f = file ( fname )
    		lines = f.readlines ()
    		f.close () 
		
		txt = ''.join ( lines )
		txt = txt.replace ( "(", "" ).replace ( ")", "" )
		print txt
    		
		#Font
    		self.SetFont ( 'Times', '', 12 )
	
    		
		#Output text in a 6 cm width column
    		self.MultiCell ( 60, 5, txt )

    		#Line break
    		self.Ln ()
    
		#Mention in italics
    		self.SetFont ( '', 'I' )
    		self.Cell ( 0, 5, ' -- end of excerpt -- ' )
 
		#Go back to first column
    		self.SetCol ( 0 )


	
	def PrintChapter ( self, num, title, file ):

		self.AddPage ( )
    		self.ChapterTitle ( num, title )
    		self.ChapterBody ( file )


pdf = my_pdf ( )
title='20000 Leagues Under the Seas'
pdf.SetTitle ( title )
pdf.SetAuthor ( 'Jules Verne' )
pdf.PrintChapter ( 1, 'A RUNAWAY REEF', 'tut3.py' )
pdf.PrintChapter ( 2, 'THE PROS AND CONS', 'tut2.py' )
pdf.Output()
