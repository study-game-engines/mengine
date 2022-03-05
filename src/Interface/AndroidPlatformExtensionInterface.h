#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/String.h"

namespace Mengine
{
    class AndroidPlatformExtensionInterface
        : virtual public UnknownInterface
    {
    public:
        virtual JNIEnv * getJENV() const = 0;

        virtual jclass getJClassActivity() const = 0;
        virtual jobject getJObjectActivity() const = 0;

        virtual String getAndroidId() const = 0;
    };
}