#pragma once

#include "Config/Config.h"
#include "Config/StdInt.h"

#define LOGGER_LEVEL_COUNT (Mengine::LM_VERBOSE + 1)

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerLevel : uint32_t
    {
        LM_SILENT = 0,
        LM_FATAL,
        LM_MESSAGE_RELEASE,
        LM_ERROR,
        LM_WARNING,
        LM_MESSAGE,
        LM_INFO,
        LM_DEBUG,
        LM_VERBOSE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerColor : uint32_t
    {
        LCOLOR_NONE = 0 << 0,
        LCOLOR_RED = 1 << 1,
        LCOLOR_BLUE = 1 << 2,
        LCOLOR_GREEN = 1 << 3
    };
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerFilter : uint32_t
    {
        LFILTER_NONE = 0 << 0,
        LFILTER_PROTECTED = 1 << 0,
    };
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerFlag : uint32_t
    {
        LFLAG_NONE = 0 << 0,
        LFLAG_FUNCTIONSTAMP = 1 << 0,
        LFLAG_TIMESTAMP = 1 << 1,
        LFLAG_THREADSTAMP = 1 << 2,
        LFLAG_SYMBOL = 1 << 3,
        LFLAG_CATEGORY = 1 << 4,
        LFLAG_SHORT = LFLAG_TIMESTAMP | LFLAG_SYMBOL | LFLAG_CATEGORY,
        LFLAG_FULL = 0xFFFFFFFF
    };
    //////////////////////////////////////////////////////////////////////////
}
