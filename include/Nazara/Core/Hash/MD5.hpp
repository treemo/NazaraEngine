// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_MD5_HPP
#define NAZARA_HASH_MD5_HPP

#define NAZARA_HASH_MD5

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>

struct NzHashMD5_state;

class NAZARA_API NzHashMD5 : public NzHashImpl
{
	public:
		NzHashMD5();
		virtual ~NzHashMD5();

		void Append(const nzUInt8* data, unsigned int len);
		void Begin();
		NzHashDigest End();

		static unsigned int GetDigestLength();
		static NzString GetHashName();

	private:
		NzHashMD5_state* m_state;
};

#undef NAZARA_HASH_MD5

#endif // NAZARA_HASH_MD5_HPP