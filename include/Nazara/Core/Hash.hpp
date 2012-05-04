// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASH_HPP
#define NAZARA_HASH_HPP

#define NAZARA_HASH

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/HashImpl.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

class NAZARA_API NzHash : NzNonCopyable
{
	public:
		NzHash(nzHash hash);
		NzHash(NzHashImpl* hashImpl);
		~NzHash();

		NzHashDigest Hash(const NzHashable& hashable);

	private:
		NzHashImpl* m_impl;
};

#undef NAZARA_HASH

#endif // NAZARA_HASH_HPP