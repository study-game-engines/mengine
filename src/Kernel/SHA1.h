#pragma once

#include "Config/Config.h"
#include "Config/StdInt.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void makeSHA1( const void * _buffer, size_t _size, uint8_t * const _sha1 );
        void makeSHA1u64( const void * _buffer, size_t _size, uint64_t * const _u64 );
        void makeSHA1HEX( const void * _buffer, size_t _size, Char * const _hex );
        void makeSHA1Base64( const void * _buffer, size_t _size, Char * const _base64 );
    }
}