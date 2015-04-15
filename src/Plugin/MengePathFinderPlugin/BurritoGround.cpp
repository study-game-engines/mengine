#	include "BurritoGround.h"

#	include "Math/ccd.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::BurritoGround()
		: m_cb(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::~BurritoGround()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoGround::initialize( const mt::planef & _plane, PyObject * _cb )
	{
		m_plane = _plane;
		m_cb = _cb;

		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoGround::check_collision( float _timing, const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float & _collisionTiming, mt::vec2f & _factor ) const
	{
		float ccd_timing;
		if( mt::ccd_sphere_plane( _position, _radius, _velocity, m_plane, ccd_timing ) == false )
		{ 
			return false;
		}

		if( ccd_timing > _timing )
		{
			return false;
		}

		_collisionTiming = _timing - ccd_timing;

		PyObject * py_factor = pybind::ask( m_cb, "()"
			);

		_factor = pybind::extract<mt::vec2f>( py_factor );

		return true;
	}
}