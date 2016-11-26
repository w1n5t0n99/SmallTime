#pragma once
#ifndef _FLOATUTIL_
#define _FLOATUTIL_

#include <iostream>
#include <cfloat>
#include <cinttypes>
#include <assert.h>
#include <cstdlib>

/* See
https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
for the potential portability problems with the union and bit-fields below.
*/

//================================================================
// Single Precision Floating Point
//==================================================================
union Float_t
{
	Float_t(float num = 0.0f) : f(num) {}
	// Portable extraction of components.
	bool Negative() const { return i < 0; }
	int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
	int32_t RawExponent() const { return (i >> 23) & 0xFF; }

	int32_t i;
	float f;
#ifdef _DEBUG
	struct
	{   // Bitfields for exploration. Do not use in production code.
		uint32_t mantissa : 23;
		uint32_t exponent : 8;
		uint32_t sign : 1;
	} parts;
#endif
};

inline void IterateAllPositiveFloats()
{
	// Start at zero and print that float.
	Float_t allFloats;
	allFloats.f = 0.0f;
	printf(" %1.8e\n", allFloats.f);

	// Continue through all of the floats, stopping
	// when we get to positive infinity.
	while (allFloats.RawExponent() < 255)
	{
		// Increment the integer representation
		// to move to the next float.
		allFloats.i += 1;
		printf(" %1.8e\n" , allFloats.f);
	}
}

inline bool AlmostEqualUlps(float A, float B, int maxUlpsDiff)
{
	Float_t uA(A);
	Float_t uB(B);

	// Different signs means they do not match.
	if (uA.Negative() != uB.Negative())
	{
		// Check for equality to make sure +0==-0
		if (A == B)
			return true;
		return false;
	}

	// Find the difference in ULPs.
	int ulpsDiff = abs(uA.i - uB.i);
	if (ulpsDiff <= maxUlpsDiff)
		return true;

	return false;
}

inline bool AlmostEqualRelative(float A, float B,
	float maxRelDiff = FLT_EPSILON)
{
	// Calculate the difference.
	float diff = fabs(A - B);
	A = fabs(A);
	B = fabs(B);
	// Find the largest
	float largest = (B > A) ? B : A;
	if (diff <= largest * maxRelDiff)
		return true;
	
	return false;
}

inline bool AlmostEqualUlpsAndAbs(float A, float B,
	float maxDiff, int maxUlpsDiff)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	float absDiff = fabs(A - B);
	if (absDiff <= maxDiff)
		return true;

	Float_t uA(A);
	Float_t uB(B);

	// Different signs means they do not match.
	if (uA.Negative() != uB.Negative())
		return false;

	// Find the difference in ULPs.
	int ulpsDiff = abs(uA.i - uB.i);
	if (ulpsDiff <= maxUlpsDiff)
		return true;

	return false;
}

inline bool AlmostEqualRelativeAndAbs(float A, float B,
	float maxDiff, float maxRelDiff = FLT_EPSILON)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	float diff = fabs(A - B);
	if (diff <= maxDiff)
		return true;

	A = fabs(A);
	B = fabs(B);
	float largest = (B > A) ? B : A;

	if (diff <= largest * maxRelDiff)
		return true;
	return false;
}

#ifdef _DEBUG
inline float TestFloatPrecisionAwayFromZero(float input)
{
	union Float_t num;
	num.f = input;
	// Incrementing infinity or a NaN would be bad!
	assert(num.parts.exponent < 255);
	// Increment the integer representation of our value
	num.i += 1;
	// Subtract the initial value to find our precision
	float delta = num.f - input;
	return delta;
}

inline float TestFloatPrecisionTowardsZero(float input)
{
	union Float_t num;
	num.f = input;
	// Decrementing from zero would be bad!
	assert(num.parts.exponent || num.parts.mantissa);
	// Decrementing a NaN would be bad!
	assert(num.parts.exponent != 255 || num.parts.mantissa == 0);
	// Decrement the integer representation of our value
	num.i -= 1;
	// Subtract the initial value to find our precision
	float delta = num.f - input;
	return -delta;
}

struct TwoFloats
{
	float awayDelta;
	float towardsDelta;
};

inline struct TwoFloats TestFloatPrecision(float input)
{
	struct TwoFloats result =
	{
		TestFloatPrecisionAwayFromZero(input),
		TestFloatPrecisionTowardsZero(input),
	};
	return result;
}

#endif

//================================================================
// Double Precision Floating Point
//==================================================================
union Double_t
{
	Double_t(double num = 0.0) : d(num) {}
	// Portable extraction of components
	bool Negative() const { return i < 0; }
	int64_t RawMantissa() const { return i & ((1 << 52) - 1); }
	int64_t RawExponent() const { return (i >> 52) & ((1 << 11) - 1); }

	int64_t i;
	double d;
#ifdef _DEBUG
	struct
	{   // Bitfields for exploration. Do not use in production code.
		uint64_t mantissa : 23;
		uint64_t exponent : 8;
		uint64_t sign : 1;
	} parts;
#endif
};

inline bool AlmostEqualUlps(double A, double B, int maxUlpsDiff)
{
	Double_t uA(A);
	Double_t uB(B);

	// Different signs means they do not match.
	if (uA.Negative() != uB.Negative())
	{
		// Check for equality to make sure +0==-0
		if (A == B)
			return true;
		return false;
	}

	// Find the difference in ULPs.
	long long ulpsDiff = abs(uA.i - uB.i);

	if (ulpsDiff <= static_cast<long long>(maxUlpsDiff))
		return true;

	return false;
}

inline bool AlmostEqualRelative(double A, double B,
	double maxRelDiff = DBL_EPSILON)
{
	// Calculate the difference.
	double diff = fabs(A - B);
	A = fabs(A);
	B = fabs(B);
	// Find the largest
	double largest = (B > A) ? B : A;

	if (diff <= largest * maxRelDiff)
		return true;
	return false;
}

inline bool AlmostEqualUlpsAndAbs(double A, double B,
	double maxDiff, int maxUlpsDiff)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	double absDiff = fabs(A - B);
	if (absDiff <= maxDiff)
		return true;

	Double_t uA(A);
	Double_t uB(B);

	// Different signs means they do not match.
	if (uA.Negative() != uB.Negative())
		return false;

	// Find the difference in ULPs.
	int ulpsDiff = abs(uA.i - uB.i);
	if (ulpsDiff <= maxUlpsDiff)
		return true;

	return false;
}

inline bool AlmostEqualRelativeAndAbs(double A, double B,
	double maxDiff, double maxRelDiff = DBL_EPSILON)
{
	// Check if the numbers are really close -- needed
	// when comparing numbers near zero.
	double diff = fabs(A - B);
	if (diff <= maxDiff)
		return true;

	A = fabs(A);
	B = fabs(B);
	double largest = (B > A) ? B : A;

	if (diff <= largest * maxRelDiff)
		return true;
	return false;
}

struct Flt_Intv
{
	float minInterval = 0.0f;
	float maxInterval = 0.0f;
	float precision = 0.0f;
};

inline Flt_Intv FindFloatInterval(float input)
{
	Float_t f(input);
	Float_t nf;
	nf.i = f.i + 1;

	int exp = static_cast<int>(f.RawExponent()) - 127;

	Flt_Intv fInf;
	fInf.minInterval = pow(2, exp);
	fInf.maxInterval = pow(2, exp + 1);

	Float_t min(fInf.minInterval);
	Float_t nextMin;
	nextMin.i = min.i + 1;

	fInf.precision = nextMin.f - min.f;
	
	return fInf;
}

#endif
