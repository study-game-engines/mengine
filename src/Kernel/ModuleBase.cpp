#include "ModuleBase.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ModuleBase::ModuleBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBase::~ModuleBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModuleBase::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::avaliable()
	{
		bool successful = this->_avaliable();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::_avaliable()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::initialize()
	{
		bool successful = this->_initialize();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::finalize()
	{
		this->_finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::update( bool _focus )
	{
		this->_update( _focus );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_update( bool _focus )
	{
		(void)_focus;

		//Empty;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::tick( float _time, float _timing )
	{
		this->_tick( _time, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::render( const RenderContext * _state, uint32_t _debugMask )
	{
		this->_render( _state, _debugMask );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_tick( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_render( const RenderContext * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;

		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::message( const ConstString & _messageName, const TMapWParams & _params )
	{
		this->_message( _messageName, _params );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::messageAll( const ConstString & _messageName, const TMapWParams & _params )
	{
		this->_messageAll( _messageName, _params );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_message( const ConstString & _messageName, const TMapWParams & _params )
	{
		(void)_messageName;
		(void)_params;

		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_messageAll( const ConstString & _messageName, const TMapWParams & _params )
	{
		(void)_messageName;
		(void)_params;

		//Empty
	}
}