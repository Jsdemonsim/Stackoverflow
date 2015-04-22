import java.util.Collection;
import java.util.List;
import java.util.regex.Pattern;
import java.util.ArrayList;

/**
 * Represents the standard 4x4 board.
 *
 * <p>Terminology:<ul>
 * <li>Index is a number between 0 and 15 denoting the position on the board.
 * <li>Piece is a number between 0 and 15 with 0 denoting the empty space.
 */
public class FifteenBoard extends Board<FifteenBoard> {
    /**
     * Create a new board by interpreting the input as list of pieces. Accepts two formats:<ul>
     *
     * <li>List of sixteen space-separated decimal numbers like
     *     {@code "11 15 12 0 14 10 2 13 7 6 9 8 3 5 4 1"}
     * <li>List of four underscore-separated groups of four hexadecimal digits like
     *     {@code "0123_4567_89AB_CDEF"}
     *
     */
    static FifteenBoard from(String input) {
        return FifteenBoard.from(parseStringToLong(input));
    }

    /**
     * Create a new board by interpreting every digit of the argument as the piece on the corresponding index,
     * where 0 denotes the empty place.
     * See also {@link #toString()}.
     */
    static FifteenBoard from(long indexToPiece) {
        final int distance = findDistance(indexToPiece);
        final int indexOfHole = findHole(indexToPiece);
        return new FifteenBoard(indexToPiece, distance, indexOfHole);
    }

    private static int findDistance(long indexToPiece)
    {
        // Every pair of bits in x and y is one coordinate.
        // The coordinates of the empty space don't matter and therefore get excluded via & ~15.
        // For all others, we compute the sum of absolute values of their differences.
        // See http://codereview.stackexchange.com/a/86907/14363.
        final long pieceToIndex = dual(indexToPiece);
        final long x = pieceToIndex  & ~15;
        final long y = FINISHED_PIECE_TO_INDEX & ~15;
        final long xor = x^y;
        // High bit per pair will contain whether the pair is 3, low bit is garbled.
        final long is3 = xor & (xor << 1);

        // High bit per pair will contain whether the pair is non-zero, low bit is garbled.
        final long x2 = x | (x << 1);
        final long y2 = y | (y << 1);

        // High bit per pair will contain whether both pairs are non-zero, low bit is garbled.
        final long is0 = x2 & y2;

        // High bit per pair will contain whether the pairs need correction, low bit is 0.
        final long isBoth = (is3 & is0) & HIGH;
        final long val = xor ^ isBoth; // only invert the bits set in both is3 and is0

        // Count the high bits twice and the low bits ones.
        return Long.bitCount(val) + Long.bitCount(val & HIGH);
    }

    private static int findHole(long indexToPiece)
    {
        for (int i=0;i<INDEX_LIMIT;i++) {
            if (get(indexToPiece, i) == 0)
                return i;
        }
        return 0;
    }

    @Override
    public int distance()
    {
        return distance;
    }

    private FifteenBoard(long indexToPiece, int distance, int indexOfHole) {
        this.indexToPiece = indexToPiece;
        this.distance     = distance;
        this.indexOfHole  = indexOfHole;
    }

    private static long parseStringToLong(String input) {
        long result = 0;
        if (input.contains(" ")) {
            String split[] = input.split(" ");
            for (final String s : split) {
                final int n = Integer.parseInt(s);
                result = (result << 4) + n;
            }
        } else {
            try {
                result = Long.parseLong(input.replaceAll("_", ""), 16);
            } catch (NumberFormatException e) {
                result = 0;
            }
        }
        return result;
    }

    static int indexToRow(int index) {
        return index >> LOGSIZE;
    }

    static int indexToCol(int index) {
        return index & (SIZE-1);
    }

    static long dual(long data) {
        long result = 0;
        for (int index=0; index<SIZE*SIZE; ++index) result += (long) index << (4 * get(data, index));
        return result;
    }

    /** Return true if {@code data} in hexadecimal contain all hexadecimal digits. */
    private static boolean isValidBoard(long data) {
        int bitset = 0;
        for (int index=0; index<SIZE*SIZE; ++index)
            bitset |= 1 << get(data, index);
        return bitset == 0xFFFF;
    }

    /**
     * Return a string representation of {@code this}, consisting of 4 groups of 4 hexadecimal digits.
     * The groups are separated by an underscore and each corresponds with a puzzle row.
     * Every digit corresponds with a piece, with 0 denoting the empty position.
     */
    @SuppressWarnings("boxing") @Override public String toString() {
        return String.format("%04X\n%04X\n%04X\n%04X",
                (indexToPiece>>48) & 0xFFFF,
                (indexToPiece>>32) & 0xFFFF,
                (indexToPiece>>16) & 0xFFFF,
                (indexToPiece>>00) & 0xFFFF);
    }

    @Override public boolean equals(Object obj) {
        if (!(obj instanceof FifteenBoard)) return false;
        // The other field can be ignored as it's the dual.
        return indexToPiece == ((FifteenBoard) obj).indexToPiece;
    }

    @Override public int hashCode() {
        // This may be a premature optimization, but something like Long.hashCode might lead to too many collisions.
        final long result = (123456789 * indexToPiece);
        return new Long(result).hashCode();
    }


    @Override public Collection<FifteenBoard> children() {
        return addChildrenTo(new ArrayList<FifteenBoard>(4));
    }

    private Collection<FifteenBoard> addChildrenTo(Collection<FifteenBoard> result) {
        final int col = indexToCol(indexOfHole);
        final int row = indexToRow(indexOfHole);
        if (col > 0)      result.add(move(indexOfHole-1,    3));
        if (col < SIZE-1) result.add(move(indexOfHole+1,    2));
        if (row > 0)      result.add(move(indexOfHole-SIZE, 1));
        if (row < SIZE-1) result.add(move(indexOfHole+SIZE, 0));
        return result;
    }

    @Override public FifteenBoard alternative() {
        // Swap the first two non-empty positions.
        final int index1 = indexToPiece(0) == 0 ? 2 : 0;
        final int index2 = indexToPiece(1) == 0 ? 2 : 1;
        final long piece1 = indexToPiece(index1);
        final long piece2 = indexToPiece(index2);
        final long pieceXor = piece1 ^ piece2;
        final long childIndexToPiece = indexToPiece ^ (pieceXor << 4*index1) ^ (pieceXor << 4*index2);
        final int childDistance = findDistance(childIndexToPiece);
        return new FifteenBoard(childIndexToPiece, childDistance, indexOfHole);
    }

    /* This array gives the correct row or column index for the given
     * number in the final position, which looks like this:
     *
     *   COL 3 --v  v-- COL 0
     *  ROW 3 -> 1234
     *           5678
     *           9abc
     *  ROW 0 -> def 
     *
     * So for example, correctCoord[5][0] would look up the correct row
     * for "5" in the final position, which is row 2.  CorrectCoord[5][1]
     * would look up the column for "5" which is column 3.
     */
    static final int [][] correctCoord = {
        { 0, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1 }
    };

    /**
     * Swap the piece at the given index with the hole.
     *
     * <p>This is a valid move iff the index corresponds to a neighbor
     * of the hole.
     *
     * @direction        0 = up, 1 = down, 2 = left, 3 = right.
     */
    private FifteenBoard move(int index, int direction) {
        final int  piece      = indexToPiece(index);
        final long pieceXor   = ((long) piece << 4*index) |
                                ((long) piece << 4*indexOfHole);
        final long childIndexToPiece = indexToPiece ^ pieceXor;

        final int  goodCoord = correctCoord[direction >> 1][piece];
        final int  holeCoord = ((direction & 0x2) == 0) ?
                            indexToRow(indexOfHole) : indexToCol(indexOfHole);
        final int  childDistance = distance + (((direction & 1) == 0) ?
            /* left /up   */ (goodCoord <= holeCoord ? -1 : 1) :
            /* right/down */ (goodCoord >= holeCoord ? -1 : 1));

        // The hole is where the piece used to be.
        return new FifteenBoard(childIndexToPiece, childDistance, index);
    }

    int indexToPiece(int index) {
        return get(indexToPiece, index);
    }

    private static int get(long data, int index) {
        return (int) ((data >>> (4*index)) & 0xF);
    }

    private static final long HIGH = 0xAAAAAAAAAAAAAAAAL;

    private static final int SIZE    = 4;
    private static final int LOGSIZE = 2;
    private static final int INDEX_LIMIT = SIZE*SIZE;
    private static final long FINISHED_PIECE_TO_INDEX = 0x123456789abcdef0L;

    final long indexToPiece;
    final int  distance;
    final int  indexOfHole;
}
