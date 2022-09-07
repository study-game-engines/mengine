#include "StaticWString.h"

#include "Kernel/Assertion.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        size_t staticWStringSize( const WChar * _buffer, size_t _capacity )
        {
            MENGINE_UNUSED( _capacity );

            size_t buffer_size = MENGINE_WCSLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size < _capacity );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        WChar staticWStringIndex( const WChar * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity );

            WChar c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringChange( WChar * _buffer, size_t _capacity, size_t _index, WChar _ch )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringAssign( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = MENGINE_WCSLEN( _value );
            }

            MENGINE_ASSERTION_FATAL( value_size < _capacity, "static wstring buffer assign '%ls' [size %zu]"
                , _value
                , value_size
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            MENGINE_WCSNCPY( _buffer, _value, value_size );
            _buffer[value_size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringAppend( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = MENGINE_WCSLEN( _value );
            }

            size_t buffer_size = MENGINE_WCSLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size + value_size < _capacity, "static string buffer '%ls' [size %zu] append '%ls' [size %zu]"
                , _buffer
                , buffer_size
                , _value
                , value_size
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            MENGINE_WCSNCAT( _buffer, _value, value_size );
            _buffer[buffer_size + value_size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringCopy( const WChar * _buffer, size_t _capacity, WChar * _value )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_WCSCPY( _value, _buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticWStringCompare( const WChar * _buffer, size_t _capacity, const WChar * _value )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            int32_t result = MENGINE_WCSNCMP( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}