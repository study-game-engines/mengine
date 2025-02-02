MENGINE_PROJECT(cURLPlugin)

ADD_FILTER(
src
    cURLPlugin.h
    cURLPlugin.cpp
    cURLPlugin.def
    
    cURLInterface.h
    
    cURLService.cpp
    cURLService.h
    
    cURLResponse.cpp
    cURLResponse.h
    
    cURLThreadTask.cpp
    cURLThreadTask.h
    cURLGetAssetThreadTask.cpp
    cURLGetAssetThreadTask.h
    cURLGetMessageThreadTask.cpp
    cURLGetMessageThreadTask.h
    cURLHeaderDataThreadTask.cpp
    cURLHeaderDataThreadTask.h
    cURLPostMessageThreadTask.cpp
    cURLPostMessageThreadTask.h
    cURLDeleteMessageThreadTask.cpp
    cURLDeleteMessageThreadTask.h
    
    cURLErrorHelper.cpp
    cURLErrorHelper.h
)
    
if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        cURLScriptEmbedding.cpp
        cURLScriptEmbedding.h
        
        PyCURLReceiver.cpp
        PyCURLReceiver.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

add_definitions(-DCURL_STATICLIB)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/curl/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/openssl/include)

if(MENGINE_PLUGIN_GOAP)
    INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_CURL)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/libcurl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Crypt32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Bcrypt.lib)
ENDIF()

IF(MENGINE_TARGET_UWP)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/libcurl${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}curl${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_IOS_SIMULATOR)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}curl${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_MACOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}curl${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_ANDROID)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}curl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ssl_1_1${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crypto_1_1${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}curl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ssl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}crypto${MENGINE_LIB_SUFFIX})
ENDIF()

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_GOAP)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})
    
    ADD_SUBDIRECTORY(Tasks)
endif()
