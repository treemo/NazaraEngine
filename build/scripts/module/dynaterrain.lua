if (not _OPTIONS["united"]) then
	project "NazaraDynaTerrain"
end

files
{
	"../include/Nazara/DynaTerrain/**.hpp",
	"../include/Nazara/DynaTerrain/**.inl",
	"../src/Nazara/DynaTerrain/**.hpp",
	"../src/Nazara/DynaTerrain/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/DynaTerrain/Posix/*.hpp", "../src/Nazara/DynaTerrain/Posix/*.cpp" }
else
	excludes { "../src/Nazara/DynaTerrain/Win32/*.hpp", "../src/Nazara/DynaTerrain/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/DynaTerrain/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"
		links "NazaraRenderer-s-d"
		links "NazaraGraphics-s-d"
		links "NazaraTerrainRenderer-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"
		links "NazaraRenderer-s"
		links "NazaraGraphics-s"
		links "NazaraTerrainRenderer-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"
		links "NazaraRenderer-d"
		links "NazaraGraphics-d"
		links "NazaraTerrainRenderer-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
		links "NazaraRenderer"
		links "NazaraGraphics"
		links "NazaraTerrainRenderer"
end