MENGINE_PROJECT(AppleHelpshiftPlugin)

ADD_FILTER(
src
    AppleHelpshiftPlugin.h
    AppleHelpshiftPlugin.mm

    AppleHelpshiftInterface.h

    AppleHelpshiftService.h
    AppleHelpshiftService.mm
    
    AppleHelpshiftDelegate.h
    AppleHelpshiftDelegate.mm
)

# if(MENGINE_USE_SCRIPT_SERVICE)
#     ADD_FILTER(
#     embedding
#         AppleHelpshiftScriptEmbedding.h
#         AppleFacebookScriptEmbedding.cpp
#     )
    
#     INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
# endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_HELPSHIFT)

ADD_MENGINE_COCOAPOD("HelpshiftX" "NO-GIT" "10.2.1")

# if(MENGINE_USE_SCRIPT_SERVICE)
#     TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
# endif()
