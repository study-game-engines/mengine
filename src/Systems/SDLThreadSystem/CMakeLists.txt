MENGINE_PROJECT(SDLThreadSystem)

ADD_FILTER(
src
	SDLThreadSystem.cpp
	SDLThreadSystem.h
	SDLThreadIdentity.cpp
    SDLThreadIdentity.h
   	SDLThreadIdentityRunner.cpp
    SDLThreadIdentityRunner.h
    SDLThreadProcessor.cpp
    SDLThreadProcessor.h
    SDLThreadMutex.h
    SDLThreadMutex.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include)

ADD_MENGINE_LIBRARY(Systems)