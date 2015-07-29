/*
 * LZW encoder
 *
 * - Uses fixed length 12-bit encodings.
 * - Outputs in MSB format.
 * - When encoding table fills up, then table is reset back to the initial
 *   256 entries.
 * - Written in C89 style.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static void encode(FILE *in, FILE *out);

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

    encode(in, out);

    if (argc > 1)
	fclose(in);
    if (argc > 2)
	fclose(out);

    return 0;
}

/* The LZW encoder builds a trie out of the input file, but only adds one
 * new trie node per sequence that it outputs.  There will be a maximum
 * of DICT_MAX sequences, so the child trie pointers can be uint16_t values
 * which are the node indices of the child nodes.  An index of 0 is like
 * a NULL pointer, because no node can point to node 0. */
typedef struct DictNode {
    uint16_t child[256];
} DictNode;

#define	DICT_BITS	12
#define	DICT_MAX	(1 << DICT_BITS)

/**
 * LZW encoder.  Reads from file "in" and outputs to file "out".
 */
static void encode(FILE *in, FILE *out)
{
    DictNode   *dictionary   = NULL;
    int         dictSize     = 256;
    int         nextByte     = fgetc(in);
    uint16_t    curNode      = nextByte;
    int         leftoverBits = 0;
    int         leftoverByte = 0;

    // Abort on empty input file.
    if (nextByte == EOF)
	return;

    // Initialize the dictionary.
    dictionary = calloc(DICT_MAX, sizeof(DictNode));
    if (dictionary == NULL)
	return;

    do {
	int curByte = fgetc(in);

	// Check if the file ended.  If so, output the last code and any
	// leftover bits, and then break out of the main loop.
	if (curByte == EOF) {
	    if (leftoverBits == 0) {
		fputc(curNode >> 4, out);
		fputc(curNode << 4, out);
	    } else {
		fputc(leftoverByte | (curNode >> 8), out);
		fputc(curNode, out);
	    }
	    break;
	}

	// Follow the new byte down the trie.
	uint16_t nextNode = dictionary[curNode].child[curByte];
	if (nextNode != 0) {
	    // The sequence exists, keep searching down the trie.
	    curNode = nextNode;
	    continue;
	}

	// The sequence doesn't exist.  First, output the code for curNode.
	// This is hardcoded for 12-bit output codes.
	if (leftoverBits == 0) {
	    fputc(curNode >> 4, out);
	    leftoverBits = 4;
	    leftoverByte = (curNode << 4);
	} else {
	    fputc(leftoverByte | (curNode >> 8), out);
	    fputc(curNode, out);
	    leftoverBits = 0;
	}

	// Now, extend the sequence in the trie by the new byte.
	if (dictSize < DICT_MAX) {
	    dictionary[curNode].child[curByte] = dictSize++;
	} else {
	    // The trie hit max size.  Instead of extending the trie,
	    // clear it back to the original 256 entries.
	    memset(dictionary, 0, DICT_MAX * sizeof(dictionary[0]));
	    dictSize = 256;
	}

	// Start over a new sequence with the current byte.
	curNode = curByte;
    } while (1);

    free(dictionary);
}
