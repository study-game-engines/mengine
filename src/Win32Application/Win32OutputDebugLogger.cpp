#include "Win32OutputDebugLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OutputDebugLogger::Win32OutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OutputDebugLogger::~Win32OutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OutputDebugLogger::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 256 );
        ::OutputDebugStringA( timestamp );
        ::OutputDebugStringA( " " );

        ELoggerLevel level = _message.level;

        Char symbol = Helper::getLoggerLevelSymbol( level );
        Char symbol_str[] = {symbol, '\0'};
        ::OutputDebugStringA( symbol_str );
        ::OutputDebugStringA( " " );

        if( _message.category.empty() == false )
        {
            const Char * category_str = _message.category.c_str();

            ::OutputDebugStringA( "[" );
            ::OutputDebugStringA( category_str );
            ::OutputDebugStringA( "]" );
            ::OutputDebugStringA( " " );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s", size, data );

        ::OutputDebugStringA( message );
        ::OutputDebugStringA( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
}