// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR3_HPP
#define NAZARA_VECTOR3_HPP

#include <Nazara/Core/String.hpp>

template<typename T> class NzVector3
{
	public:
		NzVector3();
		NzVector3(T X, T Y, T Z);
		explicit NzVector3(T scale);
		NzVector3(T vec[3]);
		template<typename U> explicit NzVector3(const NzVector3<U>& vec);
		NzVector3(const NzVector3& vec) = default;
		~NzVector3() = default;

		T AbsDotProduct(const NzVector3& vec) const;
		NzVector3 CrossProduct(const NzVector3& vec) const;
		double Distance(const NzVector3& vec) const;
		T DotProduct(const NzVector3& vec) const;
		NzVector3 GetNormal() const;
		void MakeCeil(const NzVector3& vec);
		void MakeFloor(const NzVector3& vec);
		double Length() const;
		double Normalize();
		T SquaredDistance(const NzVector3& vec) const;
		T SquaredLength() const;

		NzString ToString() const;

		operator NzString() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		const NzVector3& operator+() const;
		NzVector3 operator-() const;

		NzVector3 operator+(const NzVector3& vec) const;
		NzVector3 operator-(const NzVector3& vec) const;
		NzVector3 operator*(const NzVector3& vec) const;
		NzVector3 operator*(T scale) const;
		NzVector3 operator/(const NzVector3& vec) const;
		NzVector3 operator/(T scale) const;

		NzVector3& operator+=(const NzVector3& vec);
		NzVector3& operator-=(const NzVector3& vec);
		NzVector3& operator*=(const NzVector3& vec);
		NzVector3& operator*=(T scale);
		NzVector3& operator/=(const NzVector3& vec);
		NzVector3& operator/=(T scale);

		bool operator==(const NzVector3& vec) const;
		bool operator!=(const NzVector3& vec) const;
		bool operator<(const NzVector3& vec) const;
		bool operator<=(const NzVector3& vec) const;
		bool operator>(const NzVector3& vec) const;
		bool operator>=(const NzVector3& vec) const;

		T x;
		T y;
		T z;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzVector3<T>& vec);

template<typename T> NzVector3<T> operator*(T scale, const NzVector3<T>& vec);
template<typename T> NzVector3<T> operator/(T scale, const NzVector3<T>& vec);

typedef NzVector3<double> NzVector3d;
typedef NzVector3<float> NzVector3f;
typedef NzVector3<int> NzVector3i;

#include <Nazara/Math/Vector3.inl>

#endif // NAZARA_VECTOR3_HPP