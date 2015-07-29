/*
 * LZW decoder
 *
 * - Uses fixed length 12-bit encodings.
 * - Expects input in MSB format.
 * - When encoding table fills up, then table is reset back to the initial
 *   256 entries.
 * - Written in C89 style.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static void decode(FILE *in, FILE *out);

int main(int argc, char *argv[])
{
    FILE *in  = stdin;
    FILE *out = stdout;

    if (argc > 1) {
	in = fopen(argv[1], "rb");
	if (in == NULL) {
	    fprintf(stderr, "Can't open %s.\n", argv[1]);
	    return 1;
	}
    }
    if (argc > 2) {
	out = fopen(argv[2], "wb");
	if (out == NULL) {
	    fprintf(stderr, "Can't open %s.\n", argv[2]);
	    return 1;
	}
    }

    decode(in, out);

    if (argc > 1)
	fclose(in);
    if (argc > 2)
	fclose(out);

    return 0;
}

/* Each dictionary entry is a byte sequence and a length. */
typedef struct DictEntry {
    uint8_t *seq;
    int      len;
} DictEntry;

#define	DICT_BITS	12
#define	DICT_MAX	(1 << DICT_BITS)

/* We use a custom allocator because the maximum length of all sequences is
 * predictable, and we can keep all the sequences packed together by using
 * one big allocation and carving it up. */
typedef struct AllocInfo {
    uint8_t *base;
    int      len;
    uint8_t *nextAlloc;
} AllocInfo;

static void AllocInit(AllocInfo *alloc, int size);
static uint8_t *Allocate(AllocInfo *alloc, int len);

/* Use a struct to hold input state so we can read 12-bit codes from the
 * input file. */
typedef struct InputState {
    FILE   *fp;
    int     leftoverBits;
    int     leftoverCode;
} InputState;

static int ReadNextCode(InputState *inState);

#define NO_CODE		0xffff

static void decode(FILE *in, FILE *out)
{
    DictEntry  *dict      = NULL;
    AllocInfo   allocInfo;
    int         dictSize  = 256;
    InputState  inState   = { in, 0, 0 };
    uint16_t    prevCode  = ReadNextCode(&inState);
    uint8_t    *mark      = NULL;
    int         i         = 0;

    // Abort on empty input file.
    if (prevCode == NO_CODE)
	return;

    // The maximum of all sequences will be if the sequences increase in length
    // steadily from 1..DICT_MAX.  Add in an extra 2 bytes per entry to account
    // for the fact that we round each allocation to 4 bytes in size.
    AllocInit(&allocInfo, DICT_MAX*DICT_MAX/2 + DICT_MAX*2);

    // Initialize dictionary to single character entries.
    dict = calloc(DICT_MAX, sizeof(DictEntry));
    for (i = 0; i < dictSize; i++) {
	dict[i].seq = Allocate(&allocInfo, 1);
	dict[i].seq[0] = i;
	dict[i].len = 1;
    }
    // This mark is used to indicate where we should reset the allocations
    // to when we reset the dictionary to 256 entries.
    mark = allocInfo.nextAlloc;

    // Output the first code sequence, which is always a single byte.
    fputc(prevCode, out);

    do {
	uint16_t code = ReadNextCode(&inState);

	if (code > dictSize) {
	    if (code == NO_CODE)
		break;
	    fprintf(stderr, "Error: bad code %d, dictSize = %d.\n", code,
		    dictSize);
	    exit(1);
	}

	// Add entry to dictionary first.  That way, if we need to use
	// the just added dictionary entry, it will be ready to use.
	uint8_t lastChar;
	if (code == dictSize)
	    lastChar = dict[prevCode].seq[0];
	else
	    lastChar = dict[code].seq[0];

	if (dictSize == DICT_MAX) {
	    // Dictionary hit max size.  Reset it.
	    dictSize            = 256;
	    allocInfo.nextAlloc = mark;
	} else {
	    // Extend dictionary by one entry.  The new entry is the same
	    // as the previous entry plus one character (lastChar).
	    int prevLen        = dict[prevCode].len;
	    dict[dictSize].len = prevLen + 1;
	    dict[dictSize].seq = Allocate(&allocInfo, prevLen + 1);
	    memcpy(dict[dictSize].seq, dict[prevCode].seq, prevLen);
	    dict[dictSize++].seq[prevLen] = lastChar;
	}

	// Output code sequence to file.
	fwrite(dict[code].seq, 1, dict[code].len, out);
	prevCode = code;
    } while (1);

    free(dict);
    free(allocInfo.base);
}

static void AllocInit(AllocInfo *alloc, int size)
{
    alloc->base      = malloc(size);
    alloc->len       = size;
    alloc->nextAlloc = alloc->base;
}

static uint8_t *Allocate(AllocInfo *alloc, int len)
{
    uint8_t *ret = alloc->nextAlloc;

    // Round up to the nearest 4 byte alignment.
    len = (len + 3) & ~3;
    alloc->nextAlloc += len;
    return ret;
}

/**
 * Inputs 12 bits of code from the file in a MSB manner.
 */
static int ReadNextCode(InputState *inState)
{
    int code;
    int b0 = fgetc(inState->fp);

    if (b0 == EOF)
	return NO_CODE;

    if (inState->leftoverBits == 0) {
	int b1 = fgetc(inState->fp);

	if (b1 == EOF)
	    return NO_CODE;
	code = (b0 << 4) | (b1 >> 4);
	inState->leftoverBits = 4;
	inState->leftoverCode = (b1 & 0xf) << 8;
    } else {
	code = inState->leftoverCode | b0;
	inState->leftoverBits = 0;
    }
    return code;
}
