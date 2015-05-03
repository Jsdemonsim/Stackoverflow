package maaartin.euler.e0.e05;

import static com.google.common.base.Preconditions.checkArgument;

import java.util.List;

import lombok.EqualsAndHashCode;
import lombok.Getter;

import com.google.common.base.Joiner;
import com.google.common.base.Splitter;
import com.google.common.collect.EnumMultiset;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMultiset;
import com.google.common.collect.ImmutableSortedSet;
import com.google.common.collect.Iterables;
import com.google.common.collect.Lists;
import com.google.common.collect.Multiset;

@Getter @EqualsAndHashCode(of="cards") class Hand implements Comparable<Hand> {
	Hand(Iterable<Card> cards) {
		assert Iterables.size(cards) == CARDS_IN_HAND;
		this.cards = ImmutableSortedSet.copyOf(cards);
		checkArgument(this.cards.size() == 5, cards);
		int  suitBits = 0;
		long info     = 0;

		for (final Card c : this.cards) {
			final int rank = c.rank().value() - 2;
			long rankBits  = info & (RANK_MASK << rank);

			if (rankBits == 0) {
			    // First card of this rank, add it to singles.
			    rankBits = 1L << rank;
			} else {
			    // Already have card of this rank, shift it to
			    // next count.
			    rankBits |= (rankBits << 13);
			}
			info ^= rankBits;
			suitBits |= (1 << (c.suit().symbol() - 'A'));
		}

		// Determine straight and flush.
		long singleCards = info & SINGLE_MASK;
		singleCards >>>= Long.numberOfTrailingZeros(singleCards);
		isStraight = (singleCards == STRAIGHT_VAL);
		isFlush    = (Integer.bitCount(suitBits) == 1);
		handInfo   = info;
	}

	/**
	 * The inverse method to {@link #toString().}
	 * 
	 * <p>The input must be exactly of the form "vs vs vs vs vs",
	 * where {@code v} is a {@code Value.symbol()} and {@code s} is {@code Rank.symbol()},
	 * for example {@code "5H 5C 6S 7S KD"}
	 */
	static Hand forString(String cards) {
		checkArgument(cards.length() == TO_STRING_LENGTH);
		final List<Card> list = Lists.newArrayList();
		for (final String s : Splitter.on(' ').splitToList(cards)) list.add(Card.forString(s));
		return new Hand(list);
	}

	/**
	 * Returns a positive number if {@code this} is stronger than {@code other},
	 * a negative number if it's weaker,
	 * and 0 if they're equally strong.
	 */
	@Override public int compareTo(Hand other) {
		final HandValue myHandValue = toHandValue();
		final HandValue otherHandValue = other.toHandValue();
		int result = myHandValue.compareTo(otherHandValue);
		if (result!=0) return result;

		assert myHandValue == otherHandValue;

		return Long.compare(handInfo, other.handInfo());
	}

	public long handInfo() {
	    return handInfo;
	}

	@Override public String toString() {
		return Joiner.on(' ').join(cards);
	}

	HandValue toHandValue() {
		for (final HandValue v : HAND_VALUES) {
			if (v.apply(this)) return v;
		}
		throw new RuntimeException("Impossible: " + cards);
	}

	private static final int CARDS_IN_HAND = 5;
	static final int TO_STRING_LENGTH = CARDS_IN_HAND * 3 - 1;

	private static final ImmutableList<HandValue> HAND_VALUES = ImmutableList.copyOf(HandValue.values()).reverse();

	private final ImmutableSortedSet<Card> cards;
	private final boolean isStraight;
	private final boolean isFlush;

	// HandInfo bits:
	//
	//  0-12: Single cards 2..A
	// 13-25: Pairs        2..A
	// 26-38: Triples      2..A
	// 39-51: Quads        2..A
	private final long handInfo;

	// Some would prefer these:
	//public  static final long SINGLE_MASK  = 0x0000000000001fffL;
	//public  static final long PAIR_MASK    = 0x0000000003ffe000L;
	//public  static final long TRIO_MASK    = 0x0000007ffc000000L;
	//public  static final long QUAD_MASK    = 0x000fff8000000000L;
	//private static final long RANK_MASK    = 0x0000008004002001L;
	//private static final long STRAIGHT_VAL = 0x000000000000001fL;

	// Others would prefer these:
	
	// These are masks of 13 cards each.
	public  static final long SINGLE_MASK  = (1L<<13) - (1L<<0);
	public  static final long PAIR_MASK    = (1L<<26) - (1L<<13);
	public  static final long TRIO_MASK    = (1L<<39) - (1L<<26);
	public  static final long QUAD_MASK    = (1L<<52) - (1L<<39);

	// This is a mask for one particular rank (4 bits).
	public  static final long RANK_MASK    = (1L<<0)  | (1L<<13) |
						 (1L<<26) | (1L<<39);

	// This is used to detect a straight.
	private static final long STRAIGHT_VAL = (1L<<5) - (1L<<0);
}
