kind "ConsoleApp"

files {"main.cpp","MyHeightSource.hpp","MyHeightSource.cpp"}

if (_OPTIONS["united"]) then
	configuration "DebugStatic"
		links "NazaraEngine-s-d"

	configuration "ReleaseStatic"
		links "NazaraEngine-s"

	configuration "DebugDLL"
		links "NazaraEngine-d"

	configuration "ReleaseDLL"
		links "NazaraEngine"
else
	configuration "DebugStatic"
		links "Nazara3D-s-d"
		links "NazaraRenderer-s-d"
		links "NazaraUtility-s-d"
		links "NazaraCore-s-d"
		links "NazaraNoise-s-d"
		links "NazaraDynaTerrain-s-d"

	configuration "ReleaseStatic"
		links "Nazara3D-s"
		links "NazaraRenderer-s"
		links "NazaraUtility-s"
		links "NazaraCore-s"
		links "NazaraNoise-s"
		links "NazaraDynaTerrain-s"

	configuration "DebugDLL"
		links "Nazara3D-d"
		links "NazaraRenderer-d"
		links "NazaraUtility-d"
		links "NazaraCore-d"
		links "NazaraNoise-d"
		links "NazaraDynaTerrain-d"

	configuration "ReleaseDLL"
		links "Nazara3D"
		links "NazaraRenderer"
		links "NazaraUtility"
		links "NazaraCore"
		links "NazaraNoise"
		links "NazaraDynaTerrain"
end