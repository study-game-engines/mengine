#include "DevToDebugWidgetText.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::~DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::setConstText( const String & _text )
    {
        m_text = _text;
        m_getter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::setGetterTitle( const LambdaGetterText & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & DevToDebugWidgetText::calculateText() const
    {
        if( m_getter == nullptr )
        {
            return m_text;
        }

        m_getter( m_text );

        return m_text;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::_fillTypeJson( jpp::object & _jdata )
    {
        _jdata.set( "type", "text" );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::_fillDataJson( jpp::object & _jdata )
    {
        const String & text = this->calculateText();

        _jdata.set( "content", text );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::process( const jpp::object & _data )
    {
        MENGINE_UNUSED( _data );

        //Emty
    }
    //////////////////////////////////////////////////////////////////////////
}