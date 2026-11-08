cmake_minimum_required(VERSION 3.10)

SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out" CACHE PATH "")

# Single-config generators (Makefiles, Ninja) build nothing optimized unless
# asked to, so default to an optimized build that still has debug symbols.
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type." FORCE)
ENDIF()

SET(BUILD_GAME true CACHE BOOL "Whether or not to build the main game.")
SET(BUILD_MASTER false CACHE BOOL "Whether or not to build the master server.")
SET(BUILD_HEADLESS false CACHE BOOL "Set to true if you want to build a headless server instead of the whole game.")

IF(WIN32)
  SET(CERT "" CACHE FILEPATH "The location of the digital certificate with which to sign your installer.")
  SET(CERT_PASS "" CACHE STRING "The password with which to use your digital certificate.")
ENDIF()

SET(PATH_ZLIB "" CACHE PATH "The path the Zlib, for systems that can't find it.")
IF(NOT BUILD_HEADLESS)
  SET(PATH_GLEW "" CACHE PATH "The path to GLEW, for systems that can't find it.")
  SET(PATH_QT "" CACHE PATH "The path the Qt, for systems that can't find it.")
  SET(PATH_SFML "" CACHE PATH "The path to SFML, for systems that can't find it.")
  IF(WIN32)
    SET(PATH_UCRT "" CACHE PATH "The path the the Windows Universal CRT.")
  ENDIF()
ENDIF()

IF(NOT "${PATH_ZLIB}" STREQUAL "")
  SET(ZLIB_ROOT "${PATH_ZLIB}")
ENDIF()
IF(NOT BUILD_HEADLESS)
  IF(NOT "${PATH_GLEW}" STREQUAL "")
    SET(GLEW_ROOT "${PATH_GLEW}")
  ENDIF()
  IF(NOT "${PATH_QT}" STREQUAL "")
    SET(Qt5_DIR "${PATH_QT}/lib/cmake/Qt5")
    SET(Qt6_DIR "${PATH_QT}/lib/cmake/Qt6")
    LIST(APPEND CMAKE_PREFIX_PATH "${PATH_QT}")
  ENDIF()
  IF(NOT "${PATH_SFML}" STREQUAL "")
    SET(SFML_ROOT "${PATH_SFML}")
  ENDIF()
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules" "${PATH_SFML}/cmake/Modules")

PROJECT("TreadMarks")

# std::filesystem needs C++17 (Qt6 requires it as well).
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED OFF)
SET(CMAKE_CXX_EXTENSIONS OFF)

IF(MSVC)
  ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(/wd4244)
  ADD_DEFINITIONS(/wd4018)
ENDIF()

IF(BUILD_HEADLESS)
  ADD_DEFINITIONS(-DHEADLESS)
ENDIF()

IF(WIN32)
  SET(INSTALL_DIR "/")
ELSE()
  # An absolute destination would install into the filesystem root.  The game
  # and its data live side by side in the install prefix.
  SET(INSTALL_DIR ".")
ENDIF()

IF(BUILD_GAME)
  ADD_SUBDIRECTORY("src/game")
  IF(WIN32)
    IF(NOT BUILD_HEADLESS)
      ADD_SUBDIRECTORY("src/launcher")
    ENDIF()
  ENDIF()
ENDIF()

IF(BUILD_MASTER)
  ADD_SUBDIRECTORY("src/tmmaster")
ENDIF()

ADD_SUBDIRECTORY("src/installer")
