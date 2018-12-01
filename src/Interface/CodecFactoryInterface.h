#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CodecFactoryInterface
        : public Mixin
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
    };
    //////////////////////////////////////////////////////////////////////////		
    typedef IntrusivePtr<CodecFactoryInterface> CodecFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

