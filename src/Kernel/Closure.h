#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/Lambda.h"
#include "Config/Tuple.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class P, class M, class Forwards>
    class Closure
    {
    public:
        Closure( P && _ptr, M _method, Forwards && _forwards )
            : m_ptr( _ptr )
            , m_method( _method )
            , m_forwards( std::forward<Forwards &&>( _forwards ) )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            return std::apply( m_method, std::tuple_cat( std::make_tuple( m_ptr ), std::tuple_cat( std::make_tuple( _args ... ), m_forwards ) ) );
        }

    protected:
        P m_ptr;
        M m_method;
        Forwards m_forwards;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class P, class D, class M, class ... Forwards>
        Closure<P, M, Tuple<Forwards ...>> closure( P && _ptr, M _method, Forwards && ... _args )
        {
            return Closure<P, M, Tuple<Forwards ...>>( _ptr, _method, std::make_tuple( _args ... ) );
        }
    }
}