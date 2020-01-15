#!/usr/bin/env python


from flypdf import Pdf
import re

class my_pdf ( Pdf ):
	def init (self):
		#Initialization
    		self.B = 0
    		self.I = 0
    		self.U = 0
    		self.HREF = ''
		self.status = {'B':0, 'I':0, 'U':0}

	def WriteHTML ( self, html ):
		#HTML parser
    		html = html.replace ( "\n", ' ')
    		a = re.split ( "<(.+?)>", html ) #, -1, PREG_SPLIT_DELIM_CAPTURE)
		for i, e in enumerate(a):
	        	if ( i % 2 == 0 ):        		
        			#Text
            			if self.HREF:
                			self.PutLink ( self.HREF, e )
            			else:
                			self.Write (5, e)       	
        		else:
            			#Tag
            			if  e[0]=='/':
                			self.CloseTag (e[1:].upper())
            			else:
            
	            	   		#Extract attributes
        	        		a2 = e.split(' ')
                			tag = a2[0].upper()
                			attr = {}
					for atr in a2[1:]:
						attr.update(dict([atr.split('=')]))
	                		self.OpenTag ( tag, attr )
        
	def OpenTag ( self, tag, attr ):
		#Opening tag
		if  tag == 'B' or tag == 'I' or tag=='U':
        		self.SetStyle ( tag, True )
    		if  tag == 'A':
        		self.HREF = attr['HREF'][1:-1]
    		if  tag == 'BR':
        		self.Ln ( 5 )


	def CloseTag( self, tag ):

		#Closing tag
    		if tag == 'B' or tag=='I' or tag=='U':
        		self.SetStyle ( tag, False )
    		if tag == 'A':
       			self.HREF = ''


	def SetStyle ( self, tag, enable ):
		self.status[tag] += enable and 1 or -1
    		style = ''
		for s in ('B', 'I', 'U'):
        		if ( self.status[s] > 0 ):
            			style += s
    		self.SetFont ('', style )


	def PutLink ( self, URL, txt ):

	   	#Put a hyperlink
		self.SetTextColor ( 0, 0, 255 )
    		self.SetStyle( 'U', True )
    		self.Write ( 5, txt, URL )
    		self.SetStyle ('U', False )
    		self.SetTextColor ( 0 )

html="""You can now easily print text mixing different
styles : <B>bold</B>, <I>italic</I>, <U>underlined</U>, or
<B><I><U>all at once</U></I></B>!<BR>You can also insert links
on text, such as <A HREF="http://www.fpdf.org">www.fpdf.org</A>,
or on an image: click on the logo.""";

pdf = my_pdf ( )
pdf.init()
#First page
pdf.AddPage ()
pdf.SetFont ( 'Arial', '', 20 )
pdf.Write ( 5, 'To find out what\'s new in this tutorial, click ' )
pdf.SetFont ( '', 'U' )
link=pdf.AddLink ()
pdf.Write ( 5, 'here', str(link))
pdf.SetFont ( '' )
#Second page
pdf.AddPage ()
pdf.SetLink ( link )
pdf.Image ( 'logo.png', 10, 10, 30, 0, '', 'http://www.fpdf.org' )
pdf.SetLeftMargin ( 45 )
pdf.SetFontSize ( 14 )
pdf.WriteHTML ( html )
pdf.Output ()
