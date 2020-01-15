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
#ifndef _TRUETYPE_H_
#define _TRUETYPE_H_

#include <map>
#include <string>

// TrueType tables tags
#define TT_TAB_HEAD			*((TT_ULONG *)"head")
#define TT_TAB_HHEA			*((TT_ULONG *)"hhea")
#define TT_TAB_OS2			*((TT_ULONG *)"OS/2")
#define TT_TAB_POST			*((TT_ULONG *)"post")
#define TT_TAB_HMTX			*((TT_ULONG *)"hmtx")
#define TT_TAB_CMAP			*((TT_ULONG *)"cmap")
#define TT_TAB_NAME			*((TT_ULONG *)"name")
#define TT_TAB_GLYF			*((TT_ULONG *)"glyf")
#define TT_TAB_LOCA			*((TT_ULONG *)"loca")
#define TT_TAB_MAXP			*((TT_ULONG *)"maxp")
#define TT_TAB_CVT			*((TT_ULONG *)"cvt ")
#define TT_TAB_PREP			*((TT_ULONG *)"prep")
#define TT_TAB_FPGM			*((TT_ULONG *)"fpgm")

// TrueType to PDF measure transformation
#define TT2PDF(n)			((float)n * 1000.0F / font->unitsPerEm)

//Glyph table definitions
#define ARG_1_AND_2_ARE_WORDS			0x0001
#define ARGS_ARE_XY_VALUES				0x0002
#define ROUND_XY_TO_GRID				0x0004
#define WE_HAVE_A_SCALE					0x0008
#define RESERVED						0x0010
#define MORE_COMPONENTS					0x0020
#define WE_HAVE_AN_X_AND_Y_SCALE		0x0040
#define WE_HAVE_A_TWO_BY_TWO			0x0080
#define WE_HAVE_INSTRUCTIONS			0x0100
#define USE_MY_METRICS					0x0200
#define OVERLAP_COMPOUND				0x0400
#define SCALED_COMPONENT_OFFSET			0x0800
#define UNSCALED_COMPONENT_OFFSET		0x1000

#define ON_CURVE						0x01
#define X_SHORT							0x02
#define Y_SHORT							0x04
#define REPEAT							0x08
#define X_SAME							0x10
#define Y_SAME							0x20


//Characer sets
#define CHARSET_UNKNOWN					(-2)
#define CHARSET_WINANSI					(-1)
#define CHARSET_BASIC_LATIN				0
#define CHARSET_LATIN_SUPPLEMENT		1
#define CHARSET_LATIN_EXTENDED_A		2
#define CHARSET_LATIN_EXTENDED_B		3
#define CHARSET_IPA_EXTENSIONS			4
#define CHARSET_SPACING_MOD_LETTERS		5
#define CHARSET_COMB_DIACR_MARKS		6
#define CHARSET_BASIC_GREEK				7
#define CHARSET_CYRILLIC				8
#define CHARSET_ARMENIAN				9
#define CHARSET_BASIC_HEBREW			10
#define CHARSET_BASIC_ARABIC			11
#define CHARSET_SYRIAC					12
#define CHARSET_THAANA					13
#define CHARSET_DEVANAGARI				14
#define CHARSET_BENGALI					15
#define CHARSET_GURMUKHI				16
#define CHARSET_GUJARATI				17
#define CHARSET_ORIYA					18
#define CHARSET_TAMIL					19
#define CHARSET_TELUGU					20
#define CHARSET_KANNADA					21
#define CHARSET_MALAYALAM				22
#define CHARSET_SINHALA					23
#define CHARSET_THAI					24
#define CHARSET_LAO						25
#define CHARSET_TIBETAN					26
#define CHARSET_MYANMAR					27
#define CHARSET_BASIC_GEORGIAN			28
#define CHARSET_HANGUL_JAMO				29
#define CHARSET_ETHIOPIC				30
#define CHARSET_CHEROKEE				31
#define CHARSET_CANADIAN_ABORIGINAL		32
#define CHARSET_OGHAM					33
#define CHARSET_RUNIC					34
#define CHARSET_KHMER					35
#define CHARSET_MONGOLIAN				36
#define CHARSET_LATIN_EXTENDED_ADD		37
#define CHARSET_GREEK_EXTENDED			38
#define CHARSET_GENERAL_PUNCTUTATION	39
#define CHARSET_SUBSCRIPT_SUPERSCRIPT	40
#define CHARSET_CURRENCY_SYMBOLS		41
#define CHARSET_COMB_DIACR_MARKS_SYMB	42
#define CHARSET_LETTERLIKE_SYMBOLS		43
#define CHARSET_NUMBER_FORMS			44
#define CHARSET_ARROWS					45
#define CHARSET_MATH_OPERATORS			46
#define CHARSET_MISC_TECHNICAL			47
#define CHARSET_CONTROL_PICTURES		48
#define CHARSET_OCR						49
#define CHARSET_ENCLOSED_ALPHANUM		50
#define CHARSET_BOX_DRAWING				51
#define CHARSET_BLOCK_ELEMENTS			52
#define CHARSET_GEOMETRIC_SHAPES		53
#define CHARSET_MISC_SYMBOLS			54
#define CHARSET_DINGBATS				55
#define CHARSET_BRAILLE					56
#define CHARSET_CJK_RADICAL_SUPP		57
#define CHARSET_KANGXI_RADICALS			58
#define CHARSET_IDEOGRAPH_DESCR			59
#define CHARSET_CJK_SYMB_PUNCT			60
#define CHARSET_HIRAGANA				61
#define CHARSET_KATAKANA				62
#define CHARSET_BOPOMOFO				63
#define CHARSET_EXTENDED_BOPOMOFO		64
#define CHARSET_HANGUL_COMPAT_JAMO		65
#define CHARSET_CJK_MISC				66
#define CHARSET_CJK_LETTERS_MONTHS		67
#define CHARSET_CJK_COMPAT				68
#define CHARSET_CJK_EXTENSION_A_1		69
#define CHARSET_CJK_EXTENSION_A_2		70
#define CHARSET_CJK_EXTENSION_A_3		71
#define CHARSET_CJK_EXTENSION_A_4		72
#define CHARSET_CJK_IDEOGRAPHS_1		73
#define CHARSET_CJK_IDEOGRAPHS_2		74
#define CHARSET_CJK_IDEOGRAPHS_3		75
#define CHARSET_CJK_IDEOGRAPHS_4		76
#define CHARSET_CJK_IDEOGRAPHS_5		77
#define CHARSET_CJK_IDEOGRAPHS_6		78
#define CHARSET_CJK_IDEOGRAPHS_7		79
#define CHARSET_CJK_IDEOGRAPHS_8		80
#define CHARSET_CJK_IDEOGRAPHS_9		81
#define CHARSET_CJK_IDEOGRAPHS_10		82
#define CHARSET_YI_YI_RADICALS			83
#define CHARSET_HANGUL_1				84
#define CHARSET_HANGUL_2				85
#define CHARSET_HANGUL_3				86
#define CHARSET_HANGUL_4				87
#define CHARSET_HANGUL_5				88
#define CHARSET_HANGUL_6				89
#define CHARSET_SURROGATES				90
#define CHARSET_PRIVATE_AREA_1			91
#define CHARSET_PRIVATE_AREA_2			92
#define CHARSET_PRIVATE_AREA_3			93
#define CHARSET_PRIVATE_AREA_4			94
#define CHARSET_CJK_COMPAT_IDEOGRAPHS	95
#define CHARSET_ALPHA_PRESENT_FORMS		96
#define CHARSET_ARABIC_PRESENT_FORMS_A	97
#define CHARSET_COMBINING_HALF_MARKS	98
#define CHARSET_CJK_COMPAT_FORMS		99
#define CHARSET_SMALL_FORM_VARIANTS		100
#define CHARSET_ARABIC_PRESENT_FORMS_B	101
#define CHARSET_HALF_FULLWIDTH_FORMS	102
#define CHARSET_SPECIALS				103

#define CHARSET_COUNT					104


//PDFMETRICS structure
struct PDFMETRICS {

	char		fontName[256];
	int			Widths[2048];
	TT_USHORT	CharIdxToGID[2048];
	TT_USHORT	NumChars;
	TT_USHORT	FirstChar;
	TT_USHORT	LastChar;
	TT_UINT		flags;
	float		bbleft;
	float		bbright;
	float		bbtop;
	float		bbbottom;
	float		ItalicAngle;
	float		Ascent;
	float		Descent;
	float		CapHeight;
	int			underlinePosition;
	int			underlineThickness;
	float		StemV;
	bool		canEmbed;
	bool		isItalic;
	bool		isBold;
	bool		isSymbolic;

};

typedef PDFMETRICS *PPDFMETRICS;


//TTFONT structure
struct TTFONT {

	TT_USHORT		unitsPerEm;
	TT_USHORT		numGlyphs;
	TT_USHORT		EmbedType;
	TT_USHORT		FontType;
	TT_BOOL		isFixedPitch;
	TT_USHORT		FirstChar;
	TT_USHORT		LastChar;
	TT_USHORT		NumChars;
	std::vector< std::pair<std::string, unsigned long> > const * subset;
	float		ItalicAngle;
	TT_SHORT		Ascent;
	TT_SHORT		Descent;
	TT_SHORT		Weight;
	TT_SHORT		CapHeight;
	TT_SHORT		underlinePosition;
	TT_SHORT		underlineThickness;
	TT_SHORT		bbleft;
	TT_SHORT		bbright;
	TT_SHORT		bbtop;
	TT_SHORT		bbbottom;
	TT_SHORT		FamilyClass;
	bool		isSymbolic;
	char		fontName[256];
	TT_SHORT		indexToLocFormat;
	TT_ULONG		*locaOffsets;
	
	TT_USHORT		numberOfHMetrics;

	struct HMETRIC {
		TT_USHORT	advanceWidth;
		TT_SHORT	lsb;
	} *hmtx;

	struct CMAP4 {

		TT_USHORT	segCount;
		TT_USHORT	*endCount;
		TT_USHORT	*startCount;
		TT_SHORT	*idDelta;
		TT_USHORT	*idRangeOffset;
		TT_USHORT	*glyphId;

	} cmap4;

	//other stuff
	TT_USHORT		glyphIdArray[2048];		//mappatura da unicode char index a src GID
	//table pointers and lengths
	TT_PBYTE		tab_head;
	TT_DWORD		tab_head_len;
	TT_PBYTE		tab_hhea;
	TT_DWORD		tab_hhea_len;
	TT_PBYTE		tab_hmtx;
	TT_DWORD		tab_hmtx_len;
	TT_PBYTE		tab_cmap;
	TT_DWORD		tab_cmap_len;
	TT_PBYTE		tab_maxp;
	TT_DWORD		tab_maxp_len;
	TT_PBYTE		tab_glyf;
	TT_DWORD		tab_glyf_len;
	TT_PBYTE		tab_loca;
	TT_DWORD		tab_loca_len;
	TT_PBYTE		tab_cvt;
	TT_DWORD		tab_cvt_len;
	TT_PBYTE		tab_prep;
	TT_DWORD		tab_prep_len;
	TT_PBYTE		tab_fpgm;
	TT_DWORD		tab_fpgm_len;
	TT_PBYTE		tab_os2;
	TT_DWORD		tab_os2_len;
	TT_PBYTE		tab_post;
	TT_DWORD		tab_post_len;
	TT_PBYTE		tab_name;
	TT_DWORD		tab_name_len;

	//font reduction stuff
	TT_DWORD		dstGlyphCount;			//numero di glifi di mappatura dei caratteri del subset
	TT_USHORT		dstGlyphMapArray[2050];	//dest GID -> src GID (glifi relativi ai caratteri del subset)
	TT_DWORD		writtenGlyphCount;		//numero di glifi scritti in destinazione
	TT_DWORD		*writtenGlyphOffsets;	//offset di tutti i glifi di destinazione
	TT_USHORT		*writtenGlyphMapArray;	//dest GID -> src GID per TUTTI i glifi di destinazione
	TT_DWORD		writtenMaxOffset;		//offset massimo dei glifi scritti
	TT_USHORT		extraGID;				//contatore per l'allocazione dei GID extra
	TT_USHORT		*extraGlyphIDs;			//mappatura da GID sorgente a destinazione per i glifi extra
	TT_USHORT		CharIdxToGID[2048];		//mappatura da unicode char index a dest GID
	TT_SHORT		xMin;
	TT_SHORT		yMin;
	TT_SHORT		xMax;
	TT_SHORT		yMax;
	TT_USHORT		MaxAdvanceWidth;
	TT_SHORT		MaxLeftSideBearing;
	TT_USHORT		MinAdvanceWidth;
	TT_SHORT		MinLeftSideBearing;
	TT_USHORT		dstNumberOfHMetrics;
	TT_SHORT		dstIndexToLocFormat;
	TT_DWORD		dst_tab_glyf;
	TT_DWORD		dst_tab_glyf_len;

};

typedef TTFONT *PTTFONT;

//struttura per il file di output (con i font ridotti)
struct TTFILE {

	TT_PBYTE		data;
	TT_DWORD		allocated_len;
	TT_DWORD		ptr;

};

typedef TTFILE *PTTFILE;

enum SEGTYPE { segtype_undef, segtype_delta, segtype_array };


//CMAP4_SEGMENT structure
struct CMAP4_SEGMENT {

	SEGTYPE seg_type;

	TT_DWORD first_idx;
	TT_DWORD last_idx;

	struct CMAP4_SEGMENT *next;

};

struct CMAP4_SEGLIST {

	TT_DWORD		seg_count;
	CMAP4_SEGMENT	*first;

};

typedef CMAP4_SEGLIST *PCMAP4_SEGLIST;


// Unicode subsets structure
struct UNICODE_SUBSET {

	TT_USHORT		start_code;
	TT_USHORT		end_code;
	char			name[8];

};

extern UNICODE_SUBSET UnicodeSubsets[];

bool ParseTrueType(TT_PBYTE data, TT_ULONG len, PPDFMETRICS metrics, std::vector< std::pair<std::string, unsigned long> > const &subset);
bool CreateTTFile(PTTFILE file, PTTFONT font, std::string const &dest_file);
std::string ReduceTrueType(TT_PBYTE data, TT_ULONG len, std::vector< std::pair<std::string, unsigned long> > const &subset, std::string const &charset);

#endif

