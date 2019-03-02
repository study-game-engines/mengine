#pragma once

#ifndef MENGINE_MAX_PATH
#define MENGINE_MAX_PATH 260
#endif

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)X)
#endif

#ifndef NDEBUG
#define MENGINE_DEBUG
#endif

#ifndef NDEBUG
#ifdef WIN32
#define MENGINE_WINDOWS_DEBUG
#endif
#endif