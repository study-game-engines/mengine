#pragma once

#include "Interface/EventationInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Eventable.h"
#include "Kernel/Identity.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/String.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEventReceiver
        : public Mixin
    {
    public:
        PythonEventReceiver()
            : m_kernel( nullptr )
        {
        };

        ~PythonEventReceiver() override
        {
        };

    public:
        void initialize( pybind::kernel_interface * _kernel, const ConstString & _method, const pybind::object & _cb )
        {
            m_kernel = _kernel;
            m_method = _method;
            m_cb = _cb;
        }

    public:
        pybind::kernel_interface * getKernel() const
        {
            return m_kernel;
        }

        const ConstString & getMethod() const
        {
            return m_method;
        }

        const pybind::object & getCb() const
        {
            return m_cb;
        }

    protected:
        pybind::kernel_interface * m_kernel;
        ConstString m_method;
        pybind::object m_cb;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void assertPythonEventReceiver( const Factorable * _factorable, const pybind::dict & _kwds )
        {
            if( _kwds.empty() == true )
            {
                return;
            }

            for( pybind::dict::iterator
                it = _kwds.begin(),
                it_end = _kwds.end();
                it != it_end;
                ++it )
            {
                String k = it.key();

                LOGGER_ERROR( "node type '%s' invalid kwds '%s'"
                    , _factorable->getType().c_str()
                    , k.c_str()
                );
            }

            MENGINE_ERROR_FATAL( "invalid python event receiver type '%s'"
                , _factorable->getType().c_str()
            );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiver( pybind::kernel_interface * _kernel, const pybind::dict & _kwds, Eventable * const _eventable, const ConstString & _method, uint32_t _event, const DocumentPtr & _doc )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            if( _kwds.exist( _method ) == false )
            {
                return;
            }

            pybind::object py_event = _kwds[_method];

            if( py_event.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>( _doc );

                receiver->initialize( _kernel, _method, py_event );

                event->addEventReceiver( _event, receiver );
            }

#if defined(MENGINE_DEBUG)
            _kwds.remove( _method );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverModule( pybind::kernel_interface * _kernel, const pybind::module & _module, Eventable * const _eventable, const ConstString & _method, uint32_t _event, const DocumentPtr & _doc )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            //FixMe python2.7 don't support non native string key
            if( _module.has_attr( _method.c_str() ) == false )
            {
                return;
            }

            pybind::object py_method = _module.get_attr( _method.c_str() );

            if( py_method.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>( _doc );

                receiver->initialize( _kernel, _method, py_method );

                event->addEventReceiver( _event, receiver );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverMethod( pybind::kernel_interface * _kernel, const pybind::object & _obj, Eventable * const _eventable, const ConstString & _method, uint32_t _event, const DocumentPtr & _doc )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            //FixMe python2.7 don't support non native string key
            if( _obj.has_attr( _method.c_str() ) == false )
            {
                return;
            }

            pybind::object py_method = _obj.get_attr( _method.c_str() );

            if( py_method.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>( _doc );

                receiver->initialize( _kernel, _method, py_method );

                event->addEventReceiver( _event, receiver );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverMethod( pybind::kernel_interface * _kernel, const pybind::object & _obj, const EventablePtr & _eventable, const ConstString & _method, uint32_t _event, const DocumentPtr & _doc )
        {
            Helper::registerPythonEventReceiverMethod<T_Receiver>( _kernel, _obj, _eventable.get(), _method, _event, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
#   define MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER(Factorable, Kwds)\
    Helper::assertPythonEventReceiver(Factorable, Kwds)
#else
#   define MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER(Identity, Kwds)
#endif
//////////////////////////////////////////////////////////////////////////
