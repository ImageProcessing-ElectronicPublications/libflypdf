#!/usr/bin/env python



from flypdf import Pdf

class my_pdf ( Pdf ):	
	def Header ( self ):	
		global title

    		#Arial bold 15
    		self.SetFont ( 'Arial', 'B', 15 )
    	
		#Calculate width of title and position
    		w=self.GetStringWidth ( title ) + 6
    		self.SetX ( ( 210 - w ) /2 )
    	
		#Colors of frame, background and text
    		self.SetDrawColor ( 0, 80, 180 )
    		self.SetFillColor ( 230, 230, 0 )
   		self.SetTextColor ( 220, 50, 50 )
    	
		#Thickness of frame ( 1 mm )
    		self.SetLineWidth ( 1 )
    	
		#Title
    		self.Cell ( w, 9, title, "", 1, 'C', 1 )
    	
		#Line break
    		self.Ln ( 10 )
	

	def Footer ( self ):

    		#Position at 1.5 cm from bottom
    		self.SetY ( -15 )
    		
		#Arial italic 8
    		self.SetFont ( 'Arial', 'I', 8 )

		#Text color in gray
    		self.SetTextColor ( 128 )
    		
		#Page number
    		self.Cell ( 0, 10, 'Page ' + str(self.PageNo( )), "0", 0, 'C' )


	def ChapterTitle ( self, num, label ):

		#Arial 12
    		self.SetFont ( 'Arial', '', 12 )
    
		#Background color
    		self.SetFillColor ( 200, 220, 255 )
    		
		#Title
    		self.Cell ( 0, 6, "Chapter $num : $label", "", 1, 'L', 1 )
    
		#Line break
    		self.Ln ( 4 )


	def ChapterBody ( self, fname ):

   		#Read text file
    		f = file ( fname )
    		lines = f.readlines ()
    		f.close () 
    		
		txt = ''.join ( lines )
		txt = txt.replace ( "(", "" ).replace ( ")", "" )
		print txt

		#Times 12
    		self.SetFont ( 'Times', '',12 )
    		
		#Output justified text
    		self.MultiCell ( 0, 5, txt )

    		#Line break
    		self.Ln ()
    
		#Mention in italics
    		self.SetFont ( '', 'I' )
    		self.Cell ( 0, 5, ' -- end of excerpt -- ' )


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
