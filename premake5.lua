workspace "slo-jam"
	configurations { "debug", "release" }

local raylib_dir = 'raylib/src'

project "asteroids"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	location "asteroids/"

	targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
	objdir "obj/%{prj.name}/%{cfg.buildcfg}"
	targetname "asteroids"

	includedirs { "%{prj.location}/include" }

	includedirs { "%{wks.location}/libs/raylib/include/" }
	libdirs { "%{wks.location}/libs/raylib/" }

	links { "raylib" }

	filter "system:windows"
		links { "OpenGL32", "GDI32", "WinMM"}
	filter {}

	files { "%{prj.location}/**.h", "%{prj.location}/**.hpp", "%{prj.location}/**.cpp" }

	prebuildcommands {
		"{COPYDIR} %{prj.location}/resources/ %{wks.location}/bin/%{prj.name}/%{cfg.buildcfg}/resources/",
	}
	
	filter "configurations:debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "NDEBUG" }
		optimize "On"

	filter {}
