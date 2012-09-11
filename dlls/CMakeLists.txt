CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0015 NEW)

PROJECT(BinaryHelper)

IF(WIN32)
  set(DLL_NAME "libBinaryHelper")
ELSE()
  set(DLL_NAME "BinaryHelper")
ENDIF()

INCLUDE_DIRECTORIES(${headers})
LINK_DIRECTORIES(${DYNINST_LIBS} ${BEAENGINE_LIBS})

SET(
	DYNINST_SRCS
	DyninstBin.cpp
	DyninstBin.hh
)

ADD_LIBRARY(
	${DLL_NAME}
	SHARED
	${DYNINST_SRCS}
)

IF(WIN32)
  TARGET_LINK_LIBRARIES(
    ${DLL_NAME}
    libdyninstAPI
    libparseAPI
    libcommon
    libsymtabAPI
    libinstructionAPI
    BeaEngine_s_d
)
ELSE()
  TARGET_LINK_LIBRARIES(
    ${DLL_NAME}
    dyninstAPI
    parseAPI
    common
    symtabAPI
    instructionAPI
    dwarf
    elf
    BeaEngine_s_d
)  
ENDIF()

