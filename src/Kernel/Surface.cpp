#include "Surface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Surface::Surface()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Surface::~Surface()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Surface::activate()
    {
        this->_activate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::deactivate()
    {
        this->_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Surface::update( const UpdateContext * _context )
    {
        bool successful = this->_update( _context );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_activate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_deactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Surface::_update( const UpdateContext * _context )
    {
        (void)_context;
        //Empty;

        return false;
    }
}