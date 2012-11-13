CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0015 NEW)

PROJECT(BinSlayer)

SET(BinSlay_VERSION_MAJOR 1)
SET(BinSlay_VERSION_MINOR 0)

IF(WITH_GUI)
  FIND_PACKAGE(Qt4 REQUIRED)
  SET(GUI_PATH ./gui/)
  SET(
	qt_gui_headers
	${GUI_PATH}/MainWindow.h
	${GUI_PATH}/OpenBinaryView.h
	${GUI_PATH}/CallGraphLevelView.h
	${GUI_PATH}/CFGLevelView.h
	${GUI_PATH}/BasicBlockLevelView.h
	${GUI_PATH}/ShowFunctionView.h
	${GUI_PATH}/ShowBasicBlockView.h
  )
  SET(
	qt_gui_srcs
	${GUI_PATH}/ShowBasicBlockView.cpp
	${GUI_PATH}/ShowFunctionView.cpp
	${GUI_PATH}/BasicBlockLevelView.cpp
	${GUI_PATH}/CFGLevelView.cpp
	${GUI_PATH}/CallGraphLevelView.cpp
	${GUI_PATH}/OpenBinaryView.cpp
	${GUI_PATH}/MainWindow.cpp
	${GUI_PATH}/main.cpp
  )
  QT4_WRAP_CPP(qt_gui_headers_moc ${qt_gui_headers})
  INCLUDE(${QT_USE_FILE})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
ENDIF()

IF(WIN32)
	IF(DEBUG)
		SET(CMAKE_CXX_FLAGS "/W4 ${CMAKE_CXX_FLAGS}")
	ENDIF()
	SET(DYNINST_LIBS ./libs/windows/i386-unknown-nt4.0/lib/)	# dyninst libraries
ELSE()
	SET(CMAKE_CXX_FLAGS "-W -Wall -O2 -Wextra -std=c++0x ${CMAKE_CXX_FLAGS}")
	IF(DEBUG)
		SET(CMAKE_CXX_FLAGS " -g3 -DBINSLAYER_DEBUG ${CMAKE_CXX_FLAGS}")
	ENDIF()
	SET(DYNINST_LIBS ./libs/linux/i386-unknown-linux2.4/lib/)	# dyninst libraries
ENDIF()

IF(MEASURE_PERFORMANCE)
  SET(CMAKE_CXX_FLAGS "-DMEASURE_PERFORMANCE ${CMAKE_CXX_FLAGS}")
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(DLL_PATH ./dlls/)					# dlls dir
SET(CORE_PATH ./core/)					# core dir
SET(CORE_INCS_DIR ${CORE_PATH}/incs/)			# directory where core header files are
SET(CORE_HPP_DIR ${CORE_PATH}/hpp/)			# directory where core header templated files are
SET(CORE_SRCS_DIR ${CORE_PATH}/srcs/)			# directory where core source files are
SET(DLL_INCS_DIR ${DLL_PATH}/DllManager/)		# directory where dll header files are
SET(DYNINST_INCS ./libs/include/)			# dyninst includes
SET(BEAENGINE_INCS ./libs/)
SET(BEAENGINE_LIBS ./libs/beaengine/)

SET(headers
	${DYNINST_INCS}
	${CORE_INCS_DIR}
	${CORE_HPP_DIR}
	${DLL_INCS_DIR}
	${BEAENGINE_INCS}
	/home/egwene/Downloads/boost_1_50_0
)

INCLUDE_DIRECTORIES(${headers})
LINK_DIRECTORIES(${DYNINST_LIBS} ${BEAENGINE_LIBS})

# CORE headers
SET(CORE_INCS
	${CORE_INCS_DIR}/CallGraph.hh
	${CORE_INCS_DIR}/CFG.hh
	${CORE_INCS_DIR}/CallGraph.hh
	${CORE_INCS_DIR}/IBinary.hh
	${CORE_INCS_DIR}/Label.hh
	${CORE_INCS_DIR}/ANode.hh
	${CORE_INCS_DIR}/FctNode.hh
	${CORE_INCS_DIR}/BbNode.hh
	${CORE_INCS_DIR}/Hungarian.hh
	${CORE_INCS_DIR}/CG_Core.hh
	${CORE_INCS_DIR}/CFG_Core.hh
	${CORE_INCS_DIR}/BasicBlock_Core.hh
	${CORE_INCS_DIR}/InternalsCore.hh
	${CORE_INCS_DIR}/Function.hh
	${CORE_INCS_DIR}/BasicBlock.hh
	${CORE_INCS_DIR}/BinSlayer_Exception.hh
)

# CORE hpp
SET(CORE_HPP
	${CORE_HPP_DIR}/Bindiff.hpp
	${CORE_HPP_DIR}/BindiffSelector.hpp
	${CORE_HPP_DIR}/Crc32Selector.hpp
	${CORE_HPP_DIR}/SymNameSelector.hpp
	${CORE_HPP_DIR}/AGraph.hpp
	${CORE_HPP_DIR}/ISelector.hpp
	${CORE_HPP_DIR}/Isomorphes.hpp
	${CORE_HPP_DIR}/GraphED.hpp
	${CORE_HPP_DIR}/BinSlay.hpp
	${CORE_HPP_DIR}/IProperty.hpp
	${CORE_HPP_DIR}/UpProperty.hpp
	${CORE_HPP_DIR}/DownProperty.hpp
	${CORE_HPP_DIR}/ACore.hpp

)
# CORE sources
SET(CORE_SRCS
	${CORE_SRCS_DIR}/Hungarian.cpp
	${CORE_SRCS_DIR}/Label.cpp
	${CORE_SRCS_DIR}/CallGraph.cpp
	${CORE_SRCS_DIR}/CFG.cpp
	${CORE_SRCS_DIR}/CallGraph.cpp
	${CORE_SRCS_DIR}/ANode.cpp
	${CORE_SRCS_DIR}/FctNode.cpp
	${CORE_SRCS_DIR}/BbNode.cpp
	${CORE_SRCS_DIR}/CG_Core.cpp
	${CORE_SRCS_DIR}/CFG_Core.cpp
	${CORE_SRCS_DIR}/BasicBlock_Core.cpp
	${CORE_SRCS_DIR}/InternalsCore.cpp
	${CORE_SRCS_DIR}/Function.cpp
	${CORE_SRCS_DIR}/BasicBlock.cpp
	${CORE_SRCS_DIR}/BinSlayer_Exception.cpp
)

IF(WITH_GUI)
  SET(CORE_INCS ${CORE_INCS} ${CORE_HPP} ${qt_gui_headers})
  SET(CORE_SRCS ${CORE_SRCS} ${qt_gui_srcs} ${qt_gui_headers_moc})
ELSE()
  SET(CORE_SRCS ${CORE_SRCS} ${CORE_SRCS_DIR}/main.cpp)
ENDIF()

# DLL files
IF(WIN32)
SET(
	DLLS_LOADER
	${DLL_PATH}/DllManager/Windows/Dll.cpp
	${DLL_PATH}/DllManager/DLLmanager.cpp
	${DLL_PATH}/DllManager/DLLmanager.h
	${DLL_PATH}/DllManager/IDll.h	
	${DLL_PATH}/DllManager/Windows/Dll.h
)
ELSE()
SET(
	DLLS_LOADER
	${DLL_PATH}/DllManager/Linux/Dll.cpp
	${DLL_PATH}/DllManager/DLLmanager.cpp
	${DLL_PATH}/DllManager/DLLmanager.h
	${DLL_PATH}/DllManager/IDll.h	
	${DLL_PATH}/DllManager/Linux/Dll.h
)
ENDIF()

# CORE files
SET(
	CORE
	${CORE_INCS}
	${CORE_HPP}
	${CORE_SRCS}
	${DLLS_LOADER}
)

ADD_EXECUTABLE(BinSlayer ${CORE})

IF(WIN32)
  # target_link_libraries(bindiff_on_steroids)
ELSE()
  IF(WITH_GUI)
    TARGET_LINK_LIBRARIES(BinSlayer dl ${QT_LIBRARIES})
  ELSE()
    TARGET_LINK_LIBRARIES(BinSlayer dl rt)
  ENDIF()
ENDIF()

##
# Custom make commands
##
ADD_CUSTOM_TARGET(output-clean
    COMMAND rm -rf *.dot output/*.dot *.cg
    COMMENT "Cleaning *.dot generated files...")

ADD_CUSTOM_TARGET(cmake-clean
    COMMAND rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake
    COMMAND rm -rf Dlls/CMakeCache.txt Dlls/CMakeFiles Dlls/cmake_install.cmake    
    COMMENT "Cleaning cmake generated files...")

ADD_CUSTOM_TARGET(doxygen-generate
    COMMAND doxygen doxygen.conf
    COMMENT "Generating documentation with doxygen...")

ADD_CUSTOM_TARGET(doxygen-clean
    COMMAND rm -rf html latex
    COMMENT "Cleaning doxygen generated files...")

ADD_CUSTOM_TARGET(fclean
    COMMAND make clean
    COMMAND make doxygen-clean
    COMMAND make output-clean
    COMMAND make clean
    COMMAND make cmake-clean
    COMMAND rm -rf Makefile
    COMMENT "Cleaning all generated files...")

SUBDIRS(dlls)