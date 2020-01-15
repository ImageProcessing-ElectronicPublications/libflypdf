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

static TT_USHORT	encWinAnsiCharset[256] = { 0 };
static TT_DWORD	encWinAnsiCharsetCount = 0;
extern TT_USHORT WinAnsiEncodings[256];

static CMAP4_SEGMENT	WinAnsiSeglist[256];
static TT_DWORD			WinAnsiSegCount = 0;

static bool initialized = false;

#define MAX_GIDS_PER_SEGMENT					7
#define TT_MBYTESIZE                 1048576

//define this for generate the TrueType font as a separate file also

#undef MemAlloc

//***********************************************
// MemAlloc
//  alloc a block of memory
//***********************************************
void * MemAlloc(TT_DWORD size)
{
	void * mem = malloc(size);
	memset(mem, 0, size);
	return mem;
}

//***********************************************
// TT_USHORTCompare
//  compare two TT_USHORT values
//  (used by the quick-sort routine)
//***********************************************
static int TT_USHORTCompare(const void *s1, const void *s2)
{
	return (int)(*((TT_USHORT *)s1) - *((TT_USHORT *)s2));
}

//***********************************************
// TT_getshort
//  get a TT_SHORT value from the TT file
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
// TTF_write
//  write binary data to a font file
//	if data is NULL then the file is filled with zeroes for len TT_BYTEs
//***********************************************
static bool TTF_write(PTTFILE file, TT_PBYTE data, TT_DWORD len)
{
	TT_DWORD newlen = file->ptr + len;
	if (newlen > file->allocated_len) {
		TT_DWORD new_alloc_len = ((newlen - 1) / TT_MBYTESIZE + 1) * TT_MBYTESIZE;
		TT_PBYTE newdata = (TT_PBYTE)MemAlloc(new_alloc_len);
		if (!newdata) return false;
		if (file->data) {
			memcpy(newdata, file->data, file->ptr);
			MemFree(file->data);
		}
		file->data = newdata;
		file->allocated_len = new_alloc_len;
	}

	if (data) {
		//copy the current data
		memcpy(file->data + file->ptr, data, len);
	}
	file->ptr += len;

	return true;
}


//***********************************************
// TTF_putlong
//  write a long value to the font fle
//***********************************************
static bool TTF_putlong(PTTFILE file, long l)
{
	TT_BYTE data[4];
	TT_PBYTE pl = (TT_PBYTE)&l;

#ifdef PPC
	data[0] = pl[0];
	data[1] = pl[1];
	data[2] = pl[2];
	data[3] = pl[3];
#else
	data[0] = pl[3];
	data[1] = pl[2];
	data[2] = pl[1];
	data[3] = pl[0];
#endif

	return TTF_write(file, data, 4);
}


//***********************************************
// TTF_putshort
//  write a TT_SHORT value to the font fle
//***********************************************
static bool TTF_putshort(PTTFILE file, TT_SHORT s)
{
	TT_BYTE data[4];
	TT_PBYTE ps = (TT_PBYTE)&s;

#ifdef PPC
	data[0] = ps[0];
	data[1] = ps[1];
#else
	data[0] = ps[1];
	data[1] = ps[0];
#endif

	return TTF_write(file, data, 2);
}


//***********************************************
// FreeCMAP4SegmentList
//  free a CMAP4 segment list
//***********************************************
static void FreeCMAP4SegmentList(PCMAP4_SEGLIST seglist)
{
	CMAP4_SEGMENT *seg = seglist->first;
	while (seg) {
		CMAP4_SEGMENT *tofree = seg;
		seg = seg->next;
		MemFree(tofree);
	}
	seglist->first = NULL;
	seglist->seg_count = 0;
}


//***********************************************
// CreateCMAP4SegmentList
//  create a CMAP4 segment list from the pre-calculated array
//***********************************************
static CMAP4_SEGLIST CreateCMAP4SegmentList()
{
	CMAP4_SEGLIST slist = { 0, NULL };
	CMAP4_SEGMENT **ppseg = &(slist.first);

	TT_DWORD count = WinAnsiSegCount;
	CMAP4_SEGMENT *curseg =  WinAnsiSeglist;
	for (TT_DWORD i = 0; i < count; i++) {

		CMAP4_SEGMENT *seg = (CMAP4_SEGMENT *)MemAlloc(sizeof(CMAP4_SEGMENT));
		*seg = *curseg;

		//add to the list
		*ppseg = seg;
		ppseg = &(seg->next);
		curseg++;
	}

	return slist;
}


//***********************************************
// InitCMAP4SegmentList
//  initialize CMAP4 segment list
//***********************************************
void PrecalcCMAP4SegmentList()
{
		TT_DWORD i = 1;
		CMAP4_SEGMENT *curseg = WinAnsiSeglist;
		TT_PDWORD psegCount = &WinAnsiSegCount;
		memset(curseg, 0, sizeof(CMAP4_SEGMENT) * 256);

		//duplico la tabella degli encoding
		TT_USHORT tempArray[256];
		TT_PUSHORT encTable = WinAnsiEncodings;
		memcpy(tempArray, encTable, 256 * sizeof(TT_USHORT));
		//la ordino
		qsort(tempArray, 256, sizeof(TT_USHORT), TT_USHORTCompare);
		//creo encCharset raggruppando tempArray
		TT_PUSHORT encCharset = encWinAnsiCharset;
		TT_DWORD encCharsetCount = 0;
		TT_USHORT last_char = tempArray[0];
		for (TT_DWORD i = 0; i < 256; i++) {
			TT_USHORT cur_char = tempArray[i];
			if (cur_char != last_char) {
				encCharset[encCharsetCount++] = last_char;
				last_char = cur_char;
			}
		}
		//put the last
		encCharset[encCharsetCount++] = last_char;

		//set the characters count
		encWinAnsiCharsetCount = encCharsetCount;

		//and now create the segmentation table

		//dapprima cerco i segmenti in cset
		//ogni segmento potra' avere una tra le seguenti caratteristiche
		//		A. sara' costituito da codici unicode progressivi
		//		B. sara' costituito da codici unicode non progressivi
		//		   con un massimo di MAX_GIDS_PER_SEGMENT di differenza
		//		   tra l'ultimo codice carattere e il primo
		bool in_segment = false;
		TT_USHORT lastChar = encCharset[0];
		bool progressive = false;
		for (i = 1; i < encCharsetCount; i++) {
			
			TT_USHORT wc = encCharset[i];
			if (wc == lastChar + 1) {

				//il carattere e' superiore di una unita'
				//rispetto al precedente
				if (in_segment) {

					//c'e' gia' un segmento iniziato
					if (progressive) {
						//il segmento e' di tipo A
						// allora lo proseguo
						lastChar = wc;
						continue;
					} else {
						//il segmento e' di tipo B
						// lo devo terminare e iniziarne uno nuovo
						// di tipo A
						curseg->last_idx = i - 2;
						//lo aggiungo alla tabella
						(*psegCount)++;
						curseg++;
					}

				}

				//devo iniziare un nuovo segmento
				// inizio un nuovo segmento di tipo A (progressivo)
				curseg->seg_type = segtype_undef;
				curseg->first_idx = i - 1;
				progressive = true;
				in_segment = true;

			} else {

				//il carattere e' superiore di piu' di una unita'
				//rispetto al precedente
				if (in_segment) {

					//c'e' gia' un segmento iniziato
					if (progressive) {
						//il segmento e' di tipo A
						// lo devo terminare
						curseg->last_idx = i - 1;
						//lo termino
						in_segment = false;
						(*psegCount)++;
						curseg++;
					} else {
						//il segmento e' di tipo B
						//controllo che la differenza tra il codice di
						//carattere corrente e quello inziale non sia
						//superiore a MAX_GIDS_PER_SEGMENT
						if (encCharset[i] - encCharset[curseg->first_idx] > MAX_GIDS_PER_SEGMENT) {
							//devo spezzarlo in due segmenti
							//termino il segmento
							curseg->last_idx = i - 1;
							in_segment = false;
							(*psegCount)++;
							curseg++;
						} else {
							// altrimenti lo proseguo
							lastChar = wc;
							continue;
						}
						continue;
					}

				} else {

					//devo iniziare un nuovo segmento
					// inizio un nuovo segmento di tipo B (non-progressivo)
					curseg->seg_type = segtype_array;
					curseg->first_idx = i - 1;
					progressive = false;

					if (encCharset[i] - encCharset[i - 1] > MAX_GIDS_PER_SEGMENT) {
						curseg->last_idx = i - 1;
						(*psegCount)++;
						curseg++;
					} else
						in_segment = true;

				}

			}

			lastChar = wc;

		}

		//completo l'ultimo segmento
		if (in_segment) {
			curseg->last_idx = i - 1;
			//e lo aggiungo alla tabella
			(*psegCount)++;
		} else if ((curseg - 1)->last_idx < encCharsetCount - 1) {
			//devo creare l'ultimo segmento
			//con l'ultimo GID
			curseg->seg_type = segtype_delta;
			curseg->first_idx = encCharsetCount - 1;
			curseg->last_idx = encCharsetCount - 1;
			(*psegCount)++;
		}
}


//***********************************************
// MakeCMAP4SubSegmentList
//  build a CMAP4 sub-segment list
//***********************************************
static CMAP4_SEGMENT *MakeCMAP4SubSegmentList(CMAP4_SEGMENT *curseg, TT_USHORT *cset, TT_BYTE *gidArray,
											  TT_DWORD count, TT_DWORD *pret_count)
{
	TT_DWORD i;
	TT_DWORD ret_count = 0;
	CMAP4_SEGMENT *last_seg = curseg->next;

	if (curseg->last_idx - curseg->first_idx == 0) {

		//il segmento ha un solo GID
		//lo considero di tipo delta
		curseg->seg_type = segtype_delta;
		ret_count = 1;
		
	} else {

		//spezzo il segmento in sotto-segmenti
		TT_DWORD first = curseg->first_idx;
		TT_DWORD last = curseg->last_idx;
		bool in_segment = true;
		if (gidArray[first + 1] == gidArray[first] + 1) curseg->seg_type = segtype_delta;
		else curseg->seg_type = segtype_array;
		TT_BYTE lastGID = gidArray[first + 1];
		for (i = first + 2; i <= last; i++) {
			
			TT_BYTE curGID = gidArray[i];
			if (curGID == lastGID + 1) {

				//il GID e' superiore di una unita'
				//rispetto al precedente
				if (in_segment) {

					//c'e' gia' un segmento iniziato
					if (curseg->seg_type == segtype_delta) {
						//il segmento e' di tipo progressivo
						// allora lo proseguo
						lastGID = curGID;
						continue;
					} else {
						//il segmento e' di tipo non-progressivo
						// lo devo terminare e iniziarne uno nuovo
						// di tipo progressivo
						curseg->last_idx = i - 2;
						if (curseg->first_idx == curseg->last_idx) curseg->seg_type = segtype_delta;
						ret_count++;

					}

				}

				//devo iniziare un nuovo segmento
				// inizio un nuovo segmento di tipo progressivo
				curseg->next = (CMAP4_SEGMENT *)MemAlloc(sizeof(CMAP4_SEGMENT));
				curseg = curseg->next;
				curseg->seg_type = segtype_delta;
				curseg->first_idx = i - 1;
				in_segment = true;

			} else {

				//il GID e' superiore di piu' di una unita'
				//rispetto al precedente
				if (in_segment) {

					//c'e' gia' un segmento iniziato
					if (curseg->seg_type == segtype_delta) {
						//il segmento e' di tipo progressivo
						// lo devo terminare
						curseg->last_idx = i - 1;
						//lo termino
						in_segment = false;
						ret_count++;
					} else {
						//il segmento e' di tipo non-progressivo
						//controllo che la differenza tra il codice di
						//carattere corrente e quello inziale non sia
						//superiore a MAX_GIDS_PER_SEGMENT
						if (cset[i] - cset[curseg->first_idx] > MAX_GIDS_PER_SEGMENT) {
							//devo spezzarlo in due segmenti
							//termino il segmento
							curseg->last_idx = i - 1;
							if (curseg->first_idx == curseg->last_idx) curseg->seg_type = segtype_delta;
							in_segment = false;
							ret_count++;
						} else {
							// altrimenti lo proseguo
							lastGID = curGID;
							continue;
						}
					}

				} else {

					//devo iniziare un nuovo segmento
					// inizio un nuovo segmento di tipo non-progressivo
					curseg->next = (CMAP4_SEGMENT *)MemAlloc(sizeof(CMAP4_SEGMENT));
					curseg = curseg->next;
					curseg->first_idx = i - 1;

					if (cset[i] - cset[i - 1] > MAX_GIDS_PER_SEGMENT) {
						curseg->last_idx = i - 1;
						curseg->seg_type = segtype_delta;
						ret_count++;
					} else {
						curseg->seg_type = segtype_array;
						in_segment = true;
					}

				}

			}

			lastGID = curGID;
		}

		//completo l'ultimo segmento
		if (in_segment) {
			curseg->last_idx = i - 1;
			ret_count++;
			if (curseg->first_idx == curseg->last_idx) curseg->seg_type = segtype_delta;
		} else if (curseg->last_idx < last) {
			//devo creare l'ultimo segmento
			//con l'ultimo GID
			curseg->next = (CMAP4_SEGMENT *)MemAlloc(sizeof(CMAP4_SEGMENT));
			curseg = curseg->next;
			curseg->seg_type = segtype_delta;
			curseg->first_idx = last;
			curseg->last_idx = last;
			ret_count++;
		}

		curseg->next = last_seg;

	}

	*pret_count = ret_count;
	return curseg;
}


//***********************************************
// MakeCMAP4SegmentList
//  build a CMAP4 segment list from an ordered
//	encoding charset table and its related
//	glyph IDs table
//***********************************************
static CMAP4_SEGLIST UpdateCMAP4SegmentList(CMAP4_SEGLIST slist, TT_USHORT *cset,
											TT_BYTE *gidArray, TT_DWORD count)
{
	TT_DWORD i, ret_count;

	//ok, ho la mia prima bella segmentazione
	//adesso me la passo tutta per cercare i sotto segmenti
	//analizzando gidArray per ogni segmento di cset che ho trovato
	CMAP4_SEGMENT **pplist = &(slist.first), *last_seg;
	while (*pplist) {

		CMAP4_SEGMENT *curseg = *pplist;
		
		//LTRIM su first_idx (salto gli indici che hanno come GID 0)
		for (i = curseg->first_idx; i <= curseg->last_idx; i++) {
			if (gidArray[i] != 0) break;
		}
		if (i > curseg->last_idx) {
			//il segmento ha tutti i GID a 0
			//lo elimino dalla lista
			*pplist = curseg->next;
			MemFree(curseg);
			continue;
		}
		curseg->first_idx = i;

		//RTRIM su last_idx
		for (i = curseg->last_idx; i >= curseg->first_idx; i--) {
			if (gidArray[i] != 0) break;
		}
		curseg->last_idx = i;

		last_seg = curseg;

		if (curseg->seg_type == segtype_undef) {
		
			last_seg = MakeCMAP4SubSegmentList(curseg, cset, gidArray, count, &ret_count);
			if (!last_seg) {
				FreeCMAP4SegmentList(&slist);
				return slist;
			}

		} else {

			//considero sempre di tipo deltaValue i segmenti
			//con un solo elemento
			if (curseg->first_idx == curseg->last_idx)
				curseg->seg_type = segtype_delta;

		}

		slist.seg_count += ret_count;

		pplist = &(last_seg->next);
	}

	//se l'ultimo segmento non finisce con il carattere 0xFFFF
	//allora ne aggiungo uno
	if (cset[last_seg->last_idx] != 0xFFFF) {
		last_seg->next = (CMAP4_SEGMENT *)MemAlloc(sizeof(CMAP4_SEGMENT));
		last_seg = last_seg->next;
		last_seg->first_idx = -1;
		last_seg->last_idx = -1;
		last_seg->seg_type = segtype_delta;
		last_seg->next = 0; //?
		slist.seg_count++;
	}

	return slist;
}


//***********************************************
// WriteCMAPTable
//  write 'cmap' table to the font file
//***********************************************
static bool WriteCMAPTable(PTTFILE file, PTTFONT font)
{
	TT_DWORD i, j;
	bool retval = false;

	TT_DWORD cmap_subtab, sub_offset, offset;
	TT_DWORD gidArrayOffset;
	TT_USHORT sublen, searchRange, entrySelector;

	//calculate the glyphIdArray
	// Ho l'array di ID dei glifi del font sorgente
	// nella struttura font.
	// Devo mappare quegli ID in quelli nuovi.
	TT_DWORD glyphCount = 0;
	//duplico l'array sorgente
	TT_USHORT tempArray[2048];
	memcpy(tempArray, font->glyphIdArray, 2048 * sizeof(TT_USHORT));
	//lo ordino
	qsort(tempArray, font->NumChars, sizeof(TT_USHORT), TT_USHORTCompare);
	
	//scrivo l'array di mappatura raggruppando gli ID uguali
	//array di mappatura da ID di destinazione a ID sorgente
	TT_USHORT mapArray[2050] = {0};
	TT_USHORT lastID = tempArray[0];
	for (i = 1; i < font->NumChars; i++) {
		TT_USHORT curID = tempArray[i];
		if (curID != lastID) {
			mapArray[glyphCount++] = lastID;
			lastID = curID;
		}
	}
	//mappo l'ultimo
	mapArray[glyphCount++] = lastID;

	//i primi due glifi devono essere 0 e 1
	if (mapArray[0] != 0) {
		memmove(mapArray + 1, mapArray, sizeof(mapArray) - sizeof(TT_USHORT));
		mapArray[0] = 0;
		glyphCount++;
	}
	if (mapArray[1] != 1) {
		memmove(mapArray + 1, mapArray, sizeof(mapArray) - sizeof(TT_USHORT));
		mapArray[1] = 1;
		glyphCount++;
	}

	//copio l'array di mappatura nella struttura font
	memcpy(font->dstGlyphMapArray, mapArray, sizeof(mapArray));
	font->dstGlyphCount = glyphCount;

	// adesso devo calcolare la segmentazione per la CMAP4
	// e' una routine molto incasinata (spero che funzioni...)
	// inutile guardare il codice non si capira' nulla....
	// in pratica consiste nell'ordinare e raggruppare
	// la tabella degli encoding (WinAnsi/Symbol o Unicode) di cui devo
	// costruire la CMAP4; questa tabella ordinata e raggruppata
	// (cioe' senza caratteri duplicati) verra' costruita in
	// encCharset e encCharsetCount.
	// poi verra' costruito encGlyphIdArray (sempre lungo encCharsetCount)
	// che conterra' gli ID dei glifi di destinazione corrispondenti
	// ai caratteri unicode della tabella encCharset.
	// In parole povere avro':
	//		encCharset[encCharsetCount]
	//			tabella contenente i caratteri unicode ordinati
	//		encGlyphIdArray[encCharsetCount]
	//			tabella contenente i rispettivi ID dei glifi di destinazione

	TT_USHORT locCharset[2048] = {0};
	TT_PUSHORT encCharset = NULL;
	TT_DWORD encCharsetCount = 256;
	TT_BYTE encGlyphIdArray[2048] = {0};
	
		//se il subset unicode richiesto e' unicode effettivo
		//allora devo prima costruire encCharset e encCharsetCount
		//(operazione molto semplice essendo un subset unicode
		// formato da caratteri consecutivi di cui conosco il
		// primo e l'ultimo!)
		encCharset = locCharset;
		//encCharsetCount = font->NumChars;
		for (i = 0; i < 256; ++i)
			locCharset[i] = static_cast<TT_USHORT>((*font->subset)[i].second);

		//adesso costruisco encGlyphIdArray
		for (i = 0; i < 256; i++) {
			TT_USHORT srcID = font->glyphIdArray[i];
			//std::cout << "Looking Corrispective of " << srcID << std::endl;
			for (j = 0; j < 256; j++) {
			//	std::cout << "Comparing Glyph " << mapArray[j] << " TO " << srcID << std::endl;
				if (mapArray[j] == srcID) break;
			}
			if (j == 256) j = 0;
			encGlyphIdArray[i] = (TT_BYTE)j;
			//std::cout << "Glyph Map: " << i << " -> " << j << std::endl;
		}

		//adesso copio l'array che ho trovato nella
		//struttura TTFONT
		memcpy(font->CharIdxToGID, encGlyphIdArray, encCharsetCount * sizeof(TT_USHORT));

	//ok... ho le due tabelle sopra citate
	// adesso le analizzo per trovare le segmentazioni
	// per la tabella CMAP4.
	CMAP4_SEGMENT *curseg;
	CMAP4_SEGLIST segList = CreateCMAP4SegmentList();
	
	segList = UpdateCMAP4SegmentList(segList, encCharset, encGlyphIdArray, encCharsetCount);
	if (!segList.first) return false;

	//costruisco anche l'array di glifi
	//di mappatura da char code a srcID che
	//verra' usato per la Macintosh Subtable
	TT_BYTE macGlyphIdArray[256];
	for (i = 0; i < 256; i++) {
		TT_USHORT srcID = font->glyphIdArray[i];
		for (j = 0; j < glyphCount; j++) {
			if (srcID == mapArray[j]) break;
		}
		if (j == glyphCount) j = 0;
		macGlyphIdArray[i] = (TT_BYTE)j;
		//copio anche nella struttura TTFONT
		font->CharIdxToGID[i] = (TT_USHORT)j;
	}

	// ho tutto quello che mi serve... e' arrivato il mometo di scrivere
	// la tabella CMAP vera e propria

	// TABLE HEADER

	//get the table offset
	offset = file->ptr;

	//table version number (0)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//number of encoding tables (2)
	if (!TTF_putshort(file, 2)) goto exit_fn;

	// SUBTABLE DIRECTORY

	//we support Microsoft and Macintosh encoding tables
	//save the offset to the subtable directory
	cmap_subtab = file->ptr;

	//move forward to the beginning of the first subtable
	if (!TTF_write(file, NULL, 16)) goto exit_fn;	// 2 subtables of 8 TT_BYTEs each

	// MICROSOFT SUBTABLE (CMAP 4)
	
	// get subtable offset
	sub_offset = file->ptr;

	// write subtable info in the subtable directory
	//move file pointer to the beginning of subtable directory entry
	file->ptr = cmap_subtab;
	//Platform ID (Microsoft = 3)
	if (!TTF_putshort(file, 3)) goto exit_fn;
	//Platform-specific encoding ID (Unicode = 1, Symbol = 0)
	if (!TTF_putshort(file, font->isSymbolic ? 0 : 1)) goto exit_fn;
	//subtable offset from the begining of the cmap table
	if (!TTF_putlong(file, sub_offset - offset)) goto exit_fn;
	//reset the file pointer to the beginning of the subtable
	file->ptr = sub_offset;
	//move the subtable offset to the beginning of the next subtable
	cmap_subtab += 8;	//8 TT_BYTEs each subtable

	// write the table
	//format (4 for Microsoft)
	if (!TTF_putshort(file, 4)) goto exit_fn;
	//length (la scrivo dopo!)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//language (0)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//segCount * 2
	if (!TTF_putshort(file, (TT_USHORT)(segList.seg_count * 2))) goto exit_fn;
	//searchRange ((maximum power of 2 <= segCount) * 2)
	searchRange = (TT_USHORT)segList.seg_count;
	entrySelector = 0;
	while (searchRange != 1) {
		searchRange = searchRange >> 1;
		entrySelector++;
	}
	searchRange = searchRange << entrySelector;
	if (!TTF_putshort(file, searchRange)) goto exit_fn;
	//entrySelector (log2(maximum power of 2 <= segCount))
	if (!TTF_putshort(file, entrySelector)) goto exit_fn;
	//rangeShift (2 * segCount - searchRange)
	if (!TTF_putshort(file, (TT_USHORT)(2 * segList.seg_count - searchRange))) goto exit_fn;

	//endCount[segCount]
	for (curseg = segList.first; curseg; curseg = curseg->next) {
		if (!TTF_putshort(file,
			curseg->last_idx == -1 ? 0xFFFF : encCharset[curseg->last_idx])) goto exit_fn;
	}

	//reservedPad (0)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	
	//startCount[segCount]
	for (curseg = segList.first; curseg; curseg = curseg->next) {
		if (!TTF_putshort(file,
			curseg->first_idx == -1 ? 0xFFFF : encCharset[curseg->first_idx])) goto exit_fn;
	}

	//idDelta[segCount]
	for (curseg = segList.first; curseg; curseg = curseg->next) {
		if (curseg->seg_type == segtype_delta) {
			if (curseg->first_idx == -1) {
				if (!TTF_putshort(file, 1)) goto exit_fn;
			} else {
				if (!TTF_putshort(file,
					encGlyphIdArray[curseg->first_idx] -
					encCharset[curseg->first_idx])) goto exit_fn;
			}
		} else {
			if (!TTF_putshort(file, 0)) goto exit_fn;
		}
	}

	//scrivo un array vuoto per trovare l'offset iniziale per
	//l'array di GID
	if (!TTF_write(file, NULL, segList.seg_count * sizeof(TT_USHORT))) goto exit_fn;

	//imposto l'offset dove iniziera' l'array di ID dei glifi
	gidArrayOffset = file->ptr;

	//riporto l'offset all'inizio dell'array idRangeOffset
	file->ptr -= segList.seg_count * sizeof(TT_USHORT);

	//idRangeOffset[segCount]
	for (curseg = segList.first; curseg; curseg = curseg->next) {
		if (curseg->seg_type == segtype_array) {

			//devo scrivere il delta tra l'offset corrente nel file
			//e l'offset dove scrivero' l'array di GID
			if (!TTF_putshort(file, (TT_USHORT)(gidArrayOffset - file->ptr))) goto exit_fn;

			//salvo l'offset corrente del file
			//per poi ritornarci
			TT_DWORD offset = file->ptr;
			//mi sposto all'offset dove devo scrivere l'array di GID
			file->ptr = gidArrayOffset;

			//scrivo l'array di GID
			TT_USHORT firstChar = encCharset[curseg->first_idx];
			TT_USHORT lastChar = encCharset[curseg->last_idx];
			TT_DWORD curidx = curseg->first_idx;
			for (TT_USHORT c = firstChar; c <= lastChar; c++) {
				
				if (c == encCharset[curidx]) {
					//se il carattere e' presente nell'intervallo
					//devo scrivere il GID corrispondente
					if (!TTF_putshort(file, encGlyphIdArray[curidx++])) goto exit_fn;
				} else {
					//per ciascun carattere fuori dall'intervallo
					//scrivo uno 0 nell'array di GID
					if (!TTF_putshort(file, 0)) goto exit_fn;
				}

			}

			//setto il nuovo offset per il prossimo
			//eventuale array di GID
			gidArrayOffset = file->ptr;
			//riporto l'offset del file alla posizione originale
			file->ptr = offset;
			
		} else {
			if (!TTF_putshort(file, 0)) goto exit_fn;
		}
	}

	//adesso devo scrivere la lunghezza della sotto-tabella
	//nell'offset 2 dall'inizio della sotto-tabella
	sublen = (TT_USHORT)(gidArrayOffset - sub_offset);
	file->ptr = sub_offset + 2;
	if (!TTF_putshort(file, sublen)) goto exit_fn;

	//posiziono l'offset del file alla fine dell'ultimo
	//array scritto (cioe' all'inizio della prossima sotto-tabella)
	file->ptr = gidArrayOffset;

	// MACINTOSH SUBTABLE (CMAP 0)

	// get subtable offset
	sub_offset = file->ptr;

	// write subtable info in the subtable directory
	//move file pointer to the beginning of subtable directory entry
	file->ptr = cmap_subtab;
	//Platform ID (Macintosh = 1)
	if (!TTF_putshort(file, 1)) goto exit_fn;
	//Platform-specific encoding ID (always 0)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//subtable offset from the begining of the cmap table
	if (!TTF_putlong(file, sub_offset - offset)) goto exit_fn;
	//reset the file pointer to the beginning of the subtable
	file->ptr = sub_offset;

	// write the table
	//format (0 for Macintosh subtable)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//length (256 TT_BYTEs array + 6 fixed TT_BYTEs = 262)
	if (!TTF_putshort(file, 262)) goto exit_fn;
	//language (0)
	if (!TTF_putshort(file, 0)) goto exit_fn;
	//glyphIdArray (256 TT_BYTEs)
	if (!TTF_write(file, macGlyphIdArray, 256)) goto exit_fn;

	retval = true;

exit_fn:
	//libero la lista di segmenti
	FreeCMAP4SegmentList(&segList);
	return retval;
}


//***********************************************
// WriteCVTTable
//  write 'cvt_' table to the font file
//***********************************************
static bool WriteCVTTable(PTTFILE file, PTTFONT font)
{
	//copio la tabella cosi' com'e'
	if (!TTF_write(file, font->tab_cvt, font->tab_cvt_len)) return false;
	return true;
}


//***********************************************
// WriteFPGMTable
//  write 'fpgm' table to the font file
//***********************************************
static bool WriteFPGMTable(PTTFILE file, PTTFONT font)
{
	//copio la tabella cosi' com'e'
	if (!TTF_write(file, font->tab_fpgm, font->tab_fpgm_len)) return false;
	return true;
}


//***********************************************
// WriteGlyph
//  write a glyph to the font file
//***********************************************
static bool WriteGlyph(PTTFILE file, PTTFONT font, TT_USHORT srcGID, TT_USHORT dstGID)
{
	TT_DWORD i;
	
	//punto all'offset del glifo nella tabella
	//originale
	if (srcGID > font->numGlyphs) return false;
	TT_DWORD srcOffset = font->locaOffsets[srcGID];
	TT_DWORD glen = font->locaOffsets[srcGID + 1] - srcOffset;

	//aggiungo l'offset del glifo nella tabella degli offset
	font->writtenGlyphOffsets[dstGID] = file->ptr - font->dst_tab_glyf;
	//calcolo anche l'offset massimo
	if (file->ptr > font->writtenMaxOffset) font->writtenMaxOffset = file->ptr;

	//?? Unused ?? TT_DWORD foo = file->ptr;

	//empty glyph
	if (glen == 0) return true;

	//punto al glifo di origine
	if (srcOffset > font->tab_glyf_len) return false;
	TT_PBYTE srcGlyph = font->tab_glyf + srcOffset;

	//recupero i dati fissi
	TT_SHORT contNumb = TT_getshort(srcGlyph);
	TT_SHORT xMin = TT_getshort(srcGlyph + 2);
	TT_SHORT yMin = TT_getshort(srcGlyph + 4);
	TT_SHORT xMax = TT_getshort(srcGlyph + 6);
	TT_SHORT yMax = TT_getshort(srcGlyph + 8);

	//scrivo cio' che ho recuperato
	if (!TTF_putshort(file, contNumb)) return false;
	if (!TTF_putshort(file, xMin)) return false;
	if (!TTF_putshort(file, yMin)) return false;
	if (!TTF_putshort(file, xMax)) return false;
	if (!TTF_putshort(file, yMax)) return false;

	srcGlyph += 10;

	if (contNumb < 0) {

		//il glifo e' composito
		TT_USHORT flags;
		do {

			flags = TT_getshort(srcGlyph);
			TT_USHORT subGID = TT_getshort(srcGlyph + 2);
			srcGlyph += 4;

			TT_USHORT locGID;
			//guardo se il glifo subGID fa parte dei glifi
			//che gia' devo aggiungere (i primi font->dstGlyphCount)
			for (i = 0; i < font->dstGlyphCount; i++) {
				if (subGID == font->dstGlyphMapArray[i]) break;
			}
			if (i < font->dstGlyphCount) {
				//c'e' gia'... non lo aggiungo
				locGID = (TT_USHORT)i;
			} else {
				//non c'e'...
				//lo recupero da quello aggiunti extra
				locGID = (TT_USHORT)font->extraGlyphIDs[subGID];
				if (!locGID) return false;
			}

			if (!TTF_putshort(file, flags)) return false;
			if (!TTF_putshort(file, locGID)) return false;

			//guradare le specifiche della tabella 'glyf' per
			//ulteriori dubbi... ehm... chiarimenti....
			if (flags & ARG_1_AND_2_ARE_WORDS) {
				if (!TTF_write(file, srcGlyph, 4)) return false;
				srcGlyph += 4;
			} else {
				if (!TTF_write(file, srcGlyph, 2)) return false;
				srcGlyph += 2;
			}
			if (flags & WE_HAVE_A_SCALE) {
				if (!TTF_write(file, srcGlyph, 2)) return false;
				srcGlyph += 2;
			} else if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
				if (!TTF_write(file, srcGlyph, 4)) return false;
				srcGlyph += 4;
			} else if (flags & WE_HAVE_A_TWO_BY_TWO) {
				if (!TTF_write(file, srcGlyph, 8)) return false;
				srcGlyph += 8;
			}
			
		} while (flags & MORE_COMPONENTS);

		if (flags & WE_HAVE_INSTRUCTIONS) {
			TT_USHORT numInstr = TT_getshort(srcGlyph);
			srcGlyph += 2;
			if (!TTF_putshort(file, numInstr)) return false;
			if (!TTF_write(file, srcGlyph, numInstr)) return false;
		}

	} else if (contNumb > 0) {

		//il glifo e' semplice
		// lo scrivo semplicemente :)
		if (!TTF_write(file, srcGlyph, glen - 10)) return false;	
		
	}

	//align to the next 2-TT_BYTE boundary
	TT_DWORD align = file->ptr % 2;
	if (align) TTF_write(file, NULL, 2 - align);

	return true;
}


//***********************************************
// AddGlyph
//  add a glyph to the glyph list
//***********************************************
static bool AddGlyph(PTTFONT font, TT_USHORT srcGID, TT_USHORT dstGID)
{
	TT_DWORD i;

	//punto all'offset del glifo nella tabella
	//originale
	if (srcGID > font->numGlyphs) return false;
	TT_DWORD srcOffset = font->locaOffsets[srcGID];
	TT_DWORD glen = font->locaOffsets[srcGID + 1] - srcOffset;

	//aggiungo il glifo nella tabella dei glifi da scrivere
	font->writtenGlyphCount++;
	font->writtenGlyphMapArray[dstGID] = srcGID;

	//empty glyph
	if (glen == 0) return true;

	//punto al glifo di origine
	if (srcOffset > font->tab_glyf_len) return false;
	TT_PBYTE srcGlyph = font->tab_glyf + srcOffset;

	//recupero i dati fissi
	TT_SHORT contNumb = TT_getshort(srcGlyph);
	TT_SHORT xMin = TT_getshort(srcGlyph + 2);
	TT_SHORT yMin = TT_getshort(srcGlyph + 4);
	TT_SHORT xMax = TT_getshort(srcGlyph + 6);
	TT_SHORT yMax = TT_getshort(srcGlyph + 8);

	//cacolo i valori minimi e massimi globali
	if (xMin < font->xMin) font->xMin = xMin;
	if (yMin < font->yMin) font->yMin = yMin;
	if (xMax > font->xMax) font->xMax = xMax;
	if (yMax > font->yMax) font->yMax = yMax;

	srcGlyph += 10;

	if (contNumb < 0) {

		//il glifo e' composito
		//devo aggiungere tutti i glifi di cui e' composto
		TT_USHORT flags;
		do {

			flags = TT_getshort(srcGlyph);
			TT_USHORT subGID = TT_getshort(srcGlyph + 2);
			srcGlyph += 4;

			TT_USHORT locGID;
			//guardo se il glifo subGID fa parte dei glifi
			//che gia' devo aggiungere (i primi font->dstGlyphCount)
			for (i = 0; i < font->dstGlyphCount; i++) {
				if (subGID == font->dstGlyphMapArray[i]) break;
			}
			if (i < font->dstGlyphCount) {
				//c'e' gia'... non lo aggiungo
				locGID = (TT_USHORT)i;
			} else {
				//non c'e'...
				//cerco se per caso l'ho gia' aggiunto...
				locGID = (TT_USHORT)font->extraGlyphIDs[subGID];
				if (!locGID) {
					//non c'e'... lo devo aggiungere
					//alloco un nuovo GID
					TT_USHORT locGID = font->extraGID++;

					//salvo il GID del glifo
					font->extraGlyphIDs[subGID] = locGID;

					//aggiungo il glifo
					if (!AddGlyph(font, subGID, locGID)) return false;
				}
			}

			//guradare le specifiche della tabella 'glyf' per
			//ulteriori dubbi... ehm... chiarimenti....
			if (flags & ARG_1_AND_2_ARE_WORDS) {
				srcGlyph += 4;
			} else {
				srcGlyph += 2;
			}
			if (flags & WE_HAVE_A_SCALE) {
				srcGlyph += 2;
			} else if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
				srcGlyph += 4;
			} else if (flags & WE_HAVE_A_TWO_BY_TWO) {
				srcGlyph += 8;
			}
			
		} while (flags & MORE_COMPONENTS);

		if (flags & WE_HAVE_INSTRUCTIONS) {
			//?? Unused ?? TT_USHORT numInstr = TT_getshort(srcGlyph);
			srcGlyph += 2;
		}

	}

	return true;
}


//***********************************************
// WriteGLYFTable
//  write 'glyf' table to the font file
//***********************************************
static bool WriteGLYFTable(PTTFILE file, PTTFONT font)
{
	//inizializzo la parte della struttura font qui utilizzata
	font->xMin = 0x7FFF;
	font->yMin = 0x7FFF;
	font->extraGID = (TT_USHORT)font->dstGlyphCount;
	font->extraGlyphIDs = (TT_USHORT *)MemAlloc(font->numGlyphs * sizeof(TT_USHORT));
	if (!font->extraGlyphIDs) return false;
	font->writtenGlyphOffsets = (TT_DWORD *)MemAlloc(font->numGlyphs * sizeof(TT_DWORD));
	if (!font->writtenGlyphOffsets) return false;
	font->writtenGlyphMapArray = (TT_USHORT *)MemAlloc(font->numGlyphs * sizeof(TT_USHORT));
	if (!font->writtenGlyphMapArray) return false;

	font->dst_tab_glyf = file->ptr;

	//aggiungo tutti i glifi
	for (TT_DWORD i = 0; i < font->dstGlyphCount; i++) {

		//recupero l'ID del glifo sorgente
		TT_USHORT srcGID = font->dstGlyphMapArray[i];

		//aggiungo il glifo
		if (!AddGlyph(font, srcGID, (TT_USHORT)i)) return false;

	}

	//scrivo i glifi sul file
	for (TT_DWORD i = 0; i < font->writtenGlyphCount; i++) {
		if (!WriteGlyph(file, font, font->writtenGlyphMapArray[i], (TT_USHORT)i)) return false;
	}

	font->dst_tab_glyf_len = file->ptr - font->dst_tab_glyf;

	return true;
}


//***********************************************
// WriteHEADTable
//  write 'head' table to the font file
//***********************************************
static bool WriteHEADTable(PTTFILE file, PTTFONT font)
{
	TT_PBYTE ptab = font->tab_head;

	//table version
	if (!TTF_putlong(file, 0x00010000)) return false;
	//font revision
	if (!TTF_write(file, ptab + 4, 4)) return false;
	//checkSumAdjustment (0 for now)
	if (!TTF_putlong(file, 0)) return false;
	//magicNumber
	if (!TTF_putlong(file, 0x5F0F3CF5)) return false;
	//flags
	if (!TTF_write(file, ptab + 16, 2)) return false;
	//unitsPerEm
	if (!TTF_putshort(file, font->unitsPerEm)) return false;
	//date created & date modified
	if (!TTF_write(file, ptab + 20, 16)) return false;
	//xMin
	if (!TTF_putshort(file, font->xMin)) return false;
	//xMax
	if (!TTF_putshort(file, font->xMax)) return false;
	//yMin
	if (!TTF_putshort(file, font->yMin)) return false;
	//yMax
	if (!TTF_putshort(file, font->yMax)) return false;
	//macStyle
	if (!TTF_write(file, ptab + 44, 2)) return false;
	//lowestRecPPEM
	if (!TTF_write(file, ptab + 46, 2)) return false;
	//fontDirectionHint
	if (!TTF_write(file, ptab + 48, 2)) return false;
	//indexToLocFormat
	// (se l'offset dei glifi massimo e' < 128K uso
	//  il metodo 0 (TT_SHORT) altrimenti il metodo 1 (LONG))
	font->dstIndexToLocFormat = font->writtenMaxOffset > 131072 ? 1 : 0;
	if (!TTF_putshort(file, font->dstIndexToLocFormat)) return false;
	//glyphDataFormat	
	if (!TTF_write(file, ptab + 52, 2)) return false;

	return true;
}


//***********************************************
// WriteHMTXTable
//  write 'hmtx' table to the font file
//***********************************************
static bool WriteHMTXTable(PTTFILE file, PTTFONT font)
{
	bool retval = false;
	TT_DWORD i;

	TT_PBYTE ptab = font->tab_hmtx;
	TT_DWORD srcNumOfHMtx = font->numberOfHMetrics;
	TT_DWORD numOfHMtx = font->writtenGlyphCount;

	TT_USHORT maxAdv = 0, minAdv = (TT_USHORT)0xFFFF;
	TT_SHORT minLsb = (TT_USHORT)0x7FFF, maxLsb = (TT_USHORT)0x8000;

	//allocate storage data for all 
	TT_USHORT *advanceWidth = (TT_USHORT *)MemAlloc(font->writtenGlyphCount * sizeof(TT_USHORT));
	TT_SHORT *lsb = (TT_SHORT *)MemAlloc(font->writtenGlyphCount * sizeof(TT_SHORT));
	if (!advanceWidth || !lsb) goto exit_fn;

	//let's start with hMetrics
	for (i = 0; i < numOfHMtx; i++) {

		//get the source GID
		TT_USHORT srcGID = font->writtenGlyphMapArray[i];
		TT_PBYTE phMtx;
		//point to the beginning of the related hMetrics structure
		if (srcGID >= srcNumOfHMtx) {
			phMtx = ptab + (srcNumOfHMtx - 1) * 4;
			advanceWidth[i] = TT_getshort(phMtx);
			lsb[i] = TT_getshort(phMtx + 4 + (srcGID - srcNumOfHMtx) * 2);
		} else {
			phMtx = ptab + srcGID * 4;
			//grab the hMetrics structure
			advanceWidth[i] = TT_getshort(phMtx);
			lsb[i] = TT_getshort(phMtx + 2);
		}

		//calcolo minimi e massimi
		if (advanceWidth[i] > maxAdv) maxAdv = advanceWidth[i];
		if (advanceWidth[i] < minAdv) minAdv = advanceWidth[i];
		if (lsb[i] > maxLsb) maxLsb = lsb[i];
		if (lsb[i] < minLsb) minLsb = lsb[i];

	}

	if (numOfHMtx > 1) {
		//adesso bisogna partire dal fondo
		//ed eliminare i valori uguali
		TT_USHORT lastAdv = advanceWidth[numOfHMtx - 1];
		for (i = numOfHMtx - 2; (int)i >= 0; i--) {
			TT_USHORT curAdv = advanceWidth[i];
			if (curAdv == lastAdv) numOfHMtx--;
			else break;
		}
	}

	//adesso scrivo le hMetrics risultanti
	for (i = 0; i < numOfHMtx; i++) {
		if (!TTF_putshort(file, advanceWidth[i])) goto exit_fn;
		if (!TTF_putshort(file, lsb[i])) goto exit_fn;
	}

	//adesso scrivo i Left Side Bearing restanti
	for (i = numOfHMtx; i < font->writtenGlyphCount; i++) {
		if (!TTF_putshort(file, lsb[i])) goto exit_fn;
	}

	//salvo minimi e massimi
	font->dstNumberOfHMetrics = (TT_USHORT)numOfHMtx;
	font->MaxAdvanceWidth = maxAdv;
	font->MinAdvanceWidth = minAdv;
	font->MaxLeftSideBearing = maxLsb;
	font->MinLeftSideBearing = minLsb;

	retval = true;
	
exit_fn:
	if (advanceWidth) MemFree(advanceWidth);
	if (lsb) MemFree(lsb);
	return retval;
}


//***********************************************
// WriteHHEATable
//  write 'hhea' table to the font file
//***********************************************
static bool WriteHHEATable(PTTFILE file, PTTFONT font)
{
	TT_PBYTE ptab = font->tab_hhea;

	//table version
	if (!TTF_putlong(file, 0x00010000)) return false;
	//Asceder, Descender, LineGap
	if (!TTF_write(file, ptab + 4, 6)) return false;
	//advanceWidthMax
	if (!TTF_putshort(file, font->MaxAdvanceWidth)) return false;
	//minLeftSideBearing
	if (!TTF_putshort(file, font->MinLeftSideBearing)) return false;
	//minRightSideBearing (Min(aw - lsb - (xMax - xMin)))
	TT_SHORT minRsb = font->MinAdvanceWidth - font->MaxLeftSideBearing - (font->xMax - font->xMin);
	if (!TTF_putshort(file, minRsb)) return false;
	//xMaxExtent (Max(lsb + (xMax - xMin)))
	if (!TTF_putshort(file, font->MaxLeftSideBearing + (font->xMax - font->xMin))) return false;
	//caretSlopeRise, caretSlopeRun, caretOffset
	//4 reserved TT_SHORTs, metricDataFormat
	if (!TTF_write(file, ptab + 18, 16)) return false;
	//numberOfHMetrics
	if (!TTF_putshort(file, font->dstNumberOfHMetrics)) return false;

	return true;
}


//***********************************************
// WriteLOCATable
//  write 'loca' table to the font file
//***********************************************
static bool WriteLOCATable(PTTFILE file, PTTFONT font)
{
	for (TT_DWORD i = 0; i < font->writtenGlyphCount; i++) {

		if (font->dstIndexToLocFormat == 0) {
			//TT_USHORT format
			TT_USHORT offset = (TT_USHORT)(font->writtenGlyphOffsets[i] / 2);
			if (!TTF_putshort(file, offset)) return false;
		} else {
			//TT_ULONG format
			TT_ULONG offset = font->writtenGlyphOffsets[i];
			if (!TTF_putlong(file, offset)) return false;
		}

	}

	//devo scrivere un elemento in piu' per
	//la lunghezza dell'ultimo glifo
	if (font->dstIndexToLocFormat == 0) {
		if (!TTF_putshort(file, (TT_USHORT)(font->dst_tab_glyf_len / 2))) return false;
	} else {
		if (!TTF_putlong(file, font->dst_tab_glyf_len)) return false;
	}

	return true;
}


//***********************************************
// WriteMAXPTable
//  write 'maxp' table to the font file
//***********************************************
static bool WriteMAXPTable(PTTFILE file, PTTFONT font)
{
	TT_PBYTE ptab = font->tab_maxp;
	TT_DWORD len = font->tab_maxp_len;

	//table version
	if (!TTF_putlong(file, 0x00010000)) return false;
	//numGlyphs
	if (!TTF_putshort(file, (TT_USHORT)font->writtenGlyphCount)) return false;
	//maxPoints, maxContours, maxCompositePoints, maxCompositeContours
	//maxZones, maxTwilightPoints, maxStorage, maxFunctionDefs,
	//maxInstructionDefs, maxStackElements, maxSizeOfInstructions
	//maxComponentElements, maxComponentDepth
	if (!TTF_write(file, ptab + 6, len - 6)) return false;
	
	return true;
}


//***********************************************
// WritePREPTable
//  write 'prep' table to the font file
//***********************************************
static bool WritePREPTable(PTTFILE file, PTTFONT font)
{
	//copio la tabella cosi' com'e'
	if (!TTF_write(file, font->tab_prep, font->tab_prep_len)) return false;
	return true;
}


//***********************************************
// CalcTableChecksum
//  calculate the checksum of a font table
//***********************************************
static TT_ULONG CalcTableChecksum(TT_PBYTE data, TT_ULONG Length)
{
	TT_ULONG *Table = (TT_ULONG *)data;

	TT_ULONG Sum = 0;
	TT_ULONG *EndPtr = Table + ((Length + 3) & ~3) / sizeof(TT_ULONG);

	while (Table < EndPtr)
		Sum += *Table++;

	return Sum;
}


//***********************************************
// WriteTableDirectoryEntry
//  writes a table directory entry
//***********************************************
static bool WriteTableDirectoryEntry(PTTFILE file, TT_DWORD tag, TT_DWORD offset, TT_DWORD length)
{
	//compute the table checksum
	TT_DWORD chkSum = CalcTableChecksum(file->data + offset, length);

	//tag
	if (!TTF_write(file, (TT_PBYTE)&tag, 4)) return false;
	//checkSum
	if (!TTF_putlong(file, chkSum)) return false;
	//offset
	if (!TTF_putlong(file, offset)) return false;
	//length
	if (!TTF_putlong(file, length)) return false;

	return true;
}


//***********************************************
// CreateTTFile
//  create a new true type file with reduced number of glyphs
//***********************************************
bool CreateTTFile(PTTFILE file, PTTFONT font, std::string const &fname)
{
	TT_DWORD cur_offset;
	TT_DWORD align;

	//1. FONT HEADER
	if (initialized == false) {
		PrecalcCMAP4SegmentList();
		initialized = true;
	}

	TT_USHORT numTables;
	numTables = 13;

	//sfnt version (1.0)
	if (!TTF_putlong(file, 0x00010000)) return false;
	//numTables (head, hhea, hmtx, cmap, glyf, loca, maxp, cvt_, prep, fpgm)
	if (!TTF_putshort(file, numTables)) return false;
	//searchRange ((maximum power of 2 <= numTables) * 16)
	if (!TTF_putshort(file, 128)) return false;
	//entrySelector (log2(maximum power of 2 <= numTables))
	if (!TTF_putshort(file, 3)) return false;
	//rangeShift (numTables * 16 - searchRange)
	if (!TTF_putshort(file, 32)) return false;

	//2. TABLE DIRECTORY
	// save the offset and leave the space for the table directory
	// we'll write this directory after writing the related tables
	TT_DWORD tabdir = file->ptr;
	if (!TTF_write(file, NULL, numTables * 16)) return false;  // numTables of 16 TT_BYTEs each

	//3. FONT TABLES

	// CMAP TABLE
	TT_DWORD tab_cmap_offset = file->ptr;
	if (!WriteCMAPTable(file, font)) return false;
	TT_DWORD tab_cmap_len = file->ptr - tab_cmap_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// CVT_ TABLE
	TT_DWORD tab_cvt_offset = file->ptr;
	if (!WriteCVTTable(file, font)) return false;
	TT_DWORD tab_cvt_len = file->ptr - tab_cvt_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// FPGM TABLE
	TT_DWORD tab_fpgm_offset = file->ptr;
	if (!WriteFPGMTable(file, font)) return false;
	TT_DWORD tab_fpgm_len = file->ptr - tab_fpgm_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// GLYF TABLE
	TT_DWORD tab_glyf_offset = file->ptr;
	if (!WriteGLYFTable(file, font)) return false;
	TT_DWORD tab_glyf_len = file->ptr - tab_glyf_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// HEAD TABLE
	TT_DWORD tab_head_offset = file->ptr;
	if (!WriteHEADTable(file, font)) return false;
	TT_DWORD tab_head_len = file->ptr - tab_head_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// HMTX TABLE
	TT_DWORD tab_hmtx_offset = file->ptr;
	if (!WriteHMTXTable(file, font)) return false;
	TT_DWORD tab_hmtx_len = file->ptr - tab_hmtx_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// HHEA TABLE
	TT_DWORD tab_hhea_offset = file->ptr;
	if (!WriteHHEATable(file, font)) return false;
	TT_DWORD tab_hhea_len = file->ptr - tab_hhea_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	// LOCA TABLE
	TT_DWORD tab_loca_offset = file->ptr;
	if (!WriteLOCATable(file, font)) return false;
	TT_DWORD tab_loca_len = file->ptr - tab_loca_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);
	
	// MAXP TABLE
	TT_DWORD tab_maxp_offset = file->ptr;
	if (!WriteMAXPTable(file, font)) return false;
	TT_DWORD tab_maxp_len = file->ptr - tab_maxp_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);
	
	// PREP TABLE
	TT_DWORD tab_prep_offset = file->ptr;
	if (!WritePREPTable(file, font)) return false;
	TT_DWORD tab_prep_len = file->ptr - tab_prep_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	TT_DWORD tab_os2_offset = file->ptr;
	if (!TTF_write(file, font->tab_os2, font->tab_os2_len)) return false;
	TT_DWORD tab_os2_len = file->ptr - tab_os2_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);
	
	TT_DWORD tab_post_offset = file->ptr;
	if (!TTF_write(file, font->tab_post, font->tab_post_len)) return false;
	TT_DWORD tab_post_len = file->ptr - tab_post_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	TT_DWORD tab_name_offset = file->ptr;
	if (!TTF_write(file, font->tab_name, font->tab_name_len)) return false;
	TT_DWORD tab_name_len = file->ptr - tab_name_offset;
	//align to the next 4-TT_BYTE boundary
	align = file->ptr % 4;
	if (align) TTF_write(file, NULL, 4 - align);

	//4. UPDATE TABLE DIRECTORY

	// write tables information in the table directory
	//save the current offset for later restore
	cur_offset = file->ptr;
	//move the file pointer to the beginning of the table directory entry
	file->ptr = tabdir;
	//write the table dirctory entry for all the tables
	//(this must be in alphabetical order)
	if (!WriteTableDirectoryEntry(file, TT_TAB_CMAP, tab_cmap_offset, tab_cmap_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_CVT, tab_cvt_offset, tab_cvt_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_FPGM, tab_fpgm_offset, tab_fpgm_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_GLYF, tab_glyf_offset, tab_glyf_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_HEAD, tab_head_offset, tab_head_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_HHEA, tab_hhea_offset, tab_hhea_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_HMTX, tab_hmtx_offset, tab_hmtx_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_LOCA, tab_loca_offset, tab_loca_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_MAXP, tab_maxp_offset, tab_maxp_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_PREP, tab_prep_offset, tab_prep_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_NAME, tab_name_offset, tab_name_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_OS2, tab_os2_offset, tab_os2_len)) return false;
	if (!WriteTableDirectoryEntry(file, TT_TAB_POST, tab_post_offset, tab_post_len)) return false;

	//restore the file pointer
	file->ptr = cur_offset;

	//5. CALCULATE AND SET FILE CHECKSUM
	TT_DWORD fileChkSum = CalcTableChecksum(file->data, file->ptr);
	file->ptr = tab_head_offset + 8;
	if (!TTF_putlong(file, 0xB1B0AFBA - fileChkSum)) return false;
	//restore the file pointer
	file->ptr = cur_offset;

	FILE *fp = fopen(fname.c_str(), "wb");
	fwrite(file->data, file->ptr, 1, fp);
	fclose(fp);

	return true;
}

