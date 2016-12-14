#include "../include/astro_math.h"
#include "../include/core_math.h"

/* Most algorithms converted from FourmiLab Calendar Converter source */

namespace smalltime
{
	namespace math
	{
		/*  OBLIQEQ  --  Calculate the obliquity of the ecliptic for a given
		Julian date.  This uses Laskar's tenth-degree
		polynomial fit (J. Laskar, Astronomy and
		Astrophysics, Vol. 157, page 68 [1986]) which is
		accurate to within 0.01 arc second between AD 1000
		and AD 3000, and within a few seconds of arc for
		+/-10000 years around AD 2000.  If we're outside the
		range in which this fit is valid (deep time) we
		simply return the J2000 value of the obliquity, which
		happens to be almost precisely the mean.  */
		static const std::array<RD, 10> KOTERMS = { -4680.93, -1.55, 1999.25, -51.38, -249.67, -39.05, 7.12, 27.87, 5.79, 2.45 };

		// Other astrological constants
		static constexpr RD KJ2000 = 2451545.0;							// Julian day of J2000 epoch
		static constexpr RD KJULIAN_CENTURY = 36525.0;				    // Days in Julian century
		static constexpr RD KJULIAN_MILLENNIUM = (KJULIAN_CENTURY * 10);    // Days in Julian millennium
		static constexpr RD KASTRONOMICAL_UNIT = 149597870.0;				// Astronomical unit in kilometres
		static constexpr RD KTROPICAL_YEAR = 365.24219878;

		/* Periodic terms for nutation in longiude (delta \Psi) and
		obliquity (delta \Epsilon) as given in table 21.A of
		Meeus, "Astronomical Algorithms", first edition. */
		static std::array<int, 315> KNUT_ARG_MULT = {
				0, 0, 0, 0, 1,
				-2, 0, 0, 2, 2,
				0, 0, 0, 2, 2,
				0, 0, 0, 0, 2,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				-2, 1, 0, 2, 2,
				0, 0, 0, 2, 1,
				0, 0, 1, 2, 2,
				-2, -1, 0, 2, 2,
				-2, 0, 1, 0, 0,
				-2, 0, 0, 2, 1,
				0, 0, -1, 2, 2,
				2, 0, 0, 0, 0,
				0, 0, 1, 0, 1,
				2, 0, -1, 2, 2,
				0, 0, -1, 0, 1,
				0, 0, 1, 2, 1,
				-2, 0, 2, 0, 0,
				0, 0, -2, 2, 1,
				2, 0, 0, 2, 2,
				0, 0, 2, 2, 2,
				0, 0, 2, 0, 0,
				-2, 0, 1, 2, 2,
				0, 0, 0, 2, 0,
				-2, 0, 0, 2, 0,
				0, 0, -1, 2, 1,
				0, 2, 0, 0, 0,
				2, 0, -1, 0, 1,
				-2, 2, 0, 2, 2,
				0, 1, 0, 0, 1,
				-2, 0, 1, 0, 1,
				0, -1, 0, 0, 1,
				0, 0, 2, -2, 0,
				2, 0, -1, 2, 1,
				2, 0, 1, 2, 2,
				0, 1, 0, 2, 2,
				-2, 1, 1, 0, 0,
				0, -1, 0, 2, 2,
				2, 0, 0, 2, 1,
				2, 0, 1, 0, 0,
				-2, 0, 2, 2, 2,
				-2, 0, 1, 2, 1,
				2, 0, -2, 0, 1,
				2, 0, 0, 0, 1,
				0, -1, 1, 0, 0,
				-2, -1, 0, 2, 1,
				-2, 0, 0, 0, 1,
				0, 0, 2, 2, 1,
				-2, 0, 2, 0, 1,
				-2, 1, 0, 2, 1,
				0, 0, 1, -2, 0,
				-1, 0, 1, 0, 0,
				-2, 1, 0, 0, 0,
				1, 0, 0, 0, 0,
				0, 0, 1, 2, 0,
				-1, -1, 1, 0, 0,
				0, 1, 1, 0, 0,
				0, -1, 1, 2, 2,
				2, -1, -1, 2, 2,
				0, 0, -2, 2, 2,
				0, 0, 3, 2, 2,
				2, -1, 0, 2, 2
		};

		static std::array<int, 252> KNUT_ARG_COEFF = {
			-171996, -1742, 92095, 89,          /*  0,  0,  0,  0,  1 */
			-13187, -16, 5736, -31,          /* -2,  0,  0,  2,  2 */
			-2274, -2, 977, -5,          /*  0,  0,  0,  2,  2 */
			2062, 2, -895, 5,          /*  0,  0,  0,  0,  2 */
			1426, -34, 54, -1,          /*  0,  1,  0,  0,  0 */
			712, 1, -7, 0,          /*  0,  0,  1,  0,  0 */
			-517, 12, 224, -6,          /* -2,  1,  0,  2,  2 */
			-386, -4, 200, 0,          /*  0,  0,  0,  2,  1 */
			-301, 0, 129, -1,          /*  0,  0,  1,  2,  2 */
			217, -5, -95, 3,          /* -2, -1,  0,  2,  2 */
			-158, 0, 0, 0,          /* -2,  0,  1,  0,  0 */
			129, 1, -70, 0,          /* -2,  0,  0,  2,  1 */
			123, 0, -53, 0,          /*  0,  0, -1,  2,  2 */
			63, 0, 0, 0,          /*  2,  0,  0,  0,  0 */
			63, 1, -33, 0,          /*  0,  0,  1,  0,  1 */
			-59, 0, 26, 0,          /*  2,  0, -1,  2,  2 */
			-58, -1, 32, 0,          /*  0,  0, -1,  0,  1 */
			-51, 0, 27, 0,          /*  0,  0,  1,  2,  1 */
			48, 0, 0, 0,          /* -2,  0,  2,  0,  0 */
			46, 0, -24, 0,          /*  0,  0, -2,  2,  1 */
			-38, 0, 16, 0,          /*  2,  0,  0,  2,  2 */
			-31, 0, 13, 0,          /*  0,  0,  2,  2,  2 */
			29, 0, 0, 0,          /*  0,  0,  2,  0,  0 */
			29, 0, -12, 0,          /* -2,  0,  1,  2,  2 */
			26, 0, 0, 0,          /*  0,  0,  0,  2,  0 */
			-22, 0, 0, 0,          /* -2,  0,  0,  2,  0 */
			21, 0, -10, 0,          /*  0,  0, -1,  2,  1 */
			17, -1, 0, 0,          /*  0,  2,  0,  0,  0 */
			16, 0, -8, 0,          /*  2,  0, -1,  0,  1 */
			-16, 1, 7, 0,          /* -2,  2,  0,  2,  2 */
			-15, 0, 9, 0,          /*  0,  1,  0,  0,  1 */
			-13, 0, 7, 0,          /* -2,  0,  1,  0,  1 */
			-12, 0, 6, 0,          /*  0, -1,  0,  0,  1 */
			11, 0, 0, 0,          /*  0,  0,  2, -2,  0 */
			-10, 0, 5, 0,          /*  2,  0, -1,  2,  1 */
			-8, 0, 3, 0,          /*  2,  0,  1,  2,  2 */
			7, 0, -3, 0,          /*  0,  1,  0,  2,  2 */
			-7, 0, 0, 0,          /* -2,  1,  1,  0,  0 */
			-7, 0, 3, 0,          /*  0, -1,  0,  2,  2 */
			-7, 0, 3, 0,          /*  2,  0,  0,  2,  1 */
			6, 0, 0, 0,          /*  2,  0,  1,  0,  0 */
			6, 0, -3, 0,          /* -2,  0,  2,  2,  2 */
			6, 0, -3, 0,          /* -2,  0,  1,  2,  1 */
			-6, 0, 3, 0,          /*  2,  0, -2,  0,  1 */
			-6, 0, 3, 0,          /*  2,  0,  0,  0,  1 */
			5, 0, 0, 0,          /*  0, -1,  1,  0,  0 */
			-5, 0, 3, 0,          /* -2, -1,  0,  2,  1 */
			-5, 0, 3, 0,          /* -2,  0,  0,  0,  1 */
			-5, 0, 3, 0,          /*  0,  0,  2,  2,  1 */
			4, 0, 0, 0,          /* -2,  0,  2,  0,  1 */
			4, 0, 0, 0,          /* -2,  1,  0,  2,  1 */
			4, 0, 0, 0,          /*  0,  0,  1, -2,  0 */
			-4, 0, 0, 0,          /* -1,  0,  1,  0,  0 */
			-4, 0, 0, 0,          /* -2,  1,  0,  0,  0 */
			-4, 0, 0, 0,          /*  1,  0,  0,  0,  0 */
			3, 0, 0, 0,          /*  0,  0,  1,  2,  0 */
			-3, 0, 0, 0,          /* -1, -1,  1,  0,  0 */
			-3, 0, 0, 0,          /*  0,  1,  1,  0,  0 */
			-3, 0, 0, 0,          /*  0, -1,  1,  2,  2 */
			-3, 0, 0, 0,          /*  2, -1, -1,  2,  2 */
			-3, 0, 0, 0,          /*  0,  0, -2,  2,  2 */
			-3, 0, 0, 0,          /*  0,  0,  3,  2,  2 */
			-3, 0, 0, 0           /*  2, -1,  0,  2,  2 */
		};

		/*  Table of observed Delta T values at the beginning of
		even numbered years from 1620 through 2002.  */
		std::array<int, 382> KDELTA_TAB = {
			121, 112, 103, 95, 88, 82, 77, 72, 68, 63, 60, 56, 53, 51, 48, 46,
			44, 42, 40, 38, 35, 33, 31, 29, 26, 24, 22, 20, 18, 16, 14, 12,
			11, 10, 9, 8, 7, 7, 7, 7, 7, 7, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10,
			10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13,
			13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16,
			16, 16, 15, 15, 14, 13, 13.1, 12.5, 12.2, 12, 12, 12, 12, 12, 12,
			11.9, 11.6, 11, 10.2, 9.2, 8.2, 7.1, 6.2, 5.6, 5.4, 5.3, 5.4, 5.6,
			5.9, 6.2, 6.5, 6.8, 7.1, 7.3, 7.5, 7.6, 7.7, 7.3, 6.2, 5.2, 2.7,
			1.4, -1.2, -2.8, -3.8, -4.8, -5.5, -5.3, -5.6, -5.7, -5.9, -6,
			-6.3, -6.5, -6.2, -4.7, -2.8, -0.1, 2.6, 5.3, 7.7, 10.4, 13.3, 16,
			18.2, 20.2, 21.1, 22.4, 23.5, 23.8, 24.3, 24, 23.9, 23.9, 23.7,
			24, 24.3, 25.3, 26.2, 27.3, 28.2, 29.1, 30, 30.7, 31.4, 32.2,
			33.1, 34, 35, 36.5, 38.3, 40.2, 42.2, 44.5, 46.5, 48.5, 50.5,
			52.2, 53.8, 54.9, 55.8, 56.9, 58.3, 60, 61.6, 63, 65, 66.6
		};

		//  Periodic terms to obtain true time
		std::array<RD, 72> KEQUINOXP_TERMS = 
		{
			485, 324.96, 1934.136,
			203, 337.23, 32964.467,
			199, 342.08, 20.186,
			182, 27.85, 445267.112,
			156, 73.14, 45036.886,
			136, 171.52, 22518.443,
			77, 222.54, 65928.934,
			74, 296.72, 3034.906,
			70, 243.58, 9037.513,
			58, 119.81, 33718.147,
			52, 297.17, 150.678,
			50, 21.02, 2281.226,
			45, 247.54, 29929.562,
			44, 325.15, 31555.956,
			29, 60.93, 4443.417,
			18, 155.12, 67555.328,
			17, 288.79, 4562.452,
			16, 198.04, 62894.029,
			14, 199.76, 31436.921,
			12, 95.39, 14577.848,
			12, 287.11, 31931.756,
			12, 320.81, 34777.259,
			9, 227.73, 1222.114,
			8, 15.45, 16859.074
		};

		std::array<RD, 20> KJDE0_TAB1000 = { 
			1721139.29189, 365242.13740, 0.06134, 0.00111, -0.00071,
			1721233.25401, 365241.72562, -0.05323, 0.00907, 0.00025,
			1721325.70455, 365242.49558, -0.11677, -0.00297, 0.00074,
			1721414.39987, 365242.88257, -0.00769, -0.00933, -0.00006,
		 };

		std::array<RD, 20> KJDE0_TAB2000 =  {
			2451623.80984, 365242.37404, 0.05169, -0.00411, -0.00057,
			2451716.56767, 365241.62603, 0.00325, 0.00888, -0.00030,
			2451810.21715, 365242.01767, -0.11575, 0.00337, 0.00078,
			2451900.05952, 365242.74049, -0.06223, -0.00823, 0.00032,
		 };

		//====================================================
		// Oblique equation
		//====================================================
		RD ObliqEq(RD rd)
		{
			int i = 0;
			RD u = 0.0, v = 0.0;
			v = u = (rd - KJ2000) / (KJULIAN_CENTURY * 100.0);

			RD eps = 23.0 + (26.0 / 60.0) + (21.448 / 3600.0);

			if (abs(u) < 1.0)
			{
				for (i = 0; i < 10; i++)
				{
					eps += (KOTERMS[i] / 3600.0) * v;
					v *= u;
				}
			}
			return eps;
		}

		//==================================================
		// Calculate the nutation in longitude, deltaPsi, and
		// obliquity, deltaEpsilon for a given Julian date
		// jd.Results are returned as a two element Array
		// giving(deltaPsi, deltaEpsilon) in degrees.
		//==================================================
		std::pair<RD, RD> Nutation(RD rd)
		{
			RD deltaPsi = 0.0, deltaEpsilon = 0.0;
			int	i = 0, j = 0;
			RD	t = (rd - 2451545.0) / 36525.0;
			RD t2 = 0.0, t3 = 0.0, to10 = 0.0;
			std::array<RD, 4> ta = { 0.0 };
			RD dp = 0.0, de = 0.0, ang = 0.0;

			t3 = t * (t2 = t * t);

			/* Calculate angles.  The correspondence between the elements
			of our array and the terms cited in Meeus are:

			ta[0] = D  ta[0] = M  ta[2] = M'  ta[3] = F  ta[4] = \Omega

			*/

			ta[0] = DegToRad(297.850363 + 445267.11148 * t - 0.0019142 * t2 +
				t3 / 189474.0);
			ta[1] = DegToRad(357.52772 + 35999.05034 * t - 0.0001603 * t2 -
				t3 / 300000.0);
			ta[2] = DegToRad(134.96298 + 477198.867398 * t + 0.0086972 * t2 +
				t3 / 56250.0);
			ta[3] = DegToRad(93.27191 + 483202.017538 * t - 0.0036825 * t2 +
				t3 / 327270);
			ta[4] = DegToRad(125.04452 - 1934.136261 * t + 0.0020708 * t2 +
				t3 / 450000.0);

			/* Range reduce the angles in case the sine and cosine functions
			don't do it as accurately or quickly. */

			for (i = 0; i < 5; i++)
			{
				ta[i] = FixAngr(ta[i]);
			}

			to10 = t / 10.0;
			for (i = 0; i < 63; i++)
			{
				ang = 0;
				for (j = 0; j < 5; j++)
				{
					if (KNUT_ARG_MULT[(i * 5) + j] != 0)
					{
						ang += KNUT_ARG_MULT[(i * 5) + j] * ta[j];
					}
				}
				dp += (KNUT_ARG_COEFF[(i * 4) + 0] + KNUT_ARG_COEFF[(i * 4) + 1] * to10) * sin(ang);
				de += (KNUT_ARG_COEFF[(i * 4) + 2] + KNUT_ARG_COEFF[(i * 4) + 3] * to10) * cos(ang);
			}

			/* Return the result, converting from ten thousandths of arc
			seconds to radians in the process. */

			deltaPsi = dp / (3600.0 * 10000.0);
			deltaEpsilon = de / (3600.0 * 10000.0);

			return std::make_pair(deltaPsi, deltaEpsilon);
		}

		//===========================================================
		// Convert celestial(ecliptical) longitude and
		// latitude into right ascension(in degrees) and
		//	declination.We must supply the time of the
		//	conversion in order to compensate correctly for the
		//	varying obliquity of the ecliptic over time.
		//	The right ascension and declination are returned
		//	as a two - element Array in that order.
		//============================================================
		std::pair<RD, RD> EcliptoEq(RD rd, RD lambda, RD beta)
		{
			/* Obliquity of the ecliptic. */

			RD eps = DegToRad(ObliqEq(rd));

			RD ra = RadToDeg(atan2((cos(eps) * sin(DegToRad(lambda)) -
				(tan(DegToRad(beta)) * sin(eps))),
				cos(DegToRad(lambda))));

			ra = FixAngle(RadToDeg(atan2((cos(eps) * sin(DegToRad(lambda)) -
				(tan(DegToRad(beta)) * sin(eps))),
				cos(DegToRad(lambda)))));

			RD dec = RadToDeg(asin((sin(eps) * sin(DegToRad(lambda)) * cos(DegToRad(beta))) +
				(sin(DegToRad(beta)) * cos(eps))));

			return std::make_pair(ra, dec);
		}

		//===============================================
		// DELTAT  --  Determine the difference, in seconds, between
		// Dynamical time and Universal time.  
		//==============================================
		RD DeltaT(int year)
		{
			RD dt = 0.0, f = 0.0, i = 0.0, t = 0.0;

			if ((year >= 1620) && (year <= 2000))
			{
				i = std::floor(static_cast<RD>((year - 1620)) / 2.0);
				f = (static_cast<RD>((year - 1620)) / 2.0) - i;  /* Fractional part of year */
				dt = KDELTA_TAB[i] + ((KDELTA_TAB[i + 1] - KDELTA_TAB[i]) * f);
			}
			else 
			{
				t = static_cast<RD>((year - 2000)) / 100.0;
				if (year < 948) 
				{
					dt = 2177.0 + (497.0 * t) + (44.1 * t * t);
				}
				else 
				{
					dt = 102.0 + (102.0 * t) + (25.3 * t * t);
					if ((year > 2000) && (year < 2100)) 
					{
						dt += 0.37 * static_cast<RD>((year - 2100));
					}
				}
			}
			return dt;
		}

		//=====================================================
		// EQUINOX  --  Determine the Julian Ephemeris Day of an
		//	equinox or solstice.The "which" argument
		//	selects the item to be computed :
		//
		//  0   March equinox
		//	1   June solstice
		//	2   September equinox
		//	3   December solstice
		//=====================================================
		RD Equinox(int year, int which)
		{
			RD delta_l = 0.0, jde0 = 0.0, jde = 0.0, s = 0.0, t = 0.0, w = 0.0, y = 0.0;

			/*  Initialise terms for mean equinox and solstices.  We
			have two sets: one for years prior to 1000 and a second
			for subsequent years.  */
			RD* jde0_tab = nullptr;

			if (year < 1000)
			{
				jde0_tab = KJDE0_TAB1000.data();
				y = year / 1000;
			}
			else 
			{
				jde0_tab = KJDE0_TAB2000.data();
				y = (year - 2000) / 1000;
			}

			which *= 5;
			jde0 = jde0_tab[which + 0] +
				(jde0_tab[which + 1] * y) +
				(jde0_tab[which + 2] * y * y) +
				(jde0_tab[which +3] * y * y * y) +
				(jde0_tab[which +4] * y * y * y * y);

			t = (jde0 - 2451545.0) / 36525;
			w = (35999.373 * t) - 2.47;
			delta_l = 1 + (0.0334 * DegCos(w)) + (0.0007 * DegCos(2 * w));
			//  Sum the periodic terms for time T

			s = 0;
			int i = 0, j = 0;
			for (i = j = 0; i < 24; i++)
			{
				s += KEQUINOXP_TERMS[j] * DegCos(KEQUINOXP_TERMS[j + 1] + (KEQUINOXP_TERMS[j + 2] * t));
				j += 3;
			}

			jde = jde0 + ((s * 0.00001) / delta_l);

			return jde;
		}

		//=================================================================
		// SUNPOS  --  Position of the Sun.  Please see the comments
		// on the return statement at the end of this function
		// which describe the array it returns.  We return
		// intermediate values because they are useful in a
		// variety of other contexts. 
		//===================================================================
		std::array<RD, 12> SunPos(RD rd)
		{
			RD t = (rd - KJ2000) / KJULIAN_CENTURY;
			RD t2 = t * t;
			RD l0 = 280.46646 + (36000.76983 * t) + (0.0003032 * t2);
			l0 = FixAngle(l0);
			RD m = 357.52911 + (35999.05029 * t) + (-0.0001537 * t2);
			m = FixAngle(m);
			RD e = 0.016708634 + (-0.000042037 * t) + (-0.0000001267 * t2);
			RD c = ((1.914602 + (-0.004817 * t) + (-0.000014 * t2)) * DegSin(m)) +
				((0.019993 - (0.000101 * t)) * DegSin(2 * m)) +
				(0.000289 * DegSin(3 * m));

			RD sun_long = l0 + c;
			RD sun_anomaly = m + c;

			RD sun_r = (1.000001018 * (1 - (e * e))) / (1 + (e * DegCos(sun_anomaly)));
			RD omega = 125.04 - (1934.136 * t);
			RD lambda = sun_long + (-0.00569) + (-0.00478 * DegSin(omega));

			RD epsilon0 = ObliqEq(rd);
			RD epsilon = epsilon0 + (0.00256 * DegCos(omega));

			RD alpha = RadToDeg(atan2(DegCos(epsilon0) * DegSin(sun_long), DegCos(sun_long)));
			alpha = FixAngle(alpha);

			RD delta = RadToDeg(asin(DegSin(epsilon0) * DegSin(sun_long)));

			RD alpha_app = RadToDeg(atan2(DegCos(epsilon) * DegSin(lambda), DegCos(lambda)));
			alpha_app = FixAngle(alpha_app);

			RD delta_app = RadToDeg(asin(DegSin(epsilon) * DegSin(lambda)));

			return
			{                //  Angular quantities are expressed in decimal degrees
				l0,                           //  [0] Geometric mean longitude of the Sun
				m,                            //  [1] Mean anomaly of the Sun
				e,                            //  [2] Eccentricity of the Earth's orbit
				c,                            //  [3] Sun's equation of the Centre
				sun_long,                      //  [4] Sun's true longitude
				sun_anomaly,                   //  [5] Sun's true anomaly
				sun_r,                         //  [6] Sun's radius vector in AU
				lambda,                       //  [7] Sun's apparent longitude at true equinox of the date
				alpha,                        //  [8] Sun's true right ascension
				delta,                        //  [9] Sun's true declination
				alpha_app,                     // [10] Sun's apparent right ascension
				delta_app                      // [11] Sun's apparent declination
			};

		}

		//==========================================================
		// Compute equation of time for a given moment.
		// Returns the equation of time as a fraction of a day
		//===========================================================
		RD EquationOfTime(RD rd)
		{
			RD tau = (rd - KJ2000) / KJULIAN_MILLENNIUM;
			RD l0 = 280.4664567 + (360007.6982779 * tau) +
				(0.03032028 * tau * tau) +
				((tau * tau * tau) / 49931.0) +
				(-((tau * tau * tau * tau) / 15300.0)) +
				(-((tau * tau * tau * tau * tau) / 2000000.0));

			l0 = FixAngle(l0);
			
			RD alpha = SunPos(rd)[10];
			RD delta_psi = Nutation(rd).first;
			RD epsilon = ObliqEq(rd) + Nutation(rd).second;
			RD e = l0 + (-0.0057183) + (-alpha) + (delta_psi * DegCos(epsilon));
			e = e - 20.0 * (std::floor(e / 20.0));
			e = e / (24 * 60);

			return e;
		}


	}
}