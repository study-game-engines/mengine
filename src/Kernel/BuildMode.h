#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool isDebugMode();
        bool isDevelopmentMode();
        bool isQualityAssuranceMode();
        bool isBuildPublish();
        bool isMasterRelease();
        bool isWindowsDebug();
        const Char * getEngineGITSHA1();
        const Char * getEngineGITURL();
        const Char * getEngineGITBranch();
        const Char * getEngineGITDate();
        const Char * getContentCommit();
        const Char * getBuildProjectName();
        const Char * getBuildTimestamp();
        const Char * getBuildUsername();
        const Char * getBuildVersion();
        uint64_t getBuildNumber();
    }
}