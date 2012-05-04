// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCKIMPL_WINDOWS_HPP
#define NAZARA_CLOCKIMPL_WINDOWS_HPP

#include <Nazara/Prerequesites.hpp>

bool NzClockImplInitializeHighPrecision();
nzUInt64 NzClockImplGetMicroseconds();
nzUInt64 NzClockImplGetMilliseconds();

#endif // NAZARA_CLOCKIMPL_WINDOWS_HPP