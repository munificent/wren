workspace "wren"
  configurations { "Release", "Debug" }
  platforms { "64bit", "32bit", "arm", "64bit-no-nan-tagging" }
  defaultplatform "64bit"
  startproject "wren_test"
  location ("../" .. _ACTION)

  filter "configurations:Debug"
    targetsuffix "_d"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

  filter "platforms:64bit-no-nan-tagging"
    defines { "WREN_NAN_TAGGING=0" }

  --the 'xcode4' and 'gmake2' folder names
  --are simply confusing, so, simplify then
  filter { "action:xcode4" }
    location ("../xcode")

  filter "action:gmake2"
    location ("../make")

  filter { "action:gmake2", "system:bsd" }
    location ("../make.bsd")

  filter { "action:gmake2", "system:macosx" }
    location ("../make.mac")

  filter "platforms:32bit"
    architecture "x86"

  filter "platforms:64bit"
    architecture "x86_64"

  filter "system:windows"
    systemversion "latest"
    defines { "_CRT_SECURE_NO_WARNINGS" }

  filter "system:linux"
    links { "m" }

  filter "system:bsd"
    links { "m" }

project "wren"
  kind "StaticLib"
  language "C"
  cdialect "C99"
  targetdir "../../lib"

  files {
    "../../src/**.h",
    "../../src/**.c"
  }

  includedirs {
    "../../src/include",
    "../../src/vm",
    "../../src/optional"
  }

project "wren_shared"
  kind "SharedLib"
  targetname "wren"
  language "C"
  cdialect "C99"
  targetdir "../../lib"

  files {
    "../../src/**.h",
    "../../src/**.c"
  }

  includedirs {
    "../../src/include",
    "../../src/vm",
    "../../src/optional"
  }

project "wren_test"
  kind "ConsoleApp"
  language "C"
  cdialect "C99"
  targetdir "../../bin"
  dependson "wren"
  links { "wren" }

  files {
    "../../test/main.c",
    "../../test/test.c",
    "../../test/test.h",
    "../../test/api/*.c",
    "../../test/api/*.h"
  }

  includedirs {
    "../../src/include"
  }
