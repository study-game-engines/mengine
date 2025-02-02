#include "cURLErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLErrorHelper::cURLErrorHelper( const Char * _file, uint32_t _line, const Char * _method )
        : m_file( _file )
        , m_line( _line )
        , m_method( _method )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLErrorHelper::operator == ( CURLcode _code ) const
    {
        if( _code == CURLE_OK )
        {
            return false;
        }

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "curl" ), LM_ERROR, LFILTER_NONE, LCOLOR_RED, nullptr, 0, LFLAG_SHORT )("[cURL] file '%s[%u]' call '%s' get error '%s' [%u]"
            , m_file
            , m_line
            , m_method
            , curl_easy_strerror( _code )
            , _code
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}