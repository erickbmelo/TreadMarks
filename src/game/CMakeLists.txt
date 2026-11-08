cmake_minimum_required(VERSION 3.10)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

IF(NOT BUILD_HEADLESS)
  SET(GAME_SRCS
    "CJoyInput.cpp"
    "GenericBuffer.cpp"      
    "Poly.cpp"
    "Render.cpp"
    "SimpleStatus.cpp"        
    "Sound.cpp"
    "TankGUI.cpp"
  )
ELSE()
  SET(GAME_SRCS
    "CJoyInputHeadless.cpp"
    "GenericBufferHeadless.cpp"
    "PolyHeadless.cpp"
    "RenderHeadless.cpp"
    "SimpleStatusHeadless.cpp"
    "SoundHeadless.cpp"
    "TankGUIHeadless.cpp"
    "GL/TerrainStub.cpp"
  )
ENDIF(NOT BUILD_HEADLESS)

SET(GAME_SRCS 
${GAME_SRCS}
"BanList.cpp"
"BanList.h"           "Basis.cpp"            "Basis.h"
"splash.png"          "BitPacking.cpp"       "BitPacking.h"
"CamRbd.cpp"          "CamRbd.h"             "macros.h"
"CfgParse.cpp"        "CfgParse.h"
"CJoyInput.h"	  
"Compression.cpp"
"Compression.h"       "Crypt.cpp"            "Crypt.h"
"CStr.cpp"            "CStr.h"               
"Directories.h"       "Filesystem.h"         "EntityBase.cpp"       "EntityBase.h"
"EntityFlag.cpp"      "EntityFlag.h"         "EntityGUI.cpp"
"EntityGUI.h"         "EntityPFire.cpp"      "EntityPFire.h"
"EntityRacetank.cpp"  "EntityTank.cpp"       "EntityTank.h"
"FileCRCList.cpp"     "FileCRCList.h"        "FileManager.cpp"
"FileManager.h"       "Find.cpp"             "Find.h"
"GenericBuffer.h"   
"Heartbeat.cpp"
"Heartbeat.h"         "HelpText.cpp"         "HelpText.h"
"IFF.cpp"             "IFF.h"                "Image.cpp"
"Image.h"             "LadderManager.cpp"    "LadderManager.h"
"Mesh.cpp"
"Networking.cpp"      "Networking.h"         "neuquant.c"
"neuquant.h"          "PacketProcessors.cpp" "PacketProcessors.h"
"Physics.cpp"         "Physics.h"            "pi.cpp"
"pi.h"                "Poly.h"
"Quantizer.cpp"       "Quantizer.h"          "Reg.cpp"       
"Reg.h"               "Render.h"
"ResourceManager.cpp" "ResourceManager.h"    
"SimpleStatus.h"      "Sound.h"              "StatsPackage.cpp"
"StatsPackage.h"      "TankAI.cpp"           "TankAI.h"
"TankGame.cpp"        "TankGame.h"        
"TankGUI.h"           "TankRacing.cpp"       "TankRacing.h"
"Terrain.cpp"         "Terrain.h"            "TextLine.cpp"
"TextLine.hpp"        "Timer.cpp"            "Timer.h"
"TMMaster.h"          "Trees.h"
"TxtBlock.cpp"        "TxtBlock.hpp"         "vecmath.cpp"
"vecmath.h"           "Vector.cpp"           "Vector.h"
"version.h"
"VertexArray.h"       "VoxelAfx.h"           "VoxelWorld.cpp"
"VoxelWorld.h"        "Web.h"
)

SET(GAME_RESOURCES "resources.qrc")
SET(GAME_SRCS ${GAME_SRCS} ${GAME_RESOURCES})

SOURCE_GROUP("base" FILES ${GAME_SRCS})

IF(WIN32)
  SET(WIN32_SRCS
    "win32/Directories.cpp"
    "win32/Web.cpp"            "win32/Treadmarks.rc"
    "win32/TreadMarks.gdf.xml" )
  SET(GAME_SRCS ${GAME_SRCS} ${WIN32_SRCS})
  SOURCE_GROUP("win32" FILES ${WIN32_SRCS})
ELSE()
  SET(POSIX_SRCS
    "Posix/Directories.cpp"
    "Posix/Web.cpp" )

  SET(GAME_SRCS ${GAME_SRCS} ${POSIX_SRCS})
  SOURCE_GROUP("posix" FILES ${POSIX_SRCS})
ENDIF()

IF(NOT BUILD_HEADLESS)
  SET(BAD_GUI_REQS "")

  SET(OpenGL_GL_PREFERENCE GLVND)
  FIND_PACKAGE(OpenGL)
  IF(OPENGL_FOUND)
    SET(GL_SRCS
      "GL/GLPolyRender.cpp"
      "GL/GLPolyRender.h"      "GL/GLTerrain.cpp"        "GL/GLTerrain.h"
      "GL/GLTerrainRender.cpp" "GL/GLTerrainRender.h"    "GL/GLTerrainRender2.cpp"
      "GL/GLTerrainRender2.h"  "GL/GLTerrainRender3.cpp" "GL/GLTerrainRender3.h")
    SET(GAME_SRCS ${GAME_SRCS} ${GL_SRCS})
    SOURCE_GROUP("gl" FILES ${GL_SRCS})
  ELSE()
    SET(BAD_GUI_REQS "${BAD_GUI_REQS} OpenGL")
  ENDIF()

  SET(CMAKE_INCLUDE_CURRENT_DIR ON)
  SET(CMAKE_AUTOMOC ON)
  SET(CMAKE_AUTORCC ON)

  # Either Qt 5 or Qt 6 will do; the game only uses it for the splash screen
  # and the dedicated server window.
  FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets QUIET)
  IF(Qt5_FOUND)
    SET(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
  ELSE()
    FIND_PACKAGE(Qt6 COMPONENTS Core Gui Widgets QUIET)
    IF(Qt6_FOUND)
      SET(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)
    ENDIF()
  ENDIF()
  IF(QT_LIBS)
    SET(QT_SRCS	"Qt/Dedicated.h"	"Qt/Dedicated.cpp")
    SET(GAME_SRCS ${GAME_SRCS} ${QT_SRCS})
    SOURCE_GROUP("qt" FILES ${QT_SRCS})
  ELSE()
    SET(BAD_GUI_REQS "${BAD_GUI_REQS} Qt")
  ENDIF()
ENDIF(NOT BUILD_HEADLESS)

IF(WIN32 AND NOT BUILD_HEADLESS)
  ADD_EXECUTABLE(TM WIN32 ${GAME_SRCS})
ELSE()
  ADD_EXECUTABLE(TM ${GAME_SRCS})
ENDIF()

IF(MSVC)
  SET_PROPERTY(TARGET TM APPEND PROPERTY LINK_FLAGS /DEBUG)
ENDIF()

IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # This code was written for MSVC, which neither assumes strict aliasing nor
  # treats signed overflow as undefined.  The game type-puns floats through
  # integer pointers in many places, so keep GCC/Clang from optimizing on those
  # assumptions.
  TARGET_COMPILE_OPTIONS(TM PRIVATE -fno-strict-aliasing -fwrapv)
ENDIF()

IF(WIN32)
  TARGET_LINK_LIBRARIES(TM winmm.lib ws2_32.lib version.lib)
ENDIF()

IF(NOT BUILD_HEADLESS)
  IF(QT_LIBS)
    TARGET_LINK_LIBRARIES(TM ${QT_LIBS})
  ENDIF()

  IF(OPENGL_FOUND)
    TARGET_LINK_LIBRARIES(TM OpenGL::GL)
  ENDIF()
ENDIF(NOT BUILD_HEADLESS)

FIND_PACKAGE(ZLIB REQUIRED)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(TM ${ZLIB_LIBRARIES})

IF(NOT BUILD_HEADLESS)
  IF(WIN32)
    SET(SFML_COMPONENTS system window graphics audio main)
  ELSE()
    SET(SFML_COMPONENTS system window graphics audio)
  ENDIF()
  FIND_PACKAGE(SFML 2 COMPONENTS ${SFML_COMPONENTS})
  IF(SFML_FOUND)
    IF(TARGET sfml-system)
      # SFML 2.5 and later ship a package config with imported targets.
      FOREACH(COMPONENT ${SFML_COMPONENTS})
        TARGET_LINK_LIBRARIES(TM sfml-${COMPONENT})
      ENDFOREACH()
    ELSE()
      # Older SFML releases use the FindSFML.cmake module.
      INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
      TARGET_LINK_LIBRARIES(TM ${SFML_LIBRARIES})
    ENDIF()
  ELSE()
    SET(BAD_GUI_REQS "${BAD_GUI_REQS} SFML")
  ENDIF()

  FIND_PACKAGE(GLEW)
  IF(${GLEW_FOUND})
    INCLUDE_DIRECTORIES(${GLEW_INCLUDE_PATH})
    TARGET_LINK_LIBRARIES(TM ${GLEW_LIBRARY})
  ELSE()
    SET(BAD_GUI_REQS "${BAD_GUI_REQS} GLEW")
  ENDIF()
ENDIF(NOT BUILD_HEADLESS)

MARK_AS_ADVANCED(SFML_INCLUDE_DIR)
MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)

IF(NOT "${BAD_GUI_REQS}" STREQUAL "")
  MESSAGE(FATAL_ERROR "Libraries missing for GUI-enabled build. Remember to set the PATH_ params if using non-standard locations.  The following libraries were not found:${BAD_GUI_REQS}")
ENDIF()

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(GLEW_BIN_ARCH "x64")
ELSE()
  SET(GLEW_BIN_ARCH "Win32")
ENDIF()

IF(WIN32 OR APPLE)
  SET(SEARCH_DIRS "${Qt5Core_DIR}/../../../bin/" "${Qt6Core_DIR}/../../../bin/" "${SFML_INCLUDE_DIR}/../bin" "${GLEW_INCLUDE_PATH}/../bin/Release/${GLEW_BIN_ARCH}" "${ZLIB_INCLUDE_DIR}/../bin" "${PATH_UCRT}")
  FILE(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fixup$<CONFIG>.cmake" CONTENT "
    INCLUDE(BundleUtilities)
    FIXUP_BUNDLE(\"$<TARGET_FILE:TM>\" \"\" \"${SEARCH_DIRS}\")")
  ADD_CUSTOM_TARGET(
    FIXUP
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/fixup$<CONFIG>.cmake"
    DEPENDS TM
    )
ENDIF()

INSTALL(TARGETS TM RUNTIME DESTINATION "${INSTALL_DIR}")

INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/Resources/" DESTINATION "${INSTALL_DIR}")

IF(WIN32 OR APPLE)
  INSTALL(CODE "
    INCLUDE(BundleUtilities)
    FIXUP_BUNDLE(\"\${CMAKE_INSTALL_PREFIX}/TM${CMAKE_EXECUTABLE_SUFFIX}\" \"\" \"${SEARCH_DIRS}\")")
ENDIF()

IF(Qt5_FOUND AND WIN32)
  INSTALL(FILES "${Qt5Widgets_DIR}/../../../plugins/platforms/qwindows.dll" DESTINATION platforms CONFIGURATIONS Release)
ELSEIF(Qt6_FOUND AND WIN32)
  INSTALL(FILES "${Qt6Widgets_DIR}/../../../plugins/platforms/qwindows.dll" DESTINATION platforms CONFIGURATIONS Release)
ENDIF()
