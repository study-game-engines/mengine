#include "VideoConverterFFMPEGToWEBM.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/Win32PlatformServiceExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ParamsHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToWEBM::VideoConverterFFMPEGToWEBM()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    VideoConverterFFMPEGToWEBM::~VideoConverterFFMPEGToWEBM()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToWEBM::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".webm" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoConverterFFMPEGToWEBM::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool VideoConverterFFMPEGToWEBM::convert()
    {
        Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String buffer = "-loglevel error -y -i \"" + full_input + "\" -codec:v libvpx -f webm -qmin 5 -qmax 15 -threads 8 -max_muxing_queue_size 1024 \"" + full_output + "\"";

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        String ffmpeg = Helper::getParam( m_options.params, STRINGIZE_STRING_LOCAL( "ffmpeg" ), "ffmpeg.exe" );

        uint32_t exitCode;
        if( win32Platform->createProcess( ffmpeg.c_str(), buffer.c_str(), true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer.c_str()
            );

            return false;
        }

        if( exitCode != 0 )
        {
            return false;
        }

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
}