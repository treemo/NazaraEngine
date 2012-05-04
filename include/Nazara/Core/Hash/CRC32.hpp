// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_CRC32_HPP
#define NAZARA_HASH_CRC32_HPP

#define NAZARA_HASH_CRC32

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>

struct NzHashCRC32_state;

class NAZARA_API NzHashCRC32 : public NzHashImpl
{
	public:
		NzHashCRC32(nzUInt32 polynomial = 0x04c11db7);
		virtual ~NzHashCRC32();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		NzHashCRC32_state* m_state;
};

#undef NAZARA_HASH_CRC32

#endif // NAZARA_HASH_CRC32_HPP