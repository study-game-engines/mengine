#pragma once

#include "FactoryPrototypeGenerator.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstString.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class DefaultPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        typedef IntrusivePtr<Type> TypePtr;

    public:
        DefaultPrototypeGenerator()
        {
        }

        ~DefaultPrototypeGenerator() override
        {
        }

    protected:
        FactoryPtr _initializeFactory() override
        {
            FactoryPtr factory = Helper::makeFactoryPool<Type, Count>();

            return factory;
        }

        void _finalizeFactory() override
        {
            //Empty
        }

    protected:
        FactorablePointer generate( const Char * _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            TypePtr object = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( object, nullptr, "can't generate '%s::%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , _doc
            );

#ifdef MENGINE_DEBUG
            const Char * doc = MENGINE_DOCUMENT( "Default create '%s'"
                , _doc );

            object->setDocument( doc );
#endif

            return object;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeDefaultPrototypeGenerator()
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<DefaultPrototypeGenerator<Type, Count>>();

            return generator;
        }
    }
}