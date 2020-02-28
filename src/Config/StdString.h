#pragma once

#include <string.h>

#ifndef MENGINE_STRLEN
#define MENGINE_STRLEN(s) ::strlen(s)
#endif

#ifndef MENGINE_WCSLEN
#define MENGINE_WCSLEN(s) ::wcslen(s)
#endif

#ifndef MENGINE_STRCMP
#define MENGINE_STRCMP(a, b) ::strcmp(a, b)
#endif

#ifndef MENGINE_STRNCMP
#define MENGINE_STRNCMP(a, b, n) ::strncmp(a, b, n)
#endif

#ifndef MENGINE_STRRCHR
#define MENGINE_STRRCHR(s, c) ::strrchr(s, c)
#endif

#ifndef MENGINE_MEMCPY
#define MENGINE_MEMCPY(a, b, n) ::memcpy(a, b, n)
#endif

#ifndef MENGINE_STRICMP
#if defined(MENGINE_COMPILER_MSVC)
#   define MENGINE_STRICMP(a, b) _stricmp(a, b)
#elif defined(MENGINE_PLATFORM_LINUX) && defined(MENGINE_COMPILER_GCC)
#   define MENGINE_STRICMP(a, b) strcasecmp(a, b)
#else
#   define MENGINE_STRICMP(a, b) stricmp(a, b)
#endif
#endif