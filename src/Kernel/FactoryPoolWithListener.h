#pragma once

#include "FactoryWithListener.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class T>
        class MethodFactoryDestroyListener
            : public FactoryDestroyListenerInterface
        {
        public:
            MethodFactoryDestroyListener( C * _self, M _method )
                : m_self( _self )
                , m_method( _method )
            {
            }

            ~MethodFactoryDestroyListener() override
            {
            }

        protected:
            void onFactoryDestroyObject( Factorable * _object ) override
            {
                T * obj = static_cast<T *>(_object);

                (m_self->*m_method)(obj);
            }

        protected:
            C * m_self;
            M m_method;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class T>
        FactoryDestroyListenerInterfacePtr makeFactoryDestroyListener( C * _class, M _method, const DocumentPtr & _doc )
        {
            return Helper::makeFactorableUnique<MethodFactoryDestroyListener<C, M, T> >( _doc, _class, _method );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C, class M>
        void setupFactoryDestroyListener( const FactoryWithListenerPtr & _factory, C * _class, M _method, const DocumentPtr & _doc )
        {
            FactoryDestroyListenerInterfacePtr destroyListener = Helper::makeFactorableUnique<MethodFactoryDestroyListener<C, M, T> >( _doc, _class, _method );

            _factory->setDestroyListener( destroyListener );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class C, class M>
        FactoryPtr makeFactoryPoolWithListener( C * _self, M _method, const DocumentPtr & _doc )
        {
            FactoryWithListenerPtr factory = Helper::makeFactoryPool<Type, Count, FactoryWithListener>( _doc );

            Helper::setupFactoryDestroyListener<Type>( factory, _self, _method, _doc );

            return factory;
        }
    }
}
