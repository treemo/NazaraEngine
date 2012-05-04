// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Debug.hpp>

nzUInt8 NzContextParameters::defaultMajorVersion; // Initialis� par NzOpenGL
nzUInt8 NzContextParameters::defaultMinorVersion; // Initialis� par NzOpenGL
const NzContext* NzContextParameters::defaultShareContext = nullptr;
NzWindowHandle NzContextParameters::defaultWindow = 0;
bool NzContextParameters::defaultCompatibilityProfile = false;
bool NzContextParameters::defaultDoubleBuffered = false;
bool NzContextParameters::defaultShared = true;