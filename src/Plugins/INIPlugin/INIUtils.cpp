#include "INIUtils.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/SecureServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Stringalized.h"
#include "Kernel/FileGroupHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdString.h"
#include "Config/StdIO.h"

namespace Mengine
{
    namespace INIUtils
    {
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
        {
            tinyini_initialize( &_ini.ini );

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "open ini file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            _ini.path = _filePath;

            bool successful = INIUtils::loadIni( _ini, stream, _doc );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
        {
            tinyini_initialize( &_ini.ini );

            size_t size = _stream->size();

            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            Char * memory_buffer = memory->newBuffer( size + 1 );

            _stream->read( memory_buffer, size );
            memory_buffer[size] = '\0';

            if( memory_buffer[0] == 'R' && memory_buffer[1] == 'G' && memory_buffer[2] == 'C' && memory_buffer[3] == 'D' )
            {
                memory_buffer += 4;

                uint64_t secureHash = SECURE_SERVICE()
                    ->getSecureHash();

                Helper::ravingcode( secureHash, memory_buffer, size - 4, memory_buffer );
            }

            if( tinyini_load( &_ini.ini, memory_buffer ) == TINYINI_RESULT_FAILURE )
            {
                LOGGER_ERROR( "ini invalid load '%s'"
                    , tinyini_get_error_message( &_ini.ini )
                );

                return false;
            }

            _ini.memory = memory;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasIniSection( const IniStore & _ini, const Char * _section )
        {
            if( tinyini_has_section( &_ini.ini, _section ) == TINYINI_RESULT_FAILURE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasIniValue( const IniStore & _ini, const Char * _section, const Char * _key )
        {
            if( tinyini_has_property( &_ini.ini, _section, _key ) == TINYINI_RESULT_FAILURE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, const Char ** const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            *_value = ini_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ConstString * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            *_value = Helper::stringizeString( ini_value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

#if defined(MENGINE_DEBUG)
            if( MENGINE_STRCHR( ini_value, '\\' ) != nullptr )
            {
                LOGGER_ERROR( "get ini '%s' filepath section '%s' key '%s' has invalid slash '%s'"
                    , _ini.path.c_str()
                    , _section
                    , _key
                    , ini_value
                );

                MENGINE_THROW_EXCEPTION( "get ini '%s' filepath section '%s' key '%s' has invalid slash '%s'"
                    , _ini.path.c_str()
                    , _section
                    , _key
                    , ini_value
                );
                
                return false;
            }
#endif

            const ConstString & cs_value = Helper::stringizeString( ini_value );

            *_value = FilePath( cs_value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Tags * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            Stringstream ss( ini_value );

            String single_string;
            while( ss >> single_string )
            {
                const Char * str_single_string = single_string.c_str();

                ConstString cstr_value = Helper::stringizeString( str_single_string );

                _value->addTag( cstr_value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t width;
            uint32_t height;
            if( MENGINE_SSCANF( ini_value, "%u %u", &width, &height ) != 2 )
            {
                LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse resolution"
                    , _section
                    , _key
                    , ini_value
                );

                return false;
            }

            _value->setWidth( width );
            _value->setHeight( height );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Color * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            float r;
            float g;
            float b;
            float a;
            if( MENGINE_SSCANF( ini_value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
            {
                LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse ColorValue"
                    , _section
                    , _key
                    , ini_value
                );

                return false;
            }

            float coef = 1.f / 255.f;

            r *= coef;
            g *= coef;
            b *= coef;
            a *= coef;

            _value->setRGBA( r, g, b, a );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int8_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int32_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = (int8_t)tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint8_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = (uint8_t)tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int32_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int32_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint32_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint32_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int64_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            int64_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint64_t * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            uint64_t tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            float tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, double * const _value )
        {
            const Char * ini_value = tinyini_get_property_value( &_ini.ini, _section, _key );

            if( ini_value == nullptr )
            {
                return false;
            }

            double tmp_value;
            if( Helper::stringalized( ini_value, &tmp_value ) == false )
            {
                return false;
            }

            *_value = tmp_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorString * const _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                _values->emplace_back( String( value ) );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorConstString * const _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                ConstString cs = Helper::stringizeString( value );

                _values->emplace_back( cs );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorFilePath * const _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                FilePath cs = Helper::stringizeFilePath( value );

                _values->emplace_back( cs );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values )
        {
            uint32_t count = tinyini_count_property_values( &_ini.ini, _section, _key );

            for( uint32_t index = 0; index != count; ++index )
            {
                const Char * ini_value = tinyini_get_property_values( &_ini.ini, _section, _key, index );

                AspectRatioViewport arv;
                if( MENGINE_SSCANF( ini_value, "%f %f %f %f %f %f"
                    , &arv.width
                    , &arv.height
                    , &arv.viewport.begin.x
                    , &arv.viewport.begin.y
                    , &arv.viewport.end.x
                    , &arv.viewport.end.y
                ) != 6 )
                {
                    LOGGER_ERROR( "section '%s' key '%s' value '%s' invalid parse aspect ration"
                        , _section
                        , _key
                        , ini_value
                    );

                    return;
                }

                _values->emplace_back( arv );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
