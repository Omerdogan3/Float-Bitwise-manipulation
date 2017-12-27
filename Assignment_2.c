/*-------------------------------------------------------------------------*
 *  *---									---*
 *   *---		floatFields.c						---*
 *    *---									---*
 *     *---	    This file defines a C program that pulls an IEEE floating	---*
 *      *---	point number into its respective sign, exponent and mantissa	---*
 *       *---	fields, and reconstitutes it from those fields.			---*
 *        *---									---*
 *         *---	----	----	----	----	----	----	----	----	---*
 *          *---									---*
 *           *---	Version 1.0		2017 June 22		Joseph Phillips	---*
 *            *---									---*
 *             *---  Student: Lea Middleton
 *              *-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

//--			Sign related constants				--//
//
////  PURPOSE:  To tell how many bits to shift the sign bit from the least
////	signficant position to where the sign bit belongs.
#define	 SIGN_SHIFT				31
//
////  PURPOSE:  To be the mask to only keep the sign bit.
#define	 SIGN_MASK				(0x1 << SIGN_SHIFT)
//
////  PURPOSE:  To be the mask to keep everything but the sign bit.
#define	 EVERYTHING_BUT_SIGN_MASK		(~SIGN_MASK)
//
//
//
////--			Exponent related constants			--//
//
////  PURPOSE:  To tell how many bits to shift the exponent bit field from the
////	least signficant position to where the exponent bit field belongs.
#define	 EXPONENT_SHIFT				23
//
////  PURPOSE:  To be the mask to only keep the exponent bit field.
#define	 EXPONENT_MASK			((unsigned)0xFF << EXPONENT_SHIFT)
//
////  PURPOSE:  To tell the exponent bit pattern for 'infinity' and
////	'not-a-number'.
#define	 EXPONENT_INFINITE_BIT_PATTERN		0xFF
//
////  PURPOSE:  To tell the exponent bit pattern for denormalized numbers
////	(including 0.0).
#define	 EXPONENT_DENORMALIZED_BIT_PATTERN	0x00
//
////  PURPOSE:  To tell the 'bias' of the exponent bit field:
////	(powerOf2) = (exponentBitPattern) - EXPONENT_BIAS
#define	 EXPONENT_BIAS				0x7F
//
////  PURPOSE:  To tell the power of 2 for 'infinity' and 'not-a-number'.
#define	 INFINITE_POWER_OF_2			+128
//
////  PURPOSE:  To tell the power of 2 for denormalized numbers (including 0.0):
#define	 DENORMALIZED_POWER_OF_2		-127
//
////  PURPOSE:  To tell the lowest legal power of 2.
#define	 LOWEST_LEGAL_POWER_OF_2		(DENORMALIZED_POWER_OF_2+1)
//
#define	 INDISTINGUISHABLE_FROM_0_POWER_OF_2	(DENORMALIZED_POWER_OF_2-23)
//
//
////--			Mantissa related constants			--//
//
////  PURPOSE:  To tell the mask to only keep the mantissa bit field.
#define	 MANTISSA_MASK				0x007FFFFF
//
////  PURPOSE:  To tell give the hidden bit in its proper position.
#define	 MANTISSA_HIDDEN_BIT			0x00800000
//
////  PURPOSE:  To tell how many bits to shift the mantissa bit field from the
////	least signficant position to where the mantissa bit field belongs.
#define	 MANTISSA_SHIFT				0
//
////  PURPOSE:  To tell how many mantissa bits there are (including hidden bit)
#define	 NUM_MANTISSA_BITS			24
//
//
//
////--			Miscellaneous related constants			--//
//
////  PURPOSE:  To give the maximum length of C-strings.
#define	 TEXT_LEN				64

int isZero(float f){
	//  Use EVERYTHING_BUT_SIGN_MASK to see if u is a +0.0 or -0.
	unsigned int u = *(unsigned int*)&f;
	unsigned int i = u & EVERYTHING_BUT_SIGN_MASK;
	printf("I: 0x%8X\n", i);
	printf("SIGN_MASK: 0x%08X\n", SIGN_MASK);
	printf("isZero: 0x%8X\n", i);

	/* shift RIGHT 8 to the beginning to see it the
	 * bit is 0
	 */
	if((i >> 8 != 0) || (i >> 8 != -0 )){
		return(0);
	} 
	return(1);
}

int	getSign(float f){
	unsigned int u = *(unsigned int*)&f;
	printf("Bit Pattern: 0x%8X\n",u);
	unsigned int i = (u & SIGN_MASK);
	//printf("U & Sign_Mask: 0X%08X\n", i);

	/* shift RIGHT by 31 or SIGN_SHIFT, in order to
	 * see if the first bit is on or off
	 */
	if(i >> SIGN_SHIFT == 0){
		return(1);
	 } 
	return(-1);
}

int getPowerOf2(float f){
	unsigned int u = *(unsigned int*)&f;
	unsigned int xponent_bits = (u & EXPONENT_MASK)>> EXPONENT_SHIFT;
	unsigned int j = xponent_bits - EXPONENT_BIAS;
	int i = *(int*)&j;
	if(i > LOWEST_LEGAL_POWER_OF_2){
		return(i);
	}
	return(LOWEST_LEGAL_POWER_OF_2);
}  
unsigned int getMantissa(float f){
	unsigned int	mantissa;
	unsigned int	exponent;
	mantissa = (*(unsigned int*)&f) & MANTISSA_MASK;
	exponent = ((*(unsigned int*)&f) & EXPONENT_MASK) >> EXPONENT_SHIFT;

	/* shifts the exponent bit pattern to the LEFT
	 * by 8 to see if the first bit equals 0. If it
	 * is,then it is a denormalized exponent or a
	 * (-) exponent
	 */
	if(exponent << 8 == 0){
		return(mantissa | EXPONENT_DENORMALIZED_BIT_PATTERN);
	}
	return(mantissa | MANTISSA_HIDDEN_BIT);
}

unsigned char signBits(int sign){
	unsigned int u = *(unsigned int*)&sign;
	unsigned int i = (u & SIGN_MASK);

	/* shift RIGHT by 31 or SIGN_SHIFT to see if the
	 * first bit is on or off
	 */
	if(i >> SIGN_SHIFT == 0){
		return(0);
	}
	return(1);
}

unsigned char expBits(int powerOf2,unsigned int	mantissaBits){
	unsigned int l = (*(unsigned int*)&powerOf2) + EXPONENT_BIAS;
	if((mantissaBits & MANTISSA_HIDDEN_BIT) != MANTISSA_HIDDEN_BIT){
		return (EXPONENT_DENORMALIZED_BIT_PATTERN);
	}else if(powerOf2 >= INFINITE_POWER_OF_2){
		return(EXPONENT_INFINITE_BIT_PATTERN);
	}
		return(l);
}

unsigned int mantissaBits(unsigned int mantissa){
	return(mantissa & (MANTISSA_HIDDEN_BIT -1));
}


int	main(){
  char		text[TEXT_LEN];
  float		f;
  unsigned int	u;
  float*	gPtr	= (float*)&u;
  unsigned int*	vPtr	= (unsigned int*)&f;

  do
  {
    printf("Please enter a floating point number or 0 to quit testing: ");
    fgets(text,TEXT_LEN,stdin);

    f					= atof(text);
    int			sign		= getSign(f);
    unsigned int	mantissa	= getMantissa(f);
    int			exp		= getPowerOf2(f);
    unsigned char	signB		= signBits(sign);
    unsigned int	mantissaB	= mantissaBits(mantissa);
    unsigned char	expB		= expBits(exp,mantissa);
    u					= (signB     << SIGN_SHIFT)	|
    					  (expB	     << EXPONENT_SHIFT)	|
					  (mantissaB << MANTISSA_SHIFT);

    printf("\n");
    printf("The sign     of %g is %+d (should be either +1 or -1)\n",f,sign);
    printf("The exponent of %g is 2^%d (should be from -127 to +127)\n",f,exp);
    printf("The mantissa of %g is 0x%08X\n",f,mantissa);

    printf("\n");
    printf("\t\t     Your bits:\tC library:\n");
    printf("Sign        %+7d: 0x%X\t0x%X\n",
	   sign,signB,*vPtr >> SIGN_SHIFT
	  );
    printf("Exponent       %+4d: 0x%02X\t0x%02X\n",
	   exp,expB,(*vPtr & EXPONENT_MASK) >> EXPONENT_SHIFT
	  );
    printf("Mantissa 0x%08X: 0x%08X\t0x%08X\n",
	   mantissa,mantissaB, (*vPtr & MANTISSA_MASK) >> MANTISSA_SHIFT
	  );
    printf("\n");
    printf("Original:\t%g\nReconstituted:\t%g\n%s\n",
	   f,*gPtr,( (f == *gPtr) ? "Good!" : "Uh-oh!" )
	  );
    printf("\n");
  }
  while  ( !isZero(f) );

  printf("\n\n");

  return(EXIT_SUCCESS);
}

