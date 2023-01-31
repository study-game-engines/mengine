#include "Document.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Document::Document()
        : m_file( nullptr )
        , m_function( nullptr )
        , m_line( 0 )
    {
    }
    /////////////////////////////////////////////////////////////////////////
    Document::~Document()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setParent( const DocumentPtr & _parent )
    {
        m_parent = _parent;
    }
    //////////////////////////////////////////////////////////////////////////
    const DocumentPtr & Document::getParent() const
    {
        return m_parent;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setModulePath( const Char * _modulePath )
    {
        m_modulePath.assign( _modulePath );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getModulePath() const
    {
        const Char * modulePath_str = m_modulePath.c_str();

        return modulePath_str;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setFile( const Char * _file )
    {
        m_file = _file;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getFile() const
    {
        return m_file;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setFunction( const Char * _function )
    {
        m_function = _function;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getFunction() const
    {
        return m_function;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setLine( uint32_t _line )
    {
        m_line = _line;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Document::getLine() const
    {
        return m_line;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setMessage( const Char * _message )
    {
        m_message.assign( _message );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getMessage() const
    {
        const Char * message_str = m_message.c_str();

        return message_str;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Document::getMessageSize() const
    {
        String::size_type message_size = m_message.size();

        return (size_t)message_size;
    }
    //////////////////////////////////////////////////////////////////////////
}