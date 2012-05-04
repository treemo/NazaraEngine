// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/OcclusionQuery.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzOcclusionQuery::NzOcclusionQuery() :
m_id(0)
{
	#if NAZARA_RENDERER_SAFE
	if (IsSupported())
	#endif
		glGenQueries(1, reinterpret_cast<GLuint*>(&m_id));
	#if NAZARA_RENDERER_SAFE
	else
	{
		NazaraError("Occlusion queries not supported");
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (!m_id)
	{
		NazaraError("Failed to create occlusion query");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzOcclusionQuery::~NzOcclusionQuery()
{
	if (m_id)
		glDeleteQueries(1, reinterpret_cast<GLuint*>(&m_id));
}

void NzOcclusionQuery::Begin()
{
	glBeginQuery(GL_SAMPLES_PASSED, m_id);
}

void NzOcclusionQuery::End()
{
	glEndQuery(GL_SAMPLES_PASSED);
}

unsigned int NzOcclusionQuery::GetResult() const
{
	GLuint result;
	glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);

	return result;
}

bool NzOcclusionQuery::IsResultAvailable() const
{
	GLint available;
	glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &available);

	return available == GL_TRUE;
}

bool NzOcclusionQuery::IsSupported()
{
	return NazaraRenderer->HasCapability(nzRendererCap_OcclusionQuery);
}