#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Vector.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NotificationArea
    {
    public:
        NotificationArea();
        ~NotificationArea();

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex );
        const ThreadMutexInterfacePtr & getMutex() const;

    public:
        bool initialize( uint32_t _id );
        void finalize();

    public:
        void addObserver( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc );
        void removeObserver( Observable * _observer );

    public:
        bool hasObserver( Observable * _observer ) const;

    public:
        bool foreachObservers( const NotificationServiceInterface::LambdaObserver & _lambda );

    protected:
        void addObserver_( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc );
        void removeObserver_( Observable * _observer );

    protected:
        bool hasObserver_( Observable * _observer ) const;
        void updateObservers_();

    protected:
        uint32_t m_id;

        ThreadMutexInterfacePtr m_mutex;

        struct ObserverDesc
        {
            Observable * observer;
            ObserverCallableInterfacePtr callable;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentPtr doc;
#endif
        };

        typedef Vector<ObserverDesc> VectorObservers;

        VectorObservers m_observers;
        VectorObservers m_addObservers;

        AtomicUInt32 m_visiting;
    };
}