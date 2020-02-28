CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(MENGINE_BUILD_TYPE_DEBUG 1)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    SET(MENGINE_BUILD_TYPE_RELWITHDEBINFO 1)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(MENGINE_BUILD_TYPE_RELEASE 1)
ENDIF()

IF(MENGINE_BUILD_TYPE_DEBUG)
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "d")
ELSE()
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "")
ENDIF()

MACRO(SET_MENGINE_ENVIRONMENT MENGINE_TARGET MENGINE_RENDER MENGINE_ARCHITECTURE MENGINE_PLATFORM MENGINE_TOOLCHAIN)    
    SET(MENGINE_TARGET_WINDOWS OFF CACHE BOOL "MENGINE_TARGET_WINDOWS" FORCE)
    SET(MENGINE_TARGET_ANDROID OFF CACHE BOOL "MENGINE_TARGET_ANDROID" FORCE)
    SET(MENGINE_TARGET_MACOS OFF CACHE BOOL "MENGINE_TARGET_MACOS" FORCE)
    SET(MENGINE_TARGET_IOS OFF CACHE BOOL "MENGINE_TARGET_MACOS" FORCE)
    SET(MENGINE_TARGET_UNIX OFF CACHE BOOL "MENGINE_TARGET_UNIX" FORCE)

    SET(MENGINE_RENDER_OPENGL OFF CACHE BOOL "MENGINE_RENDER_OPENGL" FORCE)
    SET(MENGINE_RENDER_DIRECTX OFF CACHE BOOL "MENGINE_RENDER_DIRECTX" FORCE)

    SET(MENGINE_ARCHITECTURE_X86 OFF CACHE BOOL "MENGINE_ARCHITECTURE_X86" FORCE)
    SET(MENGINE_ARCHITECTURE_X64 OFF CACHE BOOL "MENGINE_ARCHITECTURE_X64" FORCE)

    SET(MENGINE_PLATFORM_SDL OFF CACHE BOOL "MENGINE_PLATFORM_SDL" FORCE)
    SET(MENGINE_PLATFORM_WIN32 OFF CACHE BOOL "MENGINE_PLATFORM_WIN32" FORCE)

    SET(MENGINE_TOOLCHAIN_MSVC OFF CACHE BOOL "MENGINE_TOOLCHAIN_MSVC" FORCE)
    SET(MENGINE_TOOLCHAIN_MINGW OFF CACHE BOOL "MENGINE_TOOLCHAIN_MINGW" FORCE)
    SET(MENGINE_TOOLCHAIN_GRADLE OFF CACHE BOOL "MENGINE_TOOLCHAIN_GRADLE" FORCE)
    SET(MENGINE_TOOLCHAIN_XCODE_MAC OFF CACHE BOOL "MENGINE_TOOLCHAIN_XCODE_MAC" FORCE)
    SET(MENGINE_TOOLCHAIN_XCODE_IOS OFF CACHE BOOL "MENGINE_TOOLCHAIN_XCODE_IOS" FORCE)
    SET(MENGINE_TOOLCHAIN_GCC OFF CACHE BOOL "MENGINE_TOOLCHAIN_GCC" FORCE)
    
    SET(MENGINE_TARGET_${MENGINE_TARGET} ON CACHE BOOL "MENGINE_TARGET" FORCE)
    SET(MENGINE_RENDER_${MENGINE_RENDER} ON CACHE BOOL "MENGINE_RENDER" FORCE)
    SET(MENGINE_ARCHITECTURE_${MENGINE_ARCHITECTURE} ON CACHE BOOL "MENGINE_ARCHITECTURE" FORCE)    
    SET(MENGINE_PLATFORM_${MENGINE_PLATFORM} ON CACHE BOOL "MENGINE_PLATFORM" FORCE)
    SET(MENGINE_TOOLCHAIN_${MENGINE_TOOLCHAIN} ON CACHE BOOL "MENGINE_TOOLCHAIN" FORCE)
    
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_TARGET_${MENGINE_TARGET})
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_RENDER_${MENGINE_RENDER})
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_ARCHITECTURE_${MENGINE_ARCHITECTURE})
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_PLATFORM_${MENGINE_PLATFORM})
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_TOOLCHAIN_${MENGINE_TOOLCHAIN})
        
    MESSAGE("*********************************************")
    MESSAGE("CMAKE VERSION = ${CMAKE_VERSION}")
    MESSAGE("*********************************************")
    MESSAGE("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
    MESSAGE("CMAKE_MAKE_PROGRAM = ${CMAKE_MAKE_PROGRAM}")
    MESSAGE("CMAKE_GENERATOR = ${CMAKE_GENERATOR}")
    MESSAGE("MENGINE_TARGET = ${MENGINE_TARGET}")
    MESSAGE("MENGINE_RENDER = ${MENGINE_RENDER}")
    MESSAGE("MENGINE_ARCHITECTURE = ${MENGINE_ARCHITECTURE}")
    MESSAGE("MENGINE_PLATFORM = ${MENGINE_PLATFORM}")
    MESSAGE("MENGINE_TOOLCHAIN = ${MENGINE_TOOLCHAIN}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_PROJECT_NAME = ${MENGINE_PROJECT_NAME}")
    MESSAGE("MENGINE_BUILD_ONLY_EXECUTABLE_MODULE: ${MENGINE_BUILD_ONLY_EXECUTABLE_MODULE}")
    MESSAGE("MENGINE_BUILD_MENGINE_MASTER_RELEASE: ${MENGINE_BUILD_MENGINE_MASTER_RELEASE}")
    MESSAGE("MENGINE_USE_PRECOMPILED_HEADER: ${MENGINE_USE_PRECOMPILED_HEADER}")
    MESSAGE("MENGINE_USE_PYTHON_FRAMEWORK: ${MENGINE_USE_PYTHON_FRAMEWORK}")
    MESSAGE("MENGINE_EXTERNAL_FRAMEWORK: ${MENGINE_EXTERNAL_FRAMEWORK}")
    MESSAGE("MENGINE_EXTERNAL_FRAMEWORK_NAME: ${MENGINE_EXTERNAL_FRAMEWORK_NAME}")
    MESSAGE("MENGINE_EXTERNAL_FRAMEWORK_SHARED: ${MENGINE_EXTERNAL_FRAMEWORK_SHARED}")
    MESSAGE("MENGINE_EXTERNAL_FRAMEWORK_SOURCE_PATH: ${MENGINE_EXTERNAL_FRAMEWORK_SOURCE_PATH}")
    MESSAGE("MENGINE_EXTERNAL_FRAMEWORK_TEMP_PATH: ${MENGINE_EXTERNAL_FRAMEWORK_TEMP_PATH}")
    MESSAGE("MENGINE_USE_SUBFOLDER: ${MENGINE_USE_SUBFOLDER}")
    MESSAGE("MENGINE_SUBFOLDER_NAME: ${MENGINE_SUBFOLDER_NAME}")
    MESSAGE("*********************************************")
ENDMACRO()

MACRO(SET_MENGINE_OUTPUT_DIRECTORY)
    get_directory_property( DirDefs COMPILE_DEFINITIONS )
    foreach( d ${DirDefs} )
        LIST(APPEND MENGINE_DEFINITIONS_SHA1_LIST "${d}: [${${d}}]")
    endforeach()
    
    STRING(SHA1 MENGINE_DEFINITIONS_SHA1 "${MENGINE_DEFINITIONS_SHA1_LIST}")

    if(UNIX AND NOT ANDROID)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${MENGINE_DEFINITIONS_SHA1})
        SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
    elseif(ANDROID)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}/${MENGINE_DEFINITIONS_SHA1})
        SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
    elseif(MINGW)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${MENGINE_DEFINITIONS_SHA1})
        SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
    elseif(APPLE)
        # output paths
        if(MENGINE_TARGET_MACOS)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${MENGINE_DEFINITIONS_SHA1})
            SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR})
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR})
        elseif(MENGINE_TARGET_IOS)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE}/${MENGINE_DEFINITIONS_SHA1})
            SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
        endif()
    else()
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1})
        SET(MENGINE_SHARED_PDB_OUTPUT_DIRECTORY ${MENGINE_PDB_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR})
    endif()

    MESSAGE("CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")    
    MESSAGE("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    MESSAGE("MENGINE_SHARED_PDB_OUTPUT_DIRECTORY: ${MENGINE_SHARED_PDB_OUTPUT_DIRECTORY}")
ENDMACRO()

MACRO(ADD_FILTER group_name)
    SOURCE_GROUP(${group_name} FILES ${ARGN})
    SET(SRC_FILES ${SRC_FILES} ${ARGN})
ENDMACRO()

# solution
MACRO(ADD_MENGINE_FRAMEWORK)
    IF(MENGINE_EXTERNAL_SOURCE)
        ADD_PLUGIN(MENGINE_PLUGIN_EXTERNALBOOTSTRAPPER ON OFF "MENGINE_PLUGIN_EXTERNALBOOTSTRAPPER")        
        
        add_definitions(-DMENGINE_EXTERNAL_SOURCE)
    ENDIF()
ENDMACRO()

MACRO(ADD_PLUGIN Plugin Toggle DLL MSG)
    OPTION(${Plugin}_FORCE_ENABLE ${MSG} OFF)
    OPTION(${Plugin}_FORCE_DISABLE ${MSG} OFF)
    
    IF(${Plugin}_FORCE_ENABLE)
        SET(${Plugin} ON CACHE BOOL ${MSG} FORCE)
    ELSEIF(${Plugin}_FORCE_DISABLE)
        SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
    ELSE()
        SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
    ENDIF()
    
    IF(${Plugin})
        SET(${Plugin}_DLL ${DLL} CACHE BOOL ${MSG} FORCE)
        
        IF(${Plugin}_DLL)
            SET(${Plugin}_STATIC OFF CACHE BOOL ${MSG} FORCE)
        ELSE()
            SET(${Plugin}_STATIC ON CACHE BOOL ${MSG} FORCE)
        ENDIF()
        
        add_definitions(-D${Plugin})
    ENDIF()
    
    IF(${Plugin})
        IF(${Plugin}_DLL)
            add_definitions(-D${Plugin}_DLL)
        ELSE()
            add_definitions(-D${Plugin}_STATIC)
        ENDIF()
    ENDIF()
    
    MESSAGE("PLUGIN: ${Plugin} = ${${Plugin}} [${${Plugin}_DLL}]")
ENDMACRO()

MACRO(CREATE_PRECOMPILED_HEADER)
  IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
	SET(PrecompiledHeader "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.h")
	SET(PrecompiledSource "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.cpp")
    SET(PrecompiledBinary "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
  ENDIF()
ENDMACRO()

MACRO(ADD_PRECOMPILED_HEADER)
  IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
	SET(PrecompiledHeader "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.h")
    SET(PrecompiledBinary "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${SRC_FILES}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           )
  ENDIF()
ENDMACRO()

MACRO(ADD_MENGINE_LIBRARY)
	ADD_PRECOMPILED_HEADER()

	ADD_LIBRARY(${MY_LIB_NAME} STATIC ${SRC_FILES})
	
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${MY_LIB_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} PrecompiledHeader)
    ENDIF()
    
    set(FILTER_FOLDER ${ARGN})
    list(LENGTH FILTER_FOLDER EXIST_FILTER_FOLDER)
    if(MENGINE_USE_SUBFOLDER)
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}
            )
        endif()
    else()
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        endif()
    endif()
    
    SET(APPLICATION_LINK_LIBRARIES ${APPLICATION_LINK_LIBRARIES} ${MY_LIB_NAME} PARENT_SCOPE)
ENDMACRO()

MACRO(ADD_MENGINE_SHARED)
	ADD_PRECOMPILED_HEADER()

	ADD_LIBRARY(${MY_LIB_NAME} SHARED ${SRC_FILES})
    
    SET_TARGET_PROPERTIES(${MY_LIB_NAME} PROPERTIES 
        COMPILE_PDB_NAME ${MY_LIB_NAME} 
        PDB_OUTPUT_DIRECTORY ${MENGINE_SHARED_PDB_OUTPUT_DIRECTORY}
    )
	
	SET_TARGET_PROPERTIES( ${MY_LIB_NAME} 
		PROPERTIES PREFIX ""
		)
	
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${MY_LIB_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} PrecompiledHeader)
    ENDIF()
    
    set(FILTER_FOLDER ${ARGN})
    list(LENGTH FILTER_FOLDER EXIST_FILTER_FOLDER)
    
    if(MENGINE_USE_SUBFOLDER)
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                    FOLDER ${MENGINE_SUBFOLDER_NAME}
                )
        endif()
    else()
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        endif()
    endif()
    
    SET(APPLICATION_DEPENDENCIES ${APPLICATION_DEPENDENCIES} ${MY_LIB_NAME} PARENT_SCOPE)
ENDMACRO()

MACRO(ADD_MENGINE_EXECUTABLE)
    ADD_EXECUTABLE(${MY_LIB_NAME} WIN32 ${SRC_FILES})
    
    set_target_properties(${MY_LIB_NAME} PROPERTIES 
        COMPILE_PDB_NAME ${MY_LIB_NAME} 
        PDB_OUTPUT_DIRECTORY ${MENGINE_SHARED_PDB_OUTPUT_DIRECTORY}
    )
    
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${MY_LIB_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} PrecompiledHeader)
    ENDIF()
ENDMACRO()

MACRO(ADD_MENGINE_PLUGIN PLUGIN_NAME)
    IF(${PLUGIN_NAME}_DLL)
        add_definitions(-DMENGINE_PLUGIN_DLL)
        ADD_MENGINE_SHARED()
    ELSE()
        ADD_MENGINE_LIBRARY()
    ENDIF()
    
    set(FILTER_FOLDER ${ARGN})
    list(LENGTH FILTER_FOLDER EXIST_FILTER_FOLDER)
    
    if(MENGINE_USE_SUBFOLDER)
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        else()
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/Plugins
            )
        endif()
    else()
        if(${EXIST_FILTER_FOLDER} GREATER 0)
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        else()
            set_target_properties (${MY_LIB_NAME} PROPERTIES
                FOLDER Plugins
            )
        endif()
    endif()
ENDMACRO()

if(APPLE)
  MESSAGE("CMAKE_OSX_SYSROOT: ${CMAKE_OSX_SYSROOT}")
  MESSAGE("CMAKE_OSX_DEPLOYMENT_TARGET: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

  macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
  endmacro()

  macro(ADD_APPLE_FRAMEWORK fwname)
    if(MENGINE_TARGET_MACOS)
      find_library(FRAMEWORK_${fwname}
          NAMES ${fwname}
          PATHS ${CMAKE_OSX_SYSROOT}/System/Library
          PATH_SUFFIXES Frameworks
          NO_DEFAULT_PATH)
      if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
          MESSAGE(ERROR ": Framework ${fwname} not found")
      else()
          TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${FRAMEWORK_${fwname}})
          MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
      endif()
    elseif(MENGINE_TARGET_IOS)
      find_library(FRAMEWORK_${fwname}
          NAMES ${fwname}
          PATHS ${CMAKE_OSX_SYSROOT}${CMAKE_OSX_DEPLOYMENT_TARGET}.sdk/System/Library
          PATH_SUFFIXES Frameworks
          NO_DEFAULT_PATH)
      if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
          MESSAGE(ERROR ": Framework ${fwname} not found")
      else()
          TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${FRAMEWORK_${fwname}})
          MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
        endif()
    endif()
  endmacro()
endif()