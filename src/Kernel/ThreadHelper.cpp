#include "ThreadHelper.h"

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/ThreadServiceInterface.h"


#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadJob.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            class MyThreadWorker
                : public ThreadWorkerInterface
                , public Factorable
            {
            public:
                MyThreadWorker( const LambdaThreadUpdate & _update, const LambdaThreadWorker & _worker )
                    : m_update( _update )
                    , m_worker( _worker )
                {
                }

                ~MyThreadWorker() override
                {
                }

            public:
                void onThreadWorkerUpdate( uint32_t _id ) override
                {
                    MENGINE_UNUSED( _id );

                    if( m_update != nullptr )
                    {
                        m_update();
                    }
                }

                bool onThreadWorkerWork( uint32_t _id ) override
                {
                    MENGINE_UNUSED( _id );

                    m_worker();

                    return true;
                }

                void onThreadWorkerDone( uint32_t _id ) override
                {
                    MENGINE_UNUSED( _id );
                }

            protected:
                LambdaThreadUpdate m_update;
                LambdaThreadWorker m_worker;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<MyThreadWorker, ThreadWorkerInterface> MyThreadWorkerPtr;
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool createSimpleThreadWorker( const ConstString & _threadName, EThreadPriority _priority, uint32_t _sleep, const LambdaThreadUpdate & _update, const LambdaThreadWorker & _worker, const DocumentPtr & _doc )
        {
            ThreadJobPtr threadJob = THREAD_SERVICE()
                ->createJob( _sleep, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( threadJob, "invalid create thread job" );

            if( THREAD_SERVICE()
                ->createThreadProcessor( _threadName, _priority, _doc ) == false )
            {
                return false;
            }

            if( THREAD_SERVICE()
                ->addTask( _threadName, threadJob, _doc ) == false )
            {
                return false;
            }

            Detail::MyThreadWorkerPtr worker = Helper::makeFactorableUnique<Detail::MyThreadWorker>( _doc, _update, _worker );

            UniqueId workerId = threadJob->addWorker( worker, _doc );

            if( workerId == INVALID_UNIQUE_ID )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroySimpleThreadWorker( const ConstString & _threadName )
        {
            THREAD_SERVICE()
                ->destroyThreadProcessor( _threadName );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}