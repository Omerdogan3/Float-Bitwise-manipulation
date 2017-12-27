To exercise integer bitwise manipulation by looking at the sign, exponent and mantissa fields of IEEE floating point numbers.

									This program:

(1)Separates a floating point number into bit fields, and from those determine its:
sign (either -1 or +1) with getSign()
exponent (from EXPONENT_DENORMALIZED_BIT_PATTERN to EXPONENT_INFINITE_BIT_PATTERN) with getPowerOf2()
mantissa with getMantissa()

(2)Reconstitutes the bits of a floating point number from its:
Sign with signBits(): given -1 return 0x1, or given +1 return 0x0.
Exponent with expBits(): given the mantissa and exponent, return the exponent bits.
Mantissa with mantissaBits(): given the mantissa, return the mantissa bits: