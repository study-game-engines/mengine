#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownInterface
        : public Mixin
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownInterface> UnknownInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}