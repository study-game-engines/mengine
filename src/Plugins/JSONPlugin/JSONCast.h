#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Color.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

namespace jpp
{
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::ConstString>
    {
        void operator()( const jpp::object & _obj, Mengine::ConstString * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::FilePath>
    {
        void operator()( const jpp::object & _obj, Mengine::FilePath * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Tags>
    {
        void operator()( const jpp::object & _obj, Mengine::Tags * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Resolution>
    {
        void operator()( const jpp::object & _obj, Mengine::Resolution * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Color>
    {
        void operator()( const jpp::object & _obj, Mengine::Color * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::AspectRatioViewport>
    {
        void operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::String>
    {
        void operator()( const jpp::object & _obj, Mengine::String * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
}