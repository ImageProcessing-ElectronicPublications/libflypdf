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
#include <cstring>

//***********************************************
// TT_getlong
//  get a long value from the TT file
//***********************************************
static TT_LONG TT_getlong ( TT_PBYTE data )
{
	TT_LONG l;
	TT_PBYTE pl = ( TT_PBYTE )&l;

#ifdef PPC
	pl[0] = data[0];
	pl[1] = data[1];
	pl[2] = data[2];
	pl[3] = data[3];
#else
	pl[3] = data[0];
	pl[2] = data[1];
	pl[1] = data[2];
	pl[0] = data[3];
#endif

	return l;
}


//***********************************************
// TT_getshort
//  get a short value from the TT file
//***********************************************
static TT_SHORT TT_getshort(TT_PBYTE data)
{
	TT_SHORT s;
	TT_PBYTE ps = (TT_PBYTE)&s;

#ifdef PPC
	ps[0] = data[0];
	ps[1] = data[1];
#else
	ps[1] = data[0];
	ps[0] = data[1];
#endif

	return s;
}


//***********************************************
// InitTTFont
//  initialize TTFONT structure
//***********************************************
static void InitTTFont(TTFONT *font)
{
	memset(font, 0, sizeof(TTFONT));
}


//***********************************************
// GlyphIdFromCCode
//  get the glyph id from the character code
//***********************************************
static TT_USHORT GlyphIdFromCCode(TTFONT *font, TT_USHORT ucode)
{
	TT_USHORT segCount = font->cmap4.segCount;

	TT_USHORT i;
	for (i = 0; i < segCount; i++) {
		if (ucode <= font->cmap4.endCount[i]) break;
	}
	if (i == segCount) return 0;

	if (ucode < font->cmap4.startCount[i]) return 0;

	if (font->cmap4.idRangeOffset[i] == 0)
		return (TT_USHORT)((TT_SHORT)ucode + font->cmap4.idDelta[i]);

	//compute the offset into the glyph index array
	int offset = ucode - font->cmap4.startCount[i] + font->cmap4.idRangeOffset[i] / 2;
	//make the offset start from the beginning of glyph index array
	offset -= segCount - i;

	if (font->cmap4.glyphId[offset] == 0) return 0;

	return (TT_USHORT)((TT_SHORT)font->cmap4.glyphId[offset] +
		font->cmap4.idDelta[i]);
}


//***********************************************
// ComputeTTMetrics
//  fills the metrics table
//***********************************************
static bool ComputeTTMetrics(TTFONT *font, PPDFMETRICS metrics, std::vector< std::pair<std::string, unsigned long> > const &subset)
{
	memset(metrics, 0, sizeof(PDFMETRICS));

	//standard metrics
	metrics->Ascent = TT2PDF(font->Ascent);
	metrics->Descent = TT2PDF(font->Descent);
	metrics->bbleft = TT2PDF(font->bbleft);
	metrics->bbright = TT2PDF(font->bbright);
	metrics->bbtop = TT2PDF(font->bbtop);
	metrics->bbbottom = TT2PDF(font->bbbottom);
	metrics->underlinePosition = font->underlinePosition;
	metrics->underlineThickness = font->underlineThickness;
	metrics->ItalicAngle = font->ItalicAngle;

	//CapHeight if it is present, otherwise try to guess it (666)
	metrics->CapHeight = font->CapHeight ? TT2PDF(font->CapHeight) : 666;

	//StemV from Weight class
	metrics->StemV = font->Weight / 65.0F;
	metrics->StemV = metrics->StemV * metrics->StemV + 50.0F;

	//embedding license
	if (font->EmbedType == 0x0002) metrics->canEmbed = false;
	else if (font->EmbedType & 0x0200) metrics->canEmbed = false;
	else metrics->canEmbed = true;

	//font flags
	if (font->isFixedPitch) metrics->flags |= 0x00000001;

	//family class
	TT_BYTE family = (TT_BYTE)(font->FamilyClass >> 8);
	TT_BYTE subfamily = (TT_BYTE)(font->FamilyClass & 0xFF);
	switch (family) {
	case 1:		//Oldstyle Serifs
	case 2:		//Transitional Serifs
	case 3:		//Modern Serifs
	case 4:		//Clarendon Serifs
	case 5:		//Slab Serifs
	case 7:		//Freeform Serifs
		metrics->flags |= 0x00000002;
		break;
	case 8:		//Sans Serif
	case 9:		//Ornamentals
	case 10:	//Scripts
		metrics->flags |= 0x00000008;
		break;
	case 12:	//Symbolic
		metrics->flags |= 0x00000004;
		switch (subfamily) {
		case 3:		//Mixed Serif
		case 6:		//Oldstyle Serif
			metrics->flags |= 0x00000002;
			break;
		case 7:		//Neo-Grotesque Sans Serif
			break;
		}
		break;
	}
	if (metrics->StemV > 165) metrics->flags |= 0x00040000;

	if (font->isSymbolic) metrics->flags |= 0x00000004;
	metrics->isSymbolic = font->isSymbolic;

	// force Symbolic ... bah...
	if (!(metrics->flags & 0x00000004)) metrics->flags |= 0x00000020;

	//fsSelection for ItalicStyle
	if (font->FontType & 0x0001 || font->ItalicAngle != 0) {
		metrics->flags |= 0x00000040;
		metrics->isItalic = true;
	}
	//fsSelection for BoldStyle
	if (font->FontType & 0x0010) metrics->isBold = true;
	if (font->Weight >= 700) metrics->isBold = true;

	//adesso devo prelevare le advanceWidths	
	for (int i=0; i < 256; ++i) {
		TT_USHORT ucode = static_cast<TT_USHORT>(subset[i].second);
		TT_USHORT glyphId = GlyphIdFromCCode(font, ucode);
		
		if (glyphId >= font->numberOfHMetrics)
			metrics->Widths[i] = (int)TT2PDF(font->hmtx[font->numberOfHMetrics - 1].advanceWidth);
		else
			metrics->Widths[i] = (int)TT2PDF(font->hmtx[glyphId].advanceWidth);
			
		font->glyphIdArray[i] = glyphId;
	}

	//font name
	if (font->fontName[0]) strcpy(metrics->fontName, font->fontName);

	return true;
}


//***********************************************
// ParseHEADTable
//  parse a TrueType 'head' table
//***********************************************
static bool ParseHEADTable(TT_PBYTE data, PTTFONT font)
{
	//get the unitsPerEm field
	font->unitsPerEm = TT_getshort(data + 18);

	//get the bounding box
	font->bbleft = TT_getshort(data + 36);
	font->bbtop = TT_getshort(data + 38);
	font->bbright = TT_getshort(data + 40);
	font->bbbottom = TT_getshort(data + 42);

	//get the indexToLocFormat field
	font->indexToLocFormat = TT_getshort(data + 50);

	return true;
}


//***********************************************
// ParseHHEATable
//  parse a TrueType 'hhea' table
//***********************************************
static bool ParseHHEATable(TT_PBYTE data, PTTFONT font)
{
	//get the numberOfHMetrics field
	font->numberOfHMetrics = TT_getshort(data + 34);

	return true;
}


//***********************************************
// ParseOS2Table
//  parse a TrueType 'os/2' table
//***********************************************
static bool ParseOS2Table(TT_PBYTE data, PTTFONT font)
{
	//get and the version number
	TT_USHORT version = TT_getshort(data);

	//get the Ascent and Descent fields
	font->Ascent = TT_getshort(data + 68);
	font->Descent = TT_getshort(data + 70);
	//get the usWeightClass field
	font->Weight = TT_getshort(data + 4);
	//get the fsType
	font->EmbedType = TT_getshort(data + 8);
	//get the fsSelection
	font->FontType = TT_getshort(data + 62);
	//get the fsFamilyClass
	font->FamilyClass = TT_getshort(data + 32);

	if (version >= 2) {
		//get the CapHeight field
		font->CapHeight = TT_getshort(data + 88);
	}

	return true;
}


//***********************************************
// ParsePOSTTable
//  parse a TrueType 'post' table
//***********************************************
static bool ParsePOSTTable(TT_PBYTE data, PTTFONT font)
{
	//get the italicAngle
	TT_LONG ia = TT_getlong(data + 4);
	font->ItalicAngle = (float)(ia  / 65536.0F);

	//get the underlinePosition field
	font->underlinePosition = TT_getshort(data + 8);

	//get the underlineThickness field
	font->underlineThickness = TT_getshort(data + 10);

	//get the isFixedPitch field
	font->isFixedPitch = (TT_BOOL)TT_getlong(data + 12);

	return true;
}


//***********************************************
// ParseCMAP4Subtable
//  parse a TrueType 'cmap' format 4 sub-table
//***********************************************
static bool ParseCMAP4Subtable(TT_PBYTE data, PTTFONT font)
{
	//get the format number (must be 4)
	if (TT_getshort(data) != 4) return false;

	//get the subtable length
	TT_USHORT length = TT_getshort(data + 2);

	//get the segment count * sizeof(USHORT)
	TT_USHORT segCountX2 = TT_getshort(data + 6);
	TT_USHORT segCount = font->cmap4.segCount = segCountX2 / 2;

	//allocate memory for the 4 arrays
	font->cmap4.endCount = (TT_USHORT *)MemAlloc(segCountX2);
	font->cmap4.startCount = (TT_USHORT *)MemAlloc(segCountX2);
	font->cmap4.idRangeOffset = (TT_USHORT *)MemAlloc(segCountX2);
	font->cmap4.idDelta = (TT_SHORT *)MemAlloc(segCountX2);

	//allocate memory for the glyphId array
	int glen = length - 16 - 4 * segCountX2;
	if (glen < 0) return false;
	font->cmap4.glyphId = (TT_USHORT *)MemAlloc(glen);

	//get the four arrays
	TT_USHORT i;
	for (i = 0; i < segCount; i++) {
		font->cmap4.endCount[i] = TT_getshort(data + 14 + i * sizeof(TT_USHORT));
		font->cmap4.startCount[i] = TT_getshort(data + 16 + segCountX2 + i * sizeof(TT_USHORT));
		font->cmap4.idDelta[i] = TT_getshort(data + 16 + 2 * segCountX2 + i * sizeof(TT_USHORT));
		font->cmap4.idRangeOffset[i] = TT_getshort(data + 16 + 3 * segCountX2 + i * sizeof(TT_USHORT));
	}

	//get the glyphId array
	TT_USHORT gcount = static_cast<TT_USHORT>(glen / sizeof(TT_USHORT));
	for (i = 0; i < gcount; i++)
		font->cmap4.glyphId[i] = TT_getshort(data + 16 + 4 * segCountX2 + i * sizeof(TT_USHORT));

	return true;
}


//***********************************************
// ParseCMAPTable
//  parse a TrueType 'cmap' table
//***********************************************
static bool ParseCMAPTable(TT_PBYTE data, PTTFONT font)
{
	//get the number of encoding tables
	TT_USHORT encNumb = TT_getshort(data + 2);

	//find the winansi encoding table
	for (TT_USHORT t = 0; t < encNumb; t++) {
		TT_PBYTE tab = data + 4 + t * 8;

		//get platformID and encodingID
		TT_USHORT platformID = TT_getshort(tab);
		TT_USHORT encodingID = TT_getshort(tab + 2);

		//search for the symbol or unicode encoding tables
		//platformID 3 for Microsoft Encodings
		if (platformID == 3 && (encodingID == 0 || encodingID == 1)) {

			//set the symbolic font flag
			if (encodingID == 0) font->isSymbolic = true;

			//get the offset to the subtable
			TT_ULONG offset = TT_getlong(tab + 4);

			//parse the CMAP format 4 subtable
			return ParseCMAP4Subtable(data + offset, font);

		}

	}

	return false;
}


//***********************************************
// ParseHMTXTable
//  parse a TrueType 'post' table
//***********************************************
static bool ParseHMTXTable(TT_PBYTE data, PTTFONT font)
{
	//la tabella e' composta da un array di numberOfHMetrics
	//strutture contenenti le advanceWidth per il glifo
	//corrisponsente

	//alloco memoria
	font->hmtx = (TTFONT::HMETRIC *)MemAlloc(font->numberOfHMetrics * sizeof(TTFONT::HMETRIC));

	//get the hMetrics array
	for (TT_USHORT i = 0; i < font->numberOfHMetrics; i++) {
		font->hmtx[i].advanceWidth = TT_getshort(data + i * sizeof(TTFONT::HMETRIC));
		font->hmtx[i].lsb = TT_getshort(data + 2 + i * sizeof(TTFONT::HMETRIC));
	}

	return true;
}


//***********************************************
// ParseMAXPTable
//  parse a TrueType 'maxp' table
//***********************************************
static bool ParseMAXPTable(TT_PBYTE data, PTTFONT font)
{
	//get the number of glyphs
	font->numGlyphs = TT_getshort(data + 4);

	return true;
}


//***********************************************
// ParseLOCATable
//  parse a TrueType 'loca' table
//***********************************************
static bool ParseLOCATable(TT_PBYTE data, PTTFONT font)
{
	//allocate memory
	font->locaOffsets = (TT_ULONG *)MemAlloc((font->numGlyphs + 1) * sizeof(TT_ULONG));

	if (font->indexToLocFormat == 0) {

		//copy data
		for (TT_USHORT i = 0; i <= font->numGlyphs; i++) {
			font->locaOffsets[i] = (TT_ULONG)((TT_USHORT)TT_getshort(data) * 2);
			data += sizeof(TT_USHORT);
		}

	} else {

		//copy data
		for (TT_USHORT i = 0; i <= font->numGlyphs; i++) {
			font->locaOffsets[i] = TT_getlong(data);
			data += sizeof(TT_ULONG);
		}

	}
	
	return true;
}


//***********************************************
// ParseGLYFTable
//  parse a TrueType 'glyf' table
//***********************************************
static bool ParseGLYFTable(TT_PBYTE data, PTTFONT font)
{
	if (!font->CapHeight) {

		//retrieve the CapHeight parameter from
		//the bounding box height of the 'H' glyph
		//(the 'H' character is unicode 0048)
		TT_USHORT Hgid = GlyphIdFromCCode(font, 0x0048);
		if (Hgid > font->numGlyphs) return true;

		TT_PBYTE Hdata = data + font->locaOffsets[Hgid];

		TT_SHORT yMin = TT_getshort(Hdata + 4);
		TT_SHORT yMax = TT_getshort(Hdata + 8);

		font->CapHeight = yMax - yMin;

	}

	return true;
}


//***********************************************
// ParseNAMETable
//  parse a TrueType 'name' table
//***********************************************
static void _uni2ascii ( TT_WCHAR c, char * dest, int i )
{
#ifdef WIN32
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&c, 1, dest + i, 240 - i, NULL, NULL);
#else
	char buf [ 4 ];
	wchar_t wc = ( wchar_t ) c;

	wcstombs ( buf, & wc, sizeof ( buf ) );
	// * (dest + i ) =  buf [ 0 ];
	dest [ i ] = buf [ 0 ];
#endif
}

static void _get_font_name ( PTTFONT font, TT_WCHAR * string, TT_USHORT Length )
{
	int i;
	TT_WCHAR wchar = 0;

	int slen = Length / 2;
	for ( i = 0; i < slen; i++) 
	{
		wchar = TT_getshort((TT_PBYTE)(string + i));
		_uni2ascii ( wchar, font->fontName, i  );
	}
}
static bool ParseNAMETable(TT_PBYTE data, PTTFONT font)
{
	//get the number of name records
	TT_USHORT recNumb = TT_getshort(data + 2);

	//get the oofset to the string storage area
	TT_USHORT st_offset = TT_getshort(data + 4);

	//walk over the name records
	for (TT_USHORT r = 0; r < recNumb; r++) {
		TT_PBYTE rec = data + 6 + r * 12;

		//get platform id
		TT_USHORT PlatformID = TT_getshort(rec);
		TT_USHORT NameID = TT_getshort(rec + 6);
		TT_USHORT Length = TT_getshort(rec + 8);
		TT_USHORT offset = TT_getshort(rec + 10);

		TT_WCHAR *string = (TT_WCHAR *)(data + st_offset + offset);

		if (PlatformID == 2 && NameID == 1) //ISO
		{
			_get_font_name ( font, string, Length );
			break;
		} else if (PlatformID == 3 && NameID == 1) {	//Microsoft
			_get_font_name ( font, string, Length );
		}
	}

	return true;
}


//***********************************************
// FreeTTFont
//  frees allocated resoures
//***********************************************
static void FreeTTFont(TTFONT *font)
{
	//free the cmap4 arrays
	if (font->cmap4.endCount) MemFree(font->cmap4.endCount);
	if (font->cmap4.startCount) MemFree(font->cmap4.startCount);
	if (font->cmap4.idDelta) MemFree(font->cmap4.idDelta);
	if (font->cmap4.idRangeOffset) MemFree(font->cmap4.idRangeOffset);
	if (font->cmap4.glyphId) MemFree(font->cmap4.glyphId);

	//free the hmtx array
	if (font->hmtx) MemFree(font->hmtx);

	//free the loca array (it's a union so
	// it doesn't matter which pointer we pick up)
	if (font->locaOffsets) MemFree(font->locaOffsets);
	
	//free the font reduction stuff
	if (font->writtenGlyphOffsets) MemFree(font->writtenGlyphOffsets);
	if (font->extraGlyphIDs) MemFree(font->extraGlyphIDs);
	if (font->writtenGlyphMapArray) MemFree(font->writtenGlyphMapArray);
}


//***********************************************
// ParseTrueType
//  parse a TrueType font
//***********************************************
bool ParseTrueType(TT_PBYTE data, TT_ULONG len, PPDFMETRICS metrics, std::vector< std::pair<std::string, unsigned long> > const &subset)
{
	TTFONT font;
	TTFILE file;

	TT_USHORT t, TabNumb;
	// int pt = 0;

	bool retval = false;

	InitTTFont(&font);

	memset(&file, 0, sizeof(TTFILE));

	//initial sanity check
	if (len < 12) goto exit_fn;

	//version check (must be 1.0)
	if (TT_getlong(data) != 0x00010000) goto exit_fn;

	//get the number of tables
	TabNumb = TT_getshort(data + 4);

	//length check
	if ((TT_ULONG)TabNumb * 16 + 12 > len) goto exit_fn;

	//loop over the table directory
	for (t = 0; t < TabNumb; t++) {

		//point to the beginning of the directory
		TT_PBYTE dir = data + 12 + t * 16;

		//get the directory entries
		TT_ULONG tag = *((TT_ULONG *)dir);
		// ULONG checkSum = TT_getlong(dir + 4);
		TT_ULONG offset = TT_getlong(dir + 8);
		TT_ULONG length = TT_getlong(dir + 12);

		//length check
		if (offset + length > len) goto exit_fn;

		//get the tables offsets
		if (tag == TT_TAB_HEAD) font.tab_head = data + offset, font.tab_head_len = length;
		else if (tag == TT_TAB_HHEA) font.tab_hhea = data + offset, font.tab_hhea_len = length;
		else if (tag == TT_TAB_HMTX) font.tab_hmtx = data + offset, font.tab_hmtx_len = length;
		else if (tag == TT_TAB_CMAP) font.tab_cmap = data + offset, font.tab_cmap_len = length;
		else if (tag == TT_TAB_MAXP) font.tab_maxp = data + offset, font.tab_maxp_len = length;
		else if (tag == TT_TAB_GLYF) font.tab_glyf = data + offset, font.tab_glyf_len = length;
		else if (tag == TT_TAB_LOCA) font.tab_loca = data + offset, font.tab_loca_len = length;
		else if (tag == TT_TAB_CVT) font.tab_cvt = data + offset, font.tab_cvt_len = length;
		else if (tag == TT_TAB_PREP) font.tab_prep = data + offset, font.tab_prep_len = length;
		else if (tag == TT_TAB_FPGM) font.tab_fpgm = data + offset, font.tab_fpgm_len = length;
		else if (tag == TT_TAB_OS2) font.tab_os2 = data + offset, font.tab_os2_len = length;
		else if (tag == TT_TAB_POST) font.tab_post = data + offset, font.tab_post_len = length;
		else if (tag == TT_TAB_NAME) font.tab_name = data + offset, font.tab_name_len = length;

	}

	//__asm { int 3 }
	
	//check if we have collected all the required offsets
	if (!font.tab_head || !font.tab_hhea || !font.tab_os2 ||
		!font.tab_post || !font.tab_hmtx || !font.tab_cmap ||
		!font.tab_glyf || !font.tab_loca || !font.tab_maxp 
		/*|| !font.tab_cvt || !font.tab_prep || !font.tab_fpgm*/ ) goto exit_fn;

	//parse the tables
	if (!ParseHEADTable(font.tab_head, &font)) goto exit_fn;
	if (!ParseHHEATable(font.tab_hhea, &font)) goto exit_fn;
	if (!ParseOS2Table(font.tab_os2, &font)) goto exit_fn;
	if (!ParsePOSTTable(font.tab_post, &font)) goto exit_fn;
	if (!ParseCMAPTable(font.tab_cmap, &font)) goto exit_fn;
	if (!ParseHMTXTable(font.tab_hmtx, &font)) goto exit_fn;
	if (!ParseMAXPTable(font.tab_maxp, &font)) goto exit_fn;
	if (!ParseLOCATable(font.tab_loca, &font)) goto exit_fn;
	if (!ParseGLYFTable(font.tab_glyf, &font)) goto exit_fn;

	//optional table 'name'
	if (font.tab_name && !ParseNAMETable(font.tab_name, &font)) goto exit_fn;

	//finally calculate the real font metrics
	if (!ComputeTTMetrics(&font, metrics, subset)) goto exit_fn;

	//copy the unicode char to GID mapping
	memcpy(metrics->CharIdxToGID, font.CharIdxToGID, sizeof(font.CharIdxToGID));

	retval = true;

exit_fn:
	FreeTTFont(&font);
	if (file.data) MemFree(file.data);

	return retval;
}

//***********************************************
// ReduceTrueType
//  parse a TrueType font
//***********************************************
std::string ReduceTrueType(TT_PBYTE data, TT_ULONG len, std::vector< std::pair<std::string, unsigned long> > const &subset, std::string const &charset)
{
	TTFONT font;
	TTFILE file;
	PDFMETRICS metrics;

	TT_USHORT t, TabNumb;
	// int pt = 0;

	std::string retval;

	InitTTFont(&font);

	memset(&file, 0, sizeof(TTFILE));

	//initial sanity check
	if (len < 12) goto exit_fn;

	//version check (must be 1.0)
	if (TT_getlong(data) != 0x00010000) goto exit_fn;

	//get the number of tables
	TabNumb = TT_getshort(data + 4);

	//length check
	if ((TT_ULONG)TabNumb * 16 + 12 > len) goto exit_fn;

	//loop over the table directory
	for (t = 0; t < TabNumb; t++) {

		//point to the beginning of the directory
		TT_PBYTE dir = data + 12 + t * 16;

		//get the directory entries
		TT_ULONG tag = *((TT_ULONG *)dir);
		// ULONG checkSum = TT_getlong(dir + 4);
		TT_ULONG offset = TT_getlong(dir + 8);
		TT_ULONG length = TT_getlong(dir + 12);

		//length check
		if (offset + length > len) goto exit_fn;

		//get the tables offsets
		if (tag == TT_TAB_HEAD) font.tab_head = data + offset, font.tab_head_len = length;
		else if (tag == TT_TAB_HHEA) font.tab_hhea = data + offset, font.tab_hhea_len = length;
		else if (tag == TT_TAB_HMTX) font.tab_hmtx = data + offset, font.tab_hmtx_len = length;
		else if (tag == TT_TAB_CMAP) font.tab_cmap = data + offset, font.tab_cmap_len = length;
		else if (tag == TT_TAB_MAXP) font.tab_maxp = data + offset, font.tab_maxp_len = length;
		else if (tag == TT_TAB_GLYF) font.tab_glyf = data + offset, font.tab_glyf_len = length;
		else if (tag == TT_TAB_LOCA) font.tab_loca = data + offset, font.tab_loca_len = length;
		else if (tag == TT_TAB_CVT) font.tab_cvt = data + offset, font.tab_cvt_len = length;
		else if (tag == TT_TAB_PREP) font.tab_prep = data + offset, font.tab_prep_len = length;
		else if (tag == TT_TAB_FPGM) font.tab_fpgm = data + offset, font.tab_fpgm_len = length;
		else if (tag == TT_TAB_OS2) font.tab_os2 = data + offset, font.tab_os2_len = length;
		else if (tag == TT_TAB_POST) font.tab_post = data + offset, font.tab_post_len = length;
		else if (tag == TT_TAB_NAME) font.tab_name = data + offset, font.tab_name_len = length;

	}

	//__asm { int 3 }
	
	//check if we have collected all the required offsets
	if (!font.tab_head || !font.tab_hhea || !font.tab_os2 ||
		!font.tab_post || !font.tab_hmtx || !font.tab_cmap ||
		!font.tab_glyf || !font.tab_loca || !font.tab_maxp 
		/*|| !font.tab_cvt || !font.tab_prep || !font.tab_fpgm*/ ) goto exit_fn;

	//parse the tables
	if (!ParseHEADTable(font.tab_head, &font)) goto exit_fn;
	if (!ParseHHEATable(font.tab_hhea, &font)) goto exit_fn;
	if (!ParseOS2Table(font.tab_os2, &font)) goto exit_fn;
	if (!ParsePOSTTable(font.tab_post, &font)) goto exit_fn;
	if (!ParseCMAPTable(font.tab_cmap, &font)) goto exit_fn;
	if (!ParseHMTXTable(font.tab_hmtx, &font)) goto exit_fn;
	if (!ParseMAXPTable(font.tab_maxp, &font)) goto exit_fn;
	if (!ParseLOCATable(font.tab_loca, &font)) goto exit_fn;
	if (!ParseGLYFTable(font.tab_glyf, &font)) goto exit_fn;

	//optional table 'name'
	if (font.tab_name && !ParseNAMETable(font.tab_name, &font)) goto exit_fn;	
	if (!ComputeTTMetrics(&font, &metrics, subset)) goto exit_fn;
	font.NumChars = static_cast<TT_USHORT>(subset.size());
	font.subset = &subset;

	retval = font.fontName;
	retval += "-";
	retval += charset;
	retval += ".ttf";
	if(!CreateTTFile(&file, &font, retval))
		retval = "";

exit_fn:
	FreeTTFont(&font);
	if (file.data) MemFree(file.data);

	return retval;
}
