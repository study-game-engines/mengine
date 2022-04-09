#include "DevToDebugWidgetButton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::~DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::setTitle( const String & _title )
    {
        m_title = _title;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::setClickEvent( const LambdaClickEvent & _clickEvent )
    {
        m_clickEvent = _clickEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::_fillTypeJson( jpp::object & _jdata )
    {
        _jdata.set( "type", "button" );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::_fillDataJson( jpp::object & _jdata )
    {
        _jdata.set( "title", m_title );
    }
    ////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::process( const jpp::object & _data )
    {
        MENGINE_UNUSED( _data );

        if( m_clickEvent == nullptr )
        {
            return;
        }

        m_clickEvent();
    }
    ////////////////////////////////////////////////////////////////////////
}