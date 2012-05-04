// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Config.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

template<typename T>
T NzApproach(T value, T objective, T increment)
{
	if (value < objective)
		return std::min(value + increment, objective);
	else if (value > objective)
		return std::max(value - increment, objective);
	else
		return value;
}

template<typename T>
T NzClamp(T value, T min, T max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

template<typename T>
T NzDegrees(T degrees)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return NzDegreeToRadian(degrees);
	#else
	return degrees;
	#endif
}

template<typename T>
T NzDegreeToRadian(T degrees)
{
	return degrees * (M_PI/180.0);
}

unsigned int NzGetNumberLength(signed char number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned char number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(short number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned short number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(int number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned int number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(long long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(std::abs(number)))+(number < 0 ? 2 : 1);
}

unsigned int NzGetNumberLength(unsigned long long number)
{
	if (number == 0)
		return 1;

	return static_cast<unsigned int>(std::log10(number))+1;
}

unsigned int NzGetNumberLength(float number, nzUInt8 precision)
{
	// L'impr�cision des flottants n�cessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

unsigned int NzGetNumberLength(double number, nzUInt8 precision)
{
	// L'impr�cision des flottants n�cessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

unsigned int NzGetNumberLength(long double number, nzUInt8 precision)
{
	// L'impr�cision des flottants n�cessite un cast (log10(9.99999) = 1)
	return NzGetNumberLength(static_cast<long long>(number)) + precision + 1; // Plus un pour le point
}

template<typename T>
T NzNormalizeAngle(T angle)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	const T limit = M_PI;
	#else
	const T limit = 180.0;
	#endif

	///TODO: Trouver une solution sans duplication
	if (angle > 0.0)
	{
		angle += limit;
		angle -= static_cast<int>(angle/(2.0*limit))*(2.0*limit);
		angle -= limit;
	}
	else
	{
		angle -= limit;
		angle -= static_cast<int>(angle/(2.0*limit))*(2.0*limit);
		angle += limit;
	}

	return angle;
}

template<typename T>
bool NzNumberEquals(T a, T b)
{
	if (a > b)
		return (a-b) <= std::numeric_limits<T>::epsilon();
	else
		return (b-a) <= std::numeric_limits<T>::epsilon();

}

NzString NzNumberToString(long long number, nzUInt8 radix)
{
	#if NAZARA_MATH_SAFE
	if (radix < 2 || radix > 36)
	{
		NazaraError("Base must be between 2 and 36");
		return NzString();
	}
	#endif

	if (number == 0)
		return '0';

	static const char* symbols("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	bool negative;
	if (number < 0)
	{
		negative = true;
		number = -number;
	}
	else
		negative = false;

	NzString str;
	str.Reserve(NzGetNumberLength(number));

	do
	{
		str += symbols[number % radix];
		number /= radix;
	}
	while (number > 0);

	if (negative)
		str += '-';

	return str.Reversed();
}

template<typename T>
T NzRadians(T radians)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return radians;
	#else
	return NzRadianToDegree(radians);
	#endif
}

template<typename T>
T NzRadianToDegree(T radians)
{
	return radians * (180.0/M_PI);
}

long long NzStringToNumber(NzString str, nzUInt8 radix)
{
	#if NAZARA_MATH_SAFE
	if (radix < 2 || radix > 36)
	{
		NazaraError("Radix must be between 2 and 36");
		return 0;
	}
	#endif

	str.Simplify();
	if (radix > 10)
		str.ToUpper();

	bool negative = str.StartsWith('-');
	static const char* symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	char* digit = &str[(negative) ? 1 : 0];
	unsigned long long total = 0;
	do
	{
		if (*digit == ' ')
			continue;

		total *= radix;
		const char* c = std::strchr(symbols, *digit);
		if (c && c-symbols < radix)
			total += c-symbols;
		else
		{
			NazaraError("str is not a valid number");
			return 0;
		}
	}
	while (*++digit);

	return (negative) ? -static_cast<long long>(total) : total;
}

#include <Nazara/Core/DebugOff.hpp>