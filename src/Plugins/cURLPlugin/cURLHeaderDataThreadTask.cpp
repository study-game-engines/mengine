#include "cURLHeaderDataThreadTask.h"

#include "cURLErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::cURLHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHeaderDataThreadTask::~cURLHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHeaderDataThreadTask::initialize( const String & _data )
    {
        m_data = _data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHeaderDataThreadTask::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "POST") );

        const String::value_type * data_buffer = m_data.c_str();
        String::size_type data_size = m_data.size();

        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDS, data_buffer) );
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDSIZE, (long)data_size) );

        this->setupWriteResponse( _curl );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            Stringstream ss;

            for( const String & header : m_headers )
            {
                ss << header;
                ss << std::endl;
            }

            String header_str = ss.str();

            LOGGER_STATISTIC( "HTTP: header data url '%s' header:\n %s"
                , m_url.c_str()
                , header_str.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHeaderDataThreadTask::_onCURLCleanup( CURL * _curl )
    {
        MENGINE_UNUSED( _curl );
    }
    //////////////////////////////////////////////////////////////////////////
}
