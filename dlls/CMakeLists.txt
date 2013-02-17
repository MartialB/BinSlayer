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
	DyninstBin.hh
	DyninstBin.cpp
	CFG_Builder.hh
	CFG_Builder.cpp
	BB_Labelling.hh
	BB_Labelling.cpp
)

ADD_LIBRARY(
	${DLL_NAME}
	SHARED
	${DYNINST_SRCS}
)

IF(WIN32)
  TARGET_LINK_LIBRARIES(
    ${DLL_NAME}
    libparseAPI
    libcommon
    libsymtabAPI
    libinstructionAPI
    BeaEngine_s_d
)
ELSE()
  TARGET_LINK_LIBRARIES(
    ${DLL_NAME}
    parseAPI
    common
    symtabAPI
    instructionAPI
    dwarf
    elf
    BeaEngine_s_d
)  
ENDIF()

