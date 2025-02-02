MENGINE_PROJECT(GameAnalyticsPlugin)

ADD_FILTER(
src
    GameAnalyticsPlugin.cpp
    GameAnalyticsPlugin.h
    GameAnalyticsPlugin.def
    
    GameAnalyticsInterface.h
    
    GameAnalyticsService.cpp
    GameAnalyticsService.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        GameAnalyticsScriptEmbedding.h
        GameAnalyticsScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/gameanalytics/source/gameanalytics)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_GAMEANALYTICS)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GameAnalytics${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/libcurl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/zlib${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Crypt32.lib)
ENDIF()