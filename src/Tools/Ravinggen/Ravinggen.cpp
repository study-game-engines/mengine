#include "Interface/PluginInterface.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/SecureServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/SHA1.h"

static void parse_arg( const std::wstring & _str, Mengine::WString & _value )
{
    _value = Mengine::WString( _str.begin(), _str.end() );
}

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include <string>
#include <vector>

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( Platform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorSystem, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PrototypeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_ERROR );

        LOGGER_SERVICE()
            ->setSilentMessageRelease( true );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( SecureService, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    try
    {
        if( Mengine::initializeEngine() == false )
        {
            message_error( "invalid initialize" );

            return EXIT_FAILURE;
        }
    }
    catch( const std::exception & se )
    {
        message_error( "Mengine exception %s"
            , se.what()
        );

        return EXIT_FAILURE;
    }

    PWSTR pwCmdLine = ::GetCommandLineW();

    Mengine::WString in = parse_kwds( pwCmdLine, L"--in", Mengine::WString() );
    Mengine::WString out = parse_kwds( pwCmdLine, L"--out", Mengine::WString() );
    Mengine::WString secure = parse_kwds( pwCmdLine, L"--secure", Mengine::WString() );

    if( in.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( out.empty() == true )
    {
        message_error( "not found 'out' param" );

        return EXIT_FAILURE;
    }

    if( secure.empty() == true )
    {
        message_error( "not found 'secure' param" );

        return EXIT_FAILURE;
    }

    Mengine::FilePath fp_in = Mengine::Helper::unicodeToFilePath( in );
    Mengine::FilePath fp_out = Mengine::Helper::unicodeToFilePath( out );

    const Mengine::FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
        ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

    if( fileGroup->existFile( fp_in, false ) == false )
    {
        message_error( "not found 'in' file: %s"
            , fp_in.c_str() 
        );

        return EXIT_FAILURE;
    }

    if( fileGroup->existFile( fp_out, false ) == false )
    {
        message_error( "not found 'out' file: %s"
            , fp_out.c_str()
        );

        return EXIT_FAILURE;
    }

    Mengine::MemoryInterfacePtr memory = Mengine::Helper::createMemoryFile( fileGroup, fp_in, false, false, MENGINE_DOCUMENT_FUNCTION );

    void * memoryBuffer = memory->getBuffer();
    size_t memorySize = memory->getSize();

    Mengine::Char utf8_secure[1024];
    size_t utf8_secure_len;
    Mengine::Helper::unicodeToUtf8( secure, utf8_secure, 1024, &utf8_secure_len );

    uint64_t sequreHash;
    Mengine::Helper::makeSHA1u64( utf8_secure, utf8_secure_len, &sequreHash );

    Mengine::Helper::ravingcode( sequreHash, memoryBuffer, memorySize, memoryBuffer );

    Mengine::OutputStreamInterfacePtr outputStream = Mengine::Helper::openOutputStreamFile( fileGroup, fp_out, true, MENGINE_DOCUMENT_FUNCTION );

    outputStream->write( "RGCD", 4 );
    outputStream->write( memoryBuffer, memorySize );

    message_error( "successful ravinggen" );

    return EXIT_SUCCESS;
}