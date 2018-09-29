set(EXTERNAL_PROJECT_NAME lz4)

set(EXTERNAL_LIBRARY_FILE_NAME lz4)

ExternalProject_Add(${EXTERNAL_PROJECT_NAME}_download PREFIX ${EXTERNAL_PROJECT_NAME}
        SOURCE_DIR ${DOWNLOADS_DIR}/${EXTERNAL_PROJECT_NAME}        

        GIT_REPOSITORY https://github.com/lz4/lz4.git
        GIT_TAG "v1.8.3"
        GIT_PROGRESS TRUE

        UPDATE_COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_DEPENDENCIES_DIR}/${EXTERNAL_PROJECT_NAME}/CMakeLists.txt
			${DOWNLOADS_DIR}/${EXTERNAL_PROJECT_NAME}/CMakeLists.txt     

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_DIR}/${EXTERNAL_PROJECT_NAME}
            -DPROJECT_CONFIGURE_DIR=${CMAKE_DEPENDENCIES_DIR}/${EXTERNAL_PROJECT_NAME}            
            -DTHIRDPARTY_DIR=${THIRDPARTY_DIR}
            -DBUILD_SHARED_LIBS=OFF
            -DLIBTYPE=STATIC
)

ExternalProject_Get_Property(${EXTERNAL_PROJECT_NAME}_download INSTALL_DIR)
set(EXTERNAL_PROJECT_INCLUDE_DIR ${INSTALL_DIR}/include)
set(EXTERNAL_PROJECT_LIBRARY_DIR ${INSTALL_DIR}/lib)

add_library(${EXTERNAL_PROJECT_NAME} STATIC IMPORTED)

set_target_properties(${EXTERNAL_PROJECT_NAME} PROPERTIES IMPORTED_LOCATION ${EXTERNAL_PROJECT_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${EXTERNAL_LIBRARY_FILE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
