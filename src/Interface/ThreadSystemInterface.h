#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class ThreadSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ThreadSystem" )

    public:
        virtual ThreadIdentityInterfacePtr createThread( const ConstString & _name, int32_t _priority, const Char * _doc ) = 0;
        virtual ThreadMutexInterfacePtr createMutex( const Char * _doc ) = 0;
        virtual ThreadConditionVariableInterfacePtr createConditionVariable( const Char * _doc ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual ptrdiff_t getCurrentThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SYSTEM()\
    ((Mengine::ThreadSystemInterface*)SERVICE_GET(Mengine::ThreadSystemInterface))
//////////////////////////////////////////////////////////////////////////