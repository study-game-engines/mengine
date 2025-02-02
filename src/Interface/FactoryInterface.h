#pragma once

#include "Interface/Interface.h"

#include "Kernel/Document.h"
#include "Kernel/FactorablePointer.h"

#if defined(MENGINE_DEBUG)
#   include "Config/Lambda.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryInterface
        : public Interface
    {
    public:
        virtual const ConstString & getType() const = 0;

    public:
        virtual FactorablePointer createObject( const DocumentPtr & _doc ) = 0;
        virtual void destroyObject( Factorable * _object ) = 0;

    public:
        virtual bool isEmptyObjects() const = 0;
        virtual uint32_t getCountObject() const = 0;

#if defined(MENGINE_DEBUG)
    public:
        typedef Lambda<bool( Factorable * )> LambdaFactorable;
        virtual void foreachFactorables( const LambdaFactorable & _factorable ) = 0;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryInterface> FactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}