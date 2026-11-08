cmake_minimum_required(VERSION 3.10)

SET(SRCS
"../game/BanList.h"		"../game/BanList.cpp"
"../game/BitPacking.h"	"../game/BitPacking.cpp"
"../game/CfgParse.h"	"../game/CfgParse.cpp"
"../game/Crypt.h"		"../game/Crypt.cpp"
"../game/CStr.h"		"../game/CStr.cpp"
"../game/Heartbeat.h"	"../game/Heartbeat.cpp"
"../game/Networking.h"	"../game/Networking.cpp"
"../game/Timer.h"		"../game/Timer.cpp"
"TMMaster.h"			"TMMaster.cpp"
"../game/Trees.h"
"../game/Directories.h"	"../game/Filesystem.h"
)

IF(WIN32)
  SET(SRCS ${SRCS} "../game/win32/Directories.cpp")
ELSE()
  SET(SRCS ${SRCS} "../game/Posix/Directories.cpp")
ENDIF()

ADD_EXECUTABLE(TMMaster ${SRCS})

IF(MSVC)
  SET_PROPERTY(TARGET TMMaster APPEND PROPERTY LINK_FLAGS /DEBUG)
ENDIF()

IF(WIN32)
  TARGET_LINK_LIBRARIES(TMMaster winmm.lib ws2_32.lib)
ENDIF()

INSTALL(TARGETS TMMaster RUNTIME DESTINATION "${INSTALL_DIR}")
