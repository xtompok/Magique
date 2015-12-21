/******************************************************************************
 * Filename: segment.c
 *
 * Created: 2013/02/02 21:31
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include "sr.h"
#include "segment.h"

/* Digit description. Corresponding output positions are defined in hardware.h */
static unsigned int __digits[] = {
	~(A + B + _C + D + E + F),
	~(B + _C), 
	~(A + B + G + E + D),
	~(A + B + _C + D + G),
	~(F + G + _C + B),
	~(A + F + G + _C + D),
	~(A + F + G + _C + D + E),
	~(A + B + _C),
	~(A + B + _C + D + E + F + G),
	~(A + B + _C + D + F + G),
	~(E + F + A + B + _C + G),
	~(F + E + D + _C + G),
	~(A + F + E + D),
	~(B + _C + D + E + G),
	~(A + F + E + D + G),
	~(A + F + E + G)
};

void show_digit(unsigned char digit, char number) {
	/* Clear digit bits. */
	_sr_conf &= ~SMPLEX_MASK;

	/* Select display */
	_sr_conf |= (1 << (SMPLEX_START+number)) & (0x3 << SMPLEX_START);

	/* Show digit, as defined in __digits array. */
	_sr_conf |= __digits[digit] & SMPLEX_DIGIT_MASK;
}

void clear_digit() {
	_sr_conf &= SMPLEX_START;
}
