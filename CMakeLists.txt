cmake_minimum_required(VERSION 3.28.1)

if (UNIX)
	set(CMAKE_C_COMPILER "/usr/bin/clang")
	set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

project(corto)

option(BUILD_CORTO_CODEC_UNITY "Build the unity codec shared library of corto" ON)
option(BUILD_CORTO_EXE "Build the command line binary of corto" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	# Essential to get to work with Unreal which also uses libc++
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -stdlib=libc++")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -stdlib=libc++")
endif()

if("${CMAKE_SYSTEM_NAME}" MATCHES "visionOS")
	set(VISIONOS TRUE)
	SET(VISIONOS_DEPLOYMENT_TARGET "1.0")
endif()

set(PLATFORM_INT "${PLATFORM}")

if("${CMAKE_SYSTEM_NAME}" MATCHES "visionOS")
	if(PLATFORM_INT STREQUAL "SIMULATOR_VISIONOS")
	  set(SDK_NAME xrsimulator)
	  message("Build using visionOS Simulator SDK")
	  if(NOT ARCHS)
	    set(ARCHS arm64)
	    set(APPLE_TARGET_TRIPLE_INT arm64-apple-xros${VISIONOS_DEPLOYMENT_TARGET}-simulator)
	  else()
	    set(APPLE_TARGET_TRIPLE_INT ${ARCHS_SPLIT}-apple-xros${VISIONOS_DEPLOYMENT_TARGET}-simulator)
	  endif()
	else()
		message("Build using visionOS Device SDK")
	endif()
endif()

if(DEFINED APPLE_TARGET_TRIPLE_INT)
  set(APPLE_TARGET_TRIPLE ${APPLE_TARGET_TRIPLE_INT} CACHE INTERNAL "")
  set(CMAKE_C_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
  set(CMAKE_CXX_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
  set(CMAKE_ASM_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
endif()


SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CORTO_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(CORTO_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/corto)

SET(LIB_HEADERS
	${CORTO_HEADER_PATH}/bitstream.h
	${CORTO_HEADER_PATH}/color_attribute.h
	${CORTO_HEADER_PATH}/corto.h
	${CORTO_HEADER_PATH}/cstream.h
	${CORTO_HEADER_PATH}/decoder.h
	${CORTO_HEADER_PATH}/encoder.h
	${CORTO_HEADER_PATH}/index_attribute.h
	${CORTO_HEADER_PATH}/normal_attribute.h
	${CORTO_HEADER_PATH}/point.h
	${CORTO_HEADER_PATH}/tunstall.h
	${CORTO_HEADER_PATH}/vertex_attribute.h
	${CORTO_HEADER_PATH}/zpoint.h
	${CORTO_SOURCE_PATH}/corto_codec.h)

SET(LIB_SOURCES
	${CORTO_SOURCE_PATH}/bitstream.cpp
	${CORTO_SOURCE_PATH}/color_attribute.cpp
	${CORTO_SOURCE_PATH}/cstream.cpp
	${CORTO_SOURCE_PATH}/decoder.cpp
	${CORTO_SOURCE_PATH}/encoder.cpp
	${CORTO_SOURCE_PATH}/normal_attribute.cpp
	${CORTO_SOURCE_PATH}/tunstall.cpp
	${CORTO_SOURCE_PATH}/corto_codec.cpp)

SET(EXE_SOURCE
	${CORTO_SOURCE_PATH}/main.cpp
	${CORTO_SOURCE_PATH}/meshloader.cpp
	${CORTO_SOURCE_PATH}/tinyply.cpp)

SET(EXE_HEADERS
	${CORTO_SOURCE_PATH}/meshloader.h
	${CORTO_SOURCE_PATH}/tinyply.h
	${CORTO_SOURCE_PATH}/objload.h)

SET(CORTO_DEFINITIONS "")

SET(IOS_DEPLOYMENT_TARGET "9.2")

if(MSVC)
	add_compile_options(/nologo /W3 -D_CRT_SECURE_NO_DEPRECATE)
else()
	add_compile_options(-Wall -pedantic)
endif()

ADD_LIBRARY(corto STATIC ${LIB_SOURCES} ${LIB_HEADERS})
ADD_LIBRARY(corto_dll SHARED ${LIB_SOURCES} ${LIB_HEADERS})

if (IOS)
set_target_properties(corto_dll PROPERTIES
	FRAMEWORK TRUE
	FRAMEWORK_VERSION C
	MACOSX_FRAMEWORK_IDENTIFIER com.evercoast.corto
#	MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	# "current version" in semantic format in Mach-O binary file
	VERSION 16.4.0
	# "compatibility version" in semantic format in Mach-O binary file
	SOVERSION 1.0.0
	PUBLIC_HEADER "${LIB_HEADERS}"
	XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
)
set_target_properties(corto_dll PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.evercoast.corto")
elseif(VISIONOS)
set_target_properties(corto_dll PROPERTIES
	FRAMEWORK TRUE
	FRAMEWORK_VERSION C
	MACOSX_FRAMEWORK_IDENTIFIER com.evercoast.corto
#	MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	# "current version" in semantic format in Mach-O binary file
	VERSION 16.4.0
	# "compatibility version" in semantic format in Mach-O binary file
	SOVERSION 1.0.0
	PUBLIC_HEADER "${LIB_HEADERS}"
	XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
)
set_target_properties(corto_dll PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.evercoast.corto")
endif()

SET(CORTOLIB_CFLAGS ${CORTO_CFLAGS})

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	LIST(APPEND CORTOLIB_CFLAGS -fPIC)
elseif(APPLE)
	LIST(APPEND CORTOLIB_CFLAGS -fvisibility=hidden)
	if(IOS)
		LIST(APPEND CORTOLIB_CFLAGS -fembed-bitcode)
		set_target_properties(corto PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
		set_target_properties(corto_dll PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
	elseif(VISIONOS)
		LIST(APPEND CORTOLIB_CFLAGS -fembed-bitcode)
		set_target_properties(corto PROPERTIES XCODE_ATTRIBUTE_VISIONOS_DEPLOYMENT_TARGET "${VISIONOS_DEPLOYMENT_TARGET}")
		set_target_properties(corto_dll PROPERTIES XCODE_ATTRIBUTE_VISIONOS_DEPLOYMENT_TARGET "${VISIONOS_DEPLOYMENT_TARGET}")
	endif()
endif()

target_include_directories(corto PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(corto PRIVATE ${CORTO_HEADER_PATH})
target_compile_definitions(corto PUBLIC ${CORTO_DEFINITIONS})
target_compile_options    (corto PUBLIC ${CORTOLIB_CFLAGS})
set_target_properties     (corto PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(corto_dll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(corto_dll PRIVATE ${CORTO_HEADER_PATH})
target_compile_definitions(corto_dll PUBLIC ${CORTO_DEFINITIONS})
target_compile_options    (corto_dll PUBLIC ${CORTOLIB_CFLAGS})
set_target_properties     (corto_dll PROPERTIES DEBUG_POSTFIX "d")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	# G++
	target_compile_options(corto_dll PRIVATE -Wall -Wextra)
	target_compile_options(corto PRIVATE -Wall -Wextra)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	# Clang
	target_compile_options(corto_dll PRIVATE -Wall -Wextra)
	target_compile_options(corto PRIVATE -Wall -Wextra)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	# MSVC
	target_compile_options(corto_dll PRIVATE /EHsc /MTd /W2 /c)
	target_compile_options(corto PRIVATE /EHsc /MTd /W2 /c)
	# Set the DLLEXPORT variable to export symbols
	target_compile_definitions(corto_dll PRIVATE WIN_EXPORT)
endif()

INSTALL(TARGETS corto
	RUNTIME DESTINATION bin
	BUNDLE DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

INSTALL(FILES
	${CORTO_HEADER_PATH}/bitstream.h
	${CORTO_HEADER_PATH}/color_attribute.h
	${CORTO_HEADER_PATH}/corto.h
	${CORTO_HEADER_PATH}/cstream.h
	${CORTO_HEADER_PATH}/decoder.h
	${CORTO_HEADER_PATH}/encoder.h
	${CORTO_HEADER_PATH}/index_attribute.h
	${CORTO_HEADER_PATH}/normal_attribute.h
	${CORTO_HEADER_PATH}/point.h
	${CORTO_HEADER_PATH}/tunstall.h
	${CORTO_HEADER_PATH}/vertex_attribute.h
	${CORTO_HEADER_PATH}/zpoint.h
	DESTINATION include/corto)

if (BUILD_CORTO_CODEC_UNITY)
	ADD_LIBRARY(cortocodec_unity SHARED ${LIB_SOURCES} ${LIB_HEADERS})
	target_include_directories(cortocodec_unity PUBLIC ${CORTO_HEADER_PATH})
	if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		# G++
		target_compile_options(cortocodec_unity PRIVATE -Wall -Wextra)
	elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
		# MSVC
		target_compile_options(cortocodec_unity PRIVATE /EHsc /MTd /W2 /c)
		# Set the DLLEXPORT variable to export symbols
		target_compile_definitions(cortocodec_unity PRIVATE WIN_EXPORT)
	endif()
endif()
if (BUILD_CORTO_EXE)
	ADD_EXECUTABLE(cortoexe ${EXE_SOURCE} ${EXE_HEADERS})
	SET(CORTOEXE_CFLAGS ${CORTO_CFLAGS})
	if(APPLE)
		if(IOS)
			set_target_properties(cortoexe PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
		elseif(VISIONOS)
			set_target_properties(cortoexe PROPERTIES XCODE_ATTRIBUTE_VISIONOS_DEPLOYMENT_TARGET "${VISIONOS_DEPLOYMENT_TARGET}")
		endif()
	endif()
	target_include_directories(cortoexe PUBLIC ${CORTO_HEADER_PATH})
	target_link_libraries(cortoexe PUBLIC corto)
	set_target_properties(cortoexe PROPERTIES OUTPUT_NAME "corto")

	INSTALL(TARGETS cortoexe
		RUNTIME DESTINATION bin
		BUNDLE DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
endif()
