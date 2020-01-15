/*
    FlyPDF - Create PDF files on the fly! 
    Copyright (C) 2006-2008 OS3 srl

    Written by Alessandro Molina
    Portions of code by Fabio Rotondo (fabio.rotondo@os3.it)
                        Gabriele Buscone (gabriele.buscone@os3.it)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License ONLY.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "flypdf.h"

UNICODE_SUBSET UnicodeSubsets[] = {
	{0x0020, 0x007E, "BASLAT"},	// Basic Latin
	{0x00A0, 0x00FF, "LATSUP"},	// Latin-1 Supplement
	{0x0100, 0x017F, "LATEXA"},	// Latin Extended-A
	{0x0180, 0x024F, "LATEXB"},	// Latin Extended-B
	{0x0250, 0x02AF, "IPAEXT"},	// IPA Extensions
	{0x02B0, 0x02FF, "SPMLET"},	// Spacing Modifier Letters
	{0x0300, 0x036F, "CDMARK"},	// Combining Diacritical Marks
	{0x0370, 0x03FF, "BASGRE"},	// Basic Greek
	{0x0400, 0x04FF, "CYRILL"},	// Cyrillic
	{0x0530, 0x058F, "ARMENI"},	// Armenian
	{0x0590, 0x05FF, "BASHEB"},	// Basic Hebrew
	{0x0600, 0x06FF, "BASARA"},	// Basic Arabic
	{0x0700, 0x074F, "SYRIAC"},	// Syriac
	{0x0780, 0x07BF, "THAANA"},	// Thaana
	{0x0900, 0x097F, "DEVANA"},	// Devanagari
	{0x0980, 0x09FF, "BENGAL"},	// Bengali
	{0x0A00, 0x0A7F, "GURMUK"},	// Gurmukhi
	{0x0A80, 0x0AFF, "GUJARA"},	// Gujarati
	{0x0B00, 0x0B7F, "ORIYAA"},	// Oriya
	{0x0B80, 0x0BFF, "TAMILA"},	// Tamil
	{0x0C00, 0x0C7F, "TELUGU"},	// Telugu
	{0x0C80, 0x0CFF, "KANNAD"},	// Kannada
	{0x0D00, 0x0D7F, "MALAYA"},	// Malayalam
	{0x0D80, 0x0DFF, "SINHAL"},	// Sinhala
	{0x0E00, 0x0E7F, "THAIAA"},	// Thai
	{0x0E80, 0x0EFF, "LAOAAA"},	// Lao
	{0x0F00, 0x0FCF, "TIBETA"},	// Tibetan

	{0x1000, 0x109F, "MYANMR"},	// Myanmar
	{0x10A0, 0x10FF, "BASGEO"},	// Basic Georgian
	{0x1100, 0x11FF, "HNGJAM"},	// Hangul Jamo
	{0x1200, 0x12BF, "ETHIOP"},	// Ethiopic
	{0x13A0, 0x13FF, "CHEROK"},	// Cherokee
	{0x1400, 0x14DF, "CANABS"},	// Canadian Aboriginal Syllabics
	{0x1680, 0x169F, "OGHAMA"},	// Ogham
	{0x16A0, 0x16FF, "RUNICA"},	// Runic
	{0x1780, 0x17FF, "KHMERA"},	// Khmer
	{0x1800, 0x18AF, "MONGOL"},	// Mongolian
	{0x1E00, 0x1EFF, "LATEXC"},	// Latin Extended Additional
	{0x1F00, 0x1FFF, "GREEXT"},	// Greek Extended

	{0x2000, 0x206F, "GENPUN"},	// General Punctuation
	{0x2070, 0x209F, "SUBSUP"},	// Subscripts and Superscripts
	{0x20A0, 0x20CF, "CURSYM"},	// Currency Symbols
	{0x20D0, 0x20FF, "CDMRKS"},	// Combining Diacritical Marks for Symbols
	{0x2100, 0x214F, "LETSYM"},	// Letter-like Symbols
	{0x2150, 0x218F, "NUMFRM"},	// Number Forms
	{0x2190, 0x21FF, "ARROWS"},	// Arrows
	{0x2200, 0x22FF, "MATHOP"},	// Mathematical Operators
	{0x2300, 0x23FF, "MISTEC"},	// Miscellaneous Technical
	{0x2400, 0x243F, "CONPIC"},	// Control Pictures
	{0x2440, 0x245F, "OCRAAA"},	// Optical Character Recognition
	{0x2460, 0x24FF, "ENCALP"},	// Enclosed Alphanumerics
	{0x2500, 0x257F, "BOXDRW"},	// Box Drawing
	{0x2580, 0x259F, "BLKELE"},	// Block Elements
	{0x25A0, 0x25FF, "GEOSHP"},	// Geometric Shapes
	{0x2600, 0x26FF, "MISSYM"},	// Miscellaneous Symbols
	{0x2700, 0x27BF, "DINBAT"},	// Dingbats
	{0x2800, 0x28FF, "BRAILL"},	// Braille
	{0x2E80, 0x2EFF, "CJKRSP"},	// CJK Radicals Supplement
	{0x2F00, 0x2FDF, "KANRAD"},	// Kangxi Radicals
	{0x2FF0, 0x2FFF, "IDEODS"},	// Ideographic Description

	{0x3000, 0x303F, "CJKSYM"},	// Chinese, Japanese, and Korean (CJK) Symbols and Punctuation
	{0x3040, 0x309F, "HIRAGA"},	// Hiragana
	{0x30A0, 0x30FF, "KATAKA"},	// Katakana
	{0x3100, 0x312F, "BOPOMO"},	// Bopomofo
	{0x31A0, 0x31BF, "EXBOPO"},	// Extended Bopomofo
	{0x3130, 0x318F, "HANCOM"},	// Hangul Compatibility Jamo
	{0x3190, 0x319F, "CJKMIS"},	// CJK Miscellaneous
	{0x3200, 0x32FF, "CJKLAM"},	// Enclosed CJK Letters and Months
	{0x3300, 0x33FF, "CJKCOM"},	// CJK Compatibility
	
	{0x3400, 0x3BFF, "CJKIXA"},	// CJK Unified Ideograph Extension A #A
	{0x3C00, 0x43FF, "CJKIXB"},	// CJK Unified Ideograph Extension A #B
	{0x4400, 0x4BFF, "CJKIXC"},	// CJK Unified Ideograph Extension A #C
	{0x4C00, 0x4DBF, "CJKIXD"},	// CJK Unified Ideograph Extension A #D

	{0x4E00, 0x55FF, "CJKUIA"},	// CJK Unified Ideographs #A
	{0x5600, 0x5DFF, "CJKUIB"},	// CJK Unified Ideographs #B
	{0x5E00, 0x65FF, "CJKUIC"},	// CJK Unified Ideographs #C
	{0x6600, 0x6DFF, "CJKUID"},	// CJK Unified Ideographs #D
	{0x6E00, 0x75FF, "CJKUIE"},	// CJK Unified Ideographs #E
	{0x7600, 0x7DFF, "CJKUIF"},	// CJK Unified Ideographs #F
	{0x7E00, 0x85FF, "CJKUIG"},	// CJK Unified Ideographs #G
	{0x8600, 0x8DFF, "CJKUIH"},	// CJK Unified Ideographs #H
	{0x8E00, 0x95FF, "CJKUII"},	// CJK Unified Ideographs #I
	{0x9600, 0x9FFF, "CJKUIJ"},	// CJK Unified Ideographs #J

	{0xA000, 0xA48C, "YIARAD"},	// Yi / Yi Radicals

	{0xAC00, 0xB3FF, "HNGULA"},	// Hangul #A
	{0xB400, 0xBBFF, "HNGULB"},	// Hangul #B
	{0xBC00, 0xC3FF, "HNGULC"},	// Hangul #C
	{0xC400, 0xCBFF, "HNGULD"},	// Hangul #D
	{0xCC00, 0xD3FF, "HNGULE"},	// Hangul #E
	{0xD400, 0xD7A3, "HNGULF"},	// Hangul #F

	{0xD800, 0xDFFF, "SURROG"},	// Surrogates
	
	{0xE000, 0xE7FF, "PRIVAA"},	// Private Use Area #A
	{0xE800, 0xEFFF, "PRIVAB"},	// Private Use Area #B
	{0xF000, 0xF7FF, "PRIVAC"},	// Private Use Area #C
	{0xF800, 0xF8FF, "PRIVAD"},	// Private Use Area #D
	
	{0xF900, 0xFAFF, "CJKCMI"},	// CJK Compatibility Ideographs
	{0xFB00, 0xFB4F, "ALPFRM"},	// Alphabetic Presentation Forms
	{0xFB50, 0xFDFF, "ARPFRA"},	// Arabic Presentation Forms-A
	{0xFE20, 0xFE2F, "COMHMK"},	// Combining Half Marks
	{0xFE30, 0xFE4F, "CJKCMF"},	// CJK Compatibility Forms
	{0xFE50, 0xFE6F, "SMFVAR"},	// Small Form Variants
	{0xFE70, 0xFEFE, "ARPFRB"},	// Arabic Presentation Forms-B
	{0xFF00, 0xFFEF, "HFWFRM"},	// Halfwidth and Fullwidth Forms
	{0xFFF0, 0xFFFD, "SPECIA"},	// Specials

};

