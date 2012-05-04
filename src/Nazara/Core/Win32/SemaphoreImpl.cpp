// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>

NzSemaphoreImpl::NzSemaphoreImpl(unsigned int count)
{
	m_semaphore = CreateSemaphore(nullptr, count, std::numeric_limits<LONG>::max(), nullptr);
	#if NAZARA_CORE_SAFE ///FIXME: Ne peut �chouer qu'� cause de mauvais param�tres ?
	if (!m_semaphore)
		NazaraError("Failed to create semaphore: " + NzGetLastSystemError());
	#endif
}

NzSemaphoreImpl::~NzSemaphoreImpl()
{
	CloseHandle(m_semaphore);
}

unsigned int NzSemaphoreImpl::GetCount() const
{
	LONG count;
	ReleaseSemaphore(m_semaphore, 0, &count);
	return count;
}

void NzSemaphoreImpl::Post()
{
	#if NAZARA_CORE_SAFE
	if (!ReleaseSemaphore(m_semaphore, 1, nullptr))
		NazaraError("Failed to release semaphore: " + NzGetLastSystemError());
	#else
	ReleaseSemaphore(m_semaphore, 1, nullptr);
	#endif
}

void NzSemaphoreImpl::Wait()
{
	#if NAZARA_CORE_SAFE
	if (WaitForSingleObject(m_semaphore, INFINITE) == WAIT_FAILED)
		NazaraError("Failed to wait for semaphore: " + NzGetLastSystemError());
	#else
	WaitForSingleObject(m_semaphore, INFINITE);
	#endif
}

bool NzSemaphoreImpl::Wait(nzUInt32 timeout)
{
	#if NAZARA_CORE_SAFE
	DWORD result = WaitForSingleObject(m_semaphore, timeout);
	if (result == WAIT_FAILED)
	{
		NazaraError("Failed to wait for semaphore: " + NzGetLastSystemError());
		return false;
	}
	else
		return result == WAIT_OBJECT_0;
	#else
	return WaitForSingleObject(m_semaphore, timeout) == WAIT_OBJECT_0;
	#endif
}