package maaartin.euler.e0.e05;

import static com.google.common.base.Preconditions.checkArgument;

import java.util.EnumSet;
import java.util.List;

import lombok.RequiredArgsConstructor;

import com.google.common.base.Predicate;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMultiset;
import com.google.common.collect.ImmutableSet;
import com.google.common.collect.Lists;


@RequiredArgsConstructor enum HandValue implements Predicate<Hand> {
	HIGH_CARD {
		@Override public boolean apply(Hand input) {
			long handInfo = input.handInfo();
			return Long.bitCount(handInfo & Hand.SINGLE_MASK) == 5;
		}
	},
	PAIR {
		@Override public boolean apply(Hand input) {
		    long handInfo = input.handInfo();
		    return (Long.bitCount(handInfo & Hand.PAIR_MASK) == 1) &&
			   (Long.bitCount(handInfo & Hand.SINGLE_MASK) == 3);
		}
	},
	TWO_PAIRS {
		@Override public boolean apply(Hand input) {
		    long handInfo = input.handInfo();
		    return (Long.bitCount(handInfo & Hand.PAIR_MASK) == 2);
		}
	},
	THREE_OF_A_KIND {
		@Override public boolean apply(Hand input) {
		    long handInfo = input.handInfo();
		    return (Long.bitCount(handInfo & Hand.TRIO_MASK) == 1) &&
			   (Long.bitCount(handInfo & Hand.SINGLE_MASK) == 2);
		}
	},
	STRAIGHT {
		@Override public boolean apply(Hand input) {
		    return input.isStraight();
		}
	},
	FLUSH {
		@Override public boolean apply(Hand input) {
		    return input.isFlush();
		}
	},
	FULL_HOUSE {
		@Override public boolean apply(Hand input) {
		    long handInfo = input.handInfo();
		    return (Long.bitCount(handInfo & Hand.TRIO_MASK) == 1) &&
			   (Long.bitCount(handInfo & Hand.PAIR_MASK) == 1);
		}
	},
	FOUR_OF_A_KIND {
		@Override public boolean apply(Hand input) {
		    long handInfo = input.handInfo();
		    return Long.bitCount(handInfo & Hand.QUAD_MASK) == 1;
		}
	},
	STRAIGHT_FLUSH {
		@Override public boolean apply(Hand input) {
		    return input.isStraight() && input.isFlush();
		}
	},
	;
}
