#include "PythonScriptWrapper.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/TextInterface.h"
#include "Interface/TimerInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UserdataInterface.h"
#include "Interface/ConfigInterface.h"

#include "Config/Typedef.h"
#include "Config/Stringstream.h"
#include "Config/Blobject.h"

#include "Kernel/Node.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/rand.h"

#include "Core/Polygon.h"
#include "Core/ColourValue.h"
#include "Core/MemoryHelper.h"
#include "Core/UID.h"

#include "Engine/Account.h"

#include "Engine/Movie.h"
#include "Engine/HotSpotShape.h"

#include "Logger/Logger.h"

#include "Engine/ResourceAnimation.h"

#include "Engine/Watchdog.h"

#include <ctime>
#include <sstream>
#include <iomanip>

#include "Kernel/ResourceImage.h"
#include "Engine/ResourceCursorICO.h"
#include "Engine/AccountManager.h"

#include "Kernel/Affector.h"

#include "Core/String.h"
#include "Core/CRC32.h"

#include "math/angle.h"

#include "Interface/ScriptSystemInterface.h"

#include "pybind/pybind.hpp"
#include "pybind/pickle.hpp"

#include <math.h>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class HelperScriptMethod
	{
    public:
        HelperScriptMethod()
        {
        }

    public:
        bool s_is_class( PyObject * _obj )
        {
			pybind::kernel_interface * kernel = pybind::get_kernel();

			return kernel->is_class( _obj );
        }
		
        const ConstString & s_getLanguagePack()
        {
            const ConstString & locale = APPLICATION_SERVICE()
				->getLocale();

			return locale;
        }

		float s_isometric_length_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
		{
			mt::vec3f iso_v0 = _v0;
			iso_v0.y *= 2.f;

			mt::vec3f iso_v1 = _v1;
			iso_v1.y *= 2.f;

			float iso_length = mt::length_v3_v3( iso_v0, iso_v1 );

			return iso_length;
		}

		float s_isometric_sqrlength_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
		{
			mt::vec3f iso_v0 = _v0;
			iso_v0.y *= 2.f;

			mt::vec3f iso_v1 = _v1;
			iso_v1.y *= 2.f;

			float iso_length = mt::sqrlength_v3_v3( iso_v0, iso_v1 );

			return iso_length;
		}

        WString s_utf8ToUnicode( const String & _utf8 )
        {
            WString unicode;
            Helper::utf8ToUnicode( _utf8, unicode );

            return unicode;
        }

        String s_unicodeToUtf8( const WString & _unicode )
        {
            String utf8;
            Helper::unicodeToUtf8( _unicode, utf8 );

            return utf8;
        }

		void s_debug()
		{
			printf("debug!\n");

#if defined(_MSC_VER) && !defined(I3D_ARCH_ARM) && !defined(_WIN64)
			_asm int 3;
#endif			
		}

		bool s_hasGameData( const ConstString & _name )
		{
			bool result = USERDATA_SERVICE()
				->hasUserdata( _name );

			return result;
		}

		bool s_writeGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _data, PyObject * _pickleTypes )
		{
			size_t size;
			pybind::pickle( _kernel, _data, _pickleTypes, nullptr, 0, size );

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( size, __FILE__, __LINE__ );

			if( buffer == nullptr )
			{
				return false;
			}

			void * memory_buffer = buffer->getMemory();
			size_t memory_size = buffer->getSize();

			if( pybind::pickle( _kernel, _data, _pickleTypes, memory_buffer, memory_size, size ) == false )
			{
				LOGGER_ERROR("s_writeGameData: data %s invalid pickle"
					, _name.c_str()
					);

				return false;
			}

			if( USERDATA_SERVICE()->writeUserdata( _name, memory_buffer, memory_size ) == false )
			{
				LOGGER_ERROR("s_writeGameData: data %s invalid write"
					, _name.c_str()
					);

				return false;
			}

			return true;
		}
		
		PyObject * s_loadGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _pickleTypes )
		{
			MemoryInterfacePtr binaryBuffer = USERDATA_SERVICE()
				->loadUserdata( _name );

			if( binaryBuffer == nullptr )
			{
				LOGGER_ERROR("s_readGameData: data %s invalid load"
					, _name.c_str()
					);

				return pybind::ret_none();
			}

			void * binaryBuffer_memory = binaryBuffer->getMemory();
			size_t binaryBuffer_size = binaryBuffer->getSize();

			PyObject * py_data = pybind::unpickle( _kernel, binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

			if( py_data == nullptr )
			{
				LOGGER_ERROR("s_readGameData: data %s invalid unpickle"
					, _name.c_str()
					);

				return pybind::ret_none();
			}

			return py_data;
		}

		bool s_isAltDown() const
		{
			return INPUT_SERVICE()
				->isAltDown();
		}

		bool s_isShiftDown() const
		{
			return INPUT_SERVICE()
				->isShiftDown();
		}

		bool s_isCtrlDown() const
		{
			return INPUT_SERVICE()
				->isCtrlDown();
		}

		bool s_isKeyDown( uint32_t _keyCode ) const
		{
			return INPUT_SERVICE()
				->isKeyDown( _keyCode );
		}

		bool s_isExclusiveKeyDown( uint32_t _keyCode ) const
		{
			return INPUT_SERVICE()
				->isExclusiveKeyDown( _keyCode );
		}

		bool s_isAnyKeyDown() const
		{
			return INPUT_SERVICE()
				->isAnyKeyDown();
		}

		bool s_isAnyMouseButtonDown() const
		{
			return INPUT_SERVICE()
				->isAnyMouseButtonDown();
		}

		bool s_isMouseButtonDown( uint32_t _button ) const
		{
			return INPUT_SERVICE()
				->isMouseButtonDown( _button );
		}

		void s_printChildren2( Node * _node, uint32_t _tab )
		{
			TListNodeChild & children = _node->getChildren();

			for( TSlugChild it(children); it.eof() == false; )
			{
				Node * child = *it;

				it.next_shuffle();

				ColourValue color;
				child->calcTotalColor( color );

				printf( "%.*s%s%s [%s] (%.0f, %.0f) %.2f\n"
					, _tab
					, "                                         "
					, child->isActivate() ? child->isEnable() ? "+" : "-" : "#"
					, child->getName().c_str()
					, child->getType().c_str()
					, child->getWorldPosition().x
					, child->getWorldPosition().y
					, color.getA()
					);

				s_printChildren2( child, _tab + 1 );
			}
		}

		void s_printChildren( Node * _node )
		{
			s_printChildren2( _node, 0 );
		}

		size_t s_getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group )
		{
            (void)_category;
            (void)_group;

			return 0U;
		}

		bool s_intersectPathVsCircle( const pybind::list & _path, const mt::vec3f & _position, float _radius )
		{
			size_t path_count = _path.size();

			if( path_count < 2 )
			{
				return false;
			}

			float dradius = _radius * _radius;

			for( size_t i = 1; i != path_count; ++i )
			{
				mt::vec3f v0 = _path[i - 1];
				mt::vec3f v1 = _path[i - 0];

				mt::vec3f vt;
				float d = mt::project_to_line_v3_v3( vt, v0, v1, _position );

				if( d < 0.f )
				{
					if( mt::sqrlength_v3_v3( v0, _position ) < dradius )
					{
						return true;
					}
				}
				else if( d > 1.f )
				{
					if( mt::sqrlength_v3_v3( v1, _position ) < dradius )
					{
						return true;
					}
				}
				else
				{
					if( mt::sqrlength_v3_v3( vt, _position ) < dradius )
					{
						return true;
					}
				}
			}

			return false;
		}

		pybind::object s_selectRandomPointFromPathRadius( pybind::kernel_interface * _kernel, const pybind::list & _path, const mt::vec3f & _position, float _radius )
		{
			size_t path_count = _path.size();

			float dradius = _radius * _radius;

			std::vector<mt::vec3f> points;

			for( size_t i = 0; i != path_count; ++i )
			{
				mt::vec3f v = _path[i];

				if( mt::sqrlength_v3_v3( v, _position ) > dradius )
				{
					continue;
				}

				points.emplace_back( v );
			}

			if( points.empty() == true )
			{
				return pybind::make_none_t( _kernel );
			}
			
			uint32_t size = (uint32_t)points.size();

			uint32_t index = mt::rand( size );

			const mt::vec3f & vr = points[index];

			return pybind::make_object_t( _kernel, vr );
		}

		PyObject * s_reloadModule( PyObject * _module )
		{
			return pybind::module_reload( _module );
		}

		String s_makeUID( uint32_t _length )
		{
			String uid = Helper::makeUID( _length );

			return uid;
		}

        void s_setCursorPosition( const mt::vec2f & _pos )
        {
            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentResolution();

            mt::vec2f adapt_pos;
            adapt_pos.x = _pos.x / contentResolution.getWidth() * currentResolution.getWidth();
            adapt_pos.y = _pos.y / contentResolution.getHeight() * currentResolution.getHeight();

            PLATFORM_SERVICE()
                ->setCursorPosition( adapt_pos );
        }

        bool s_isValidWindowMode()
        {
            //bool check = APPLICATION_SERVICE()
                //->isValidWindowMode();

            //return check;

            return true;
        }

		void s_addGlobalModule( const Char * _name, PyObject * _module )
        {
            SCRIPT_SERVICE()
                ->addGlobalModule( _name, _module );
        }

	public:
		uint32_t mt_enumerator()
		{
			static uint32_t i = 0;

			++i;

			return i;
		}

		int mt_rand( int a )
		{	
			return mt::rand( a );
		}

		float mt_randf( float a )
		{	
			return mt::randf( a );
		}

		int mt_range_rand( int a, int b )
		{	
			return mt::range_rand( a, b );
		}
		
		float mt_range_randf( float a, float b )
		{	
			return mt::range_randf( a, b );
		}

		float mt_around_randf( float a, float b )
		{
			float v_low = a - a * b;
			float v_hight = a + a * b;

			float v = mt_range_randf( v_low, v_hight );

			return v;
		}

		mt::vec2f mt_radius_randf( float _radius )
		{
			float rp = mt::randf( mt::constant::two_pi );
			float rr = mt::randf( 1.f );

			float rr2 = sqrtf(rr) * _radius;

			float x = ::cosf( rp );
			float y = ::sinf( rp );

			float xr = x * rr2;
			float yr = y * rr2;

			return mt::vec2f( xr, yr );
		}

		uint32_t mt_rounding( uint32_t _value, uint32_t _round )
		{
			uint32_t floor_value = _value / _round;
			uint32_t round_value = floor_value * _round;

			return round_value;
		}

		float mt_roundingf( float _value, float _round )
		{
			float floor_value = floorf(_value / _round);
			float round_value = floor_value * _round;

			return round_value;
		}

		float mt_sqrtf( float a )
		{
			return ::sqrtf(a);
		}

		float mt_powf( float a, float b )
		{
			return ::powf( a, b );
		}

		float mt_absf( float a )
		{
			return ::fabsf(a);
		}

		float mt_cosf( float a )
		{
			return ::cosf(a);
		}

		float mt_sinf( float a )
		{
			return ::sinf(a);
		}

		float mt_cosf_deg( float a )
		{
			return ::cosf( a * mt::constant::deg2rad );
		}

		float mt_sinf_deg( float a )
		{
			return ::sinf( a * mt::constant::deg2rad );
		}

		float mt_tanf( float a )
		{
			return ::tanf( a );
		}

		float mt_acosf( float a )
		{
			return ::acosf( a );
		}

		float mt_atanf( float _x )
		{
			return ::atanf( _x );
		}

		float mt_logf( float _x )
		{
			return ::logf( _x );
		}

		float mt_log10f( float _x )
		{
			return ::log10f( _x );
		}

		double mt_log10( double _x )
		{
			return ::log10( _x );
		}

		uint32_t mt_fibo( uint32_t _n )
		{
			if( _n == 0 )
			{
				return 0;
			}
			else if( _n == 1 )
			{
				return 1;
			}
			
			uint32_t fn = mt_fibo( _n - 1 ) + mt_fibo( _n - 2 );

			return fn;
		}

		uint64_t mt_fibo_bine( uint64_t _n )
		{
			const double nf = (double)_n;
			const double fplus = (1.0 + 2.2360679774997896964091736687313) * 0.5;
			const double fplusn = pow( fplus, nf );

			const double fminus = (1.0 - 2.2360679774997896964091736687313) * 0.5;
			const double fminusn = pow( fminus, nf );

			double fbine = (fplusn - fminusn) * 0.44721359549995793928183473374626;

			uint64_t fbine64 = (uint64_t)fbine;

			return fbine64;
		}

		float mt_dot_v2_v2( const mt::vec2f & _a, const mt::vec2f & _b )
		{
			float d = mt::dot_v2_v2( _a, _b );

			return d;
		}
		
		mt::vec2f mt_direction_v2_v2( const mt::vec2f & _from, const mt::vec2f & _to )
		{
			mt::vec2f direction;
			mt::dir_v2_v2( direction, _to, _from );

			return direction;
		}

		mt::vec3f mt_direction_v3_v3( const mt::vec3f & _from, const mt::vec3f & _to )
		{
			mt::vec3f direction;
			mt::dir_v3_v3( direction, _from, _to );

			return direction;
		}

		float mt_angle_from_v2_v2( const mt::vec2f & _from, const mt::vec2f & _to )
		{
			mt::vec2f direction;
			mt::sub_v2_v2( direction, _from, _to );

			mt::vec2f direction_n;
			mt::norm_v2_v2(direction_n, direction);

			float angle = mt::signed_angle( direction_n );

			return angle;
		}

		mt::vec2f mt_norm_v2( const mt::vec2f & _vec )
		{
			mt::vec2f v_n;
			mt::norm_v2_v2( v_n, _vec );

			return v_n;
		}

		mt::vec3f mt_norm_v3( const mt::vec3f & _vec )
		{
			mt::vec3f v_n;
			mt::norm_v3_v3( v_n, _vec );

			return v_n;
		}


		bool s_intersectsEllipseVsPoint( const mt::vec2f & _pos, float _radius, float _ellipse, const mt::vec2f & _point )
		{
			mt::vec2f v = _point - _pos;
			
			v.y /= _ellipse;

			float v_sqrdist = v.sqrlength();

			if( v_sqrdist > _radius * _radius )
			{
				return false;
			}

			return true;
		}

		bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
		{
			mt::box2f box_first(_minFirst, _maxFirst);
			mt::box2f box_second( _minSecond, _maxSecond );

			bool result = mt::is_intersect( box_first, box_second );

			return result;
		}

		mt::vec2f projectionPointToLine( const mt::vec2f & _point, const mt::vec2f & _v0, const mt::vec2f & _v1 )
		{
			mt::vec2f dir = _v1 - _v0;
			
			if( mt::dot_v2_v2(dir, dir) < 0.0001f )
			{
				return _v0;
			}

			mt::vec2f dir_norm;
			float dir_length = norm_v2_f(dir_norm, dir);

			mt::vec2f dir_point = _point - _v0;

			float dist = dot_v2_v2(dir_point, dir_norm);

			if( dist < 0.f )
			{
				dist = 0.f;
			}
			else if( dist > dir_length )
			{
				dist = dir_length;
			}

			mt::vec2f line_point = _v0 + dir_norm * dist;

			return line_point;
		}

		pybind::tuple s_angle_correct_interpolate_from_to( pybind::kernel_interface * _kernel, float _from, float _to )
		{
			float correct_angle_from;
			float correct_angle_to;

			mt::angle_correct_interpolate_from_to( _from, _to, correct_angle_from, correct_angle_to );

			pybind::tuple py_result = pybind::make_tuple_t( _kernel, correct_angle_from, correct_angle_to );

			return py_result;
		}

        float s_angle_between_two_vectors( const mt::vec2f & _v1, const mt::vec2f & _v2 )
        {
            mt::vec2f v1_norm;
            mt::norm_v2_v2( v1_norm, _v1 );

            mt::vec2f v2_norm;
            mt::norm_v2_v2( v2_norm, _v2 );

            float cos_v = mt::dot_v2_v2( v1_norm, v2_norm );

            float angle = acosf( cos_v );

            return angle;
        }

		pybind::list s_getPolygonPoints( pybind::kernel_interface * _kernel, const Polygon & _polygon )
		{
			pybind::list py_list( _kernel );

			const mt::vec2f * ring = _polygon.outer_points();
			size_t ring_count = _polygon.outer_count();

			py_list.append( ring, ring + ring_count );

			return py_list;
		}

		Polygon s_intersectionPolygons( Polygon _p1, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			TVectorPolygon output;
			_p1.difference( _p2, output );

			if( output.empty() == true )
			{
				return _p1;
			}

			Polygon inter = output[0];

			inter.correct();

			return inter;
		}

		bool s_intersectsPolygons( Polygon _p1, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			bool intersect = _p1.intersects( _p2 );

			return intersect;
		}

		bool s_intersectsPolygonsWM( const mt::mat4f & _wm1, Polygon _p1, const mt::mat4f & _wm2, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			Polygon p1wm;
			_p1.mul_wm( p1wm, _wm1 );

			Polygon p2wm;
			_p2.mul_wm( p2wm, _wm2 );
						
			bool intersect = p1wm.intersects( p2wm );

			return intersect;
		}

		bool s_intersectsPolygonsWMP( const mt::vec3f & _wm1, Polygon _p1, const mt::vec3f & _wm2, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			Polygon p1wm;
			_p1.transpose( p1wm, _wm1.to_vec2f() );

			Polygon p2wm;
			_p2.transpose( p2wm, _wm2.to_vec2f() );
						
			bool intersect = p1wm.intersects( p2wm );

			return intersect;
		}

		bool s_intersectsMoviesHotspot( Movie * _movie1, const ConstString & _socket1, Movie * _movie2, const ConstString & _socket2 )
		{
			if( _movie1 == nullptr )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie1 is NULL"
					);

				return false;
			}

			if( _movie2 == nullptr )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie2 is NULL"
					);

				return false;
			}

			if( _movie1->compile() == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie1 invalid compile"
					);

				return false;
			}

			if( _movie2->compile() == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie2 invalid compile"
					);

				return false;
			}

			NodePtr node1;
			MoviePtr submovie1;

			if( _movie1->hasMovieNode( _socket1, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node1, &submovie1 ) == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie1 %s not found socket shape %s"
					, _movie1->getName().c_str()
					, _socket1.c_str()
					);

				return false;
			}

			NodePtr node2;
			MoviePtr submovie2;

			if( _movie2->hasMovieNode( _socket2, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node2, &submovie2 ) == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie2 %s not found socket shape %s"
					, _movie2->getName().c_str()
					, _socket2.c_str()
					);

				return false;
			}

            HotSpotShapePtr shape1 = stdex::intrusive_static_cast<HotSpotShapePtr>(node1);

			if( shape1->compile() == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie1 %s socket shape %s invalid compile"
					, _movie1->getName().c_str()
					, _socket1.c_str()
					);

				return false;
			}

            HotSpotShapePtr shape2 = stdex::intrusive_static_cast<HotSpotShapePtr>(node2);

			if( shape2->compile() == false )
			{
				LOGGER_ERROR("s_intersectsMoviesHotspot movie2 %s socket shape %s invalid compile"
					, _movie2->getName().c_str()
					, _socket2.c_str()
					);

				return false;
			}

			const Resolution & contentResolution = APPLICATION_SERVICE()
				->getContentResolution();
			
			const RenderCameraInterfacePtr & shape1_camera = shape1->getRenderCameraInheritance();
			const RenderViewportInterfacePtr & shape1_viewport = shape1->getRenderViewportInheritance();

			mt::box2f b1;
			Polygon p1;
			shape1->getPolygonScreen( shape1_camera, shape1_viewport, contentResolution, &b1, &p1 );

			const RenderCameraInterfacePtr & shape2_camera = shape2->getRenderCameraInheritance();
			const RenderViewportInterfacePtr & shape2_viewport = shape2->getRenderViewportInheritance();

			mt::box2f b2;
			Polygon p2;
			shape2->getPolygonScreen( shape2_camera, shape2_viewport, contentResolution, &b2, &p2 );

			if( mt::is_intersect( b1, b2 ) == false )
			{
				return false;
			}
			
			bool result = s_intersectsPolygons( p1, p2 );

			return result;
		}

		bool s_intersectMoviesHotspotVsPolygon( const MoviePtr & _movie, const ConstString & _socket, Polygon _polygon )
		{
			_polygon.correct();

			if( _movie == nullptr )
			{
				LOGGER_ERROR("s_intersectMoviesHotspotVsPolygon movie is NULL"
					);

				return false;
			}

			if( _movie->compile() == false )
			{
				LOGGER_ERROR("s_intersectMoviesHotspotVsPolygon movie invalid compile"
					);

				return false;
			}

			NodePtr node;
			MoviePtr submovie;

			if( _movie->hasMovieNode( _socket, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node, &submovie ) == false )
			{
				LOGGER_ERROR("s_intersectMoviesHotspotVsPolygon movie %s not found socket shape %s"
					, _movie->getName().c_str()
					, _socket.c_str()
					);

				return false;
			}

            HotSpotShapePtr shape = stdex::intrusive_static_cast<HotSpotShapePtr>(node);

			if( shape->compile() == false )
			{
				LOGGER_ERROR("s_intersectMoviesHotspotVsPolygon movie %s socket shape %s invalid compile"
					, _movie->getName().c_str()
					, _socket.c_str()
					);

				return false;
			}

			const Resolution & contentResolution = APPLICATION_SERVICE()
				->getContentResolution();

			const RenderCameraInterfacePtr & shape_camera = shape->getRenderCameraInheritance();
			const RenderViewportInterfacePtr & shape_viewport = shape->getRenderViewportInheritance();

			Polygon p1;
			shape->getPolygonScreen( shape_camera, shape_viewport, contentResolution, nullptr, &p1 );

			bool result = s_intersectsPolygons( p1, _polygon );

			return result;
		}

#ifdef PYBIND_VISIT_OBJECTS
		class MyObjectVisits
			: public pybind::pybind_visit_objects
		{
		public:
			MyObjectVisits( const pybind::list & _py_list )
				: m_py_list(_py_list)
			{
			}

		protected:
			void visit( PyObject * _obj ) override
			{
				m_py_list.append( _obj );
			}

		protected:
			pybind::list m_py_list;
		};
#endif

		pybind::list s_objects( pybind::kernel_interface * _kernel )
		{
			pybind::list py_list( _kernel );

#ifdef PYBIND_VISIT_OBJECTS
			MyObjectVisits mov(py_list);
			pybind::visit_objects(&mov);
#endif

			return py_list;
		}

        class MyVisitorRenderTexture
            : public VisitorRenderTextureInterface
        {
        public:
			MyVisitorRenderTexture( const pybind::list & _list )
                : m_list(_list)
            {
            }

        protected:
            void visitRenderTexture( const RenderTextureInterfacePtr & _texture ) override
            {
                const FilePath & filePath = _texture->getFileName();

                m_list.append( filePath );
            }

        protected:
			pybind::list m_list;
        };

		pybind::list s_textures( pybind::kernel_interface * _kernel )
        {
			pybind::list py_list( _kernel );

            MyVisitorRenderTexture mvrt(py_list);
            RENDERTEXTURE_SERVICE()
                ->visitTexture( &mvrt );

            return py_list;
        }

		float s_watchdog( const String & _tag )
		{
			float watch = WATCHDOG( _tag );

			return watch;
		}

		bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
		{
			Polygon point_polygon;
			point_polygon.append(_point );
			point_polygon.correct();

			Polygon correct_polygon(_polygon);
			correct_polygon.correct();
			
			bool result = point_polygon.intersects( correct_polygon );

			return result;
		}

		uint64_t s_getTimeMs()
		{
			uint64_t ms = TIMER_SERVICE()
				->getMilliseconds();

			return ms;
		}

        uint64_t s_getTime()
        {
            uint64_t ms = s_getTimeMs();

            uint64_t s = ms / 1000;

            return s;
        }

        uint32_t s_getDate()
        {
            std::time_t ctTime; 
            std::time(&ctTime);

            return static_cast<uint32_t>(ctTime);
        }

		String s_getTimeString()
		{
			std::time_t ctTime;
			std::time(&ctTime);

			std::tm* sTime = std::localtime( &ctTime );

			Stringstream str;
			str << std::setw(2) << std::setfill('0') << sTime->tm_hour
				<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_min
				<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_sec;

			return str.str();
		}

		class MyAccountVisitorInterface
			: public AccountVisitorInterface
		{
		public:
			MyAccountVisitorInterface( TVectorConstString & _accounts )
				: m_accounts(_accounts)
			{
			}

		protected:
			void onAccount( const AccountInterfacePtr & _account ) override
			{
				const ConstString & name = _account->getID();

				m_accounts.emplace_back( name );
			}

		protected:
			TVectorConstString & m_accounts;

		private:
			void operator = ( const MyAccountVisitorInterface & )
			{
				//Empty
			}
		};

        TVectorConstString s_getAccounts()
		{
            TVectorConstString v_accounts;
			MyAccountVisitorInterface mav( v_accounts );

			ACCOUNT_SERVICE()
				->visitAccounts( &mav );

			return v_accounts;
		}

		bool s_addSetting( const ConstString & _setting, const WString & _defaultValue, const pybind::object & _applyFunc )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error addSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_addAccountSetting( accountID, _setting, _defaultValue, _applyFunc );
		}

		bool s_changeSetting( const ConstString & _setting, const WString & _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error changeSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSetting( accountID, _setting, _value );
		}

		bool s_changeSettingBool( const ConstString & _setting, bool _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error changeSettingBool: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingBool( accountID, _setting, _value );
		}

		bool s_changeSettingInt( const ConstString & _setting, int32_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("changeSettingInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingInt( accountID, _setting, _value );
		}

		bool s_changeSettingUInt( const ConstString & _setting, uint32_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("changeSettingUInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingUInt( accountID, _setting, _value );
		}

		bool s_changeSettingUInt64( const ConstString & _setting, uint64_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("changeSettingUInt64: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingUInt64( accountID, _setting, _value );
		}

		bool s_changeSettingFloat( const ConstString & _setting, float _value )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("changeSettingFloat: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingFloat( accountID, _setting, _value );
		}

		bool s_changeSettingStrings( const ConstString & _setting, const TVectorWString & _values )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error changeSettingStrings: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_changeAccountSettingStrings( accountID, _setting, _values );
		}

		bool s_addAccountSetting( const ConstString & _accountID, const ConstString & _setting, const WString & _defaultValue, const pybind::object & _applyFunc )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("addAccountSetting: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			bool result = account->addSetting( _setting, _defaultValue, _applyFunc );

			return result;
		}

		bool s_hasAccountSetting( const ConstString & _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("addAccountSetting: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			bool result = account->hasSetting( _setting );

			return result;
		}

		bool s_changeAccountSetting( const ConstString & _accountID, const ConstString & _setting, const WString & _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("s_changeAccountSetting: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("s_changeAccountSetting: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			bool result = account->changeSetting( _setting, _value );

			return result;
		}

		bool s_changeAccountSettingBool( const ConstString & _accountID, const ConstString & _setting, bool _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingBool: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingBool: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			if( _value == true )
			{
				setting_value = L"True";
			}
			else
			{
				setting_value = L"False";
			}

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}

		bool s_changeAccountSettingInt( const ConstString & _accountID, const ConstString & _setting, int32_t _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingBool: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingBool: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			Helper::intToWString( _value, setting_value );

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}

		bool s_changeAccountSettingUInt( const ConstString & _accountID, const ConstString & _setting, uint32_t _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingBool: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingBool: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			Helper::unsignedToWString( _value, setting_value );

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}

		bool s_changeAccountSettingUInt64( const ConstString & _accountID, const ConstString & _setting, uint64_t _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingBool: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingBool: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			Helper::unsigned64ToWString( _value, setting_value );

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}

		bool s_changeAccountSettingFloat( const ConstString & _accountID, const ConstString & _setting, float _value )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingBool: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingBool: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			Helper::floatToWString( _value, setting_value );

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}

		bool s_changeAccountSettingStrings( const ConstString & _accountID, const ConstString & _setting, const TVectorWString & _values )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("changeAccountSettingStrings: account not found '%s'"
					, _accountID.c_str()
					);

				return false;
			}

			if( account->hasSetting( _setting ) == false )
			{
				LOGGER_ERROR("changeAccountSettingStrings: account '%s' not found setting '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return false;
			}
			
			WString setting_value;

            for( const WString & value : _values )
			{
				if( setting_value.empty() == false )
				{
					setting_value += L" ,,, ";
				}

				setting_value += value;
			}

			bool result = account->changeSetting( _setting, setting_value );

			return result;
		}		

		bool s_addGlobalSetting( const ConstString & _setting, const WString & _defaultValue, const pybind::object & _applyFunc )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_addAccountSetting( accountID, _setting, _defaultValue, _applyFunc );
		}

		bool s_hasGlobalSetting( const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_hasAccountSetting( accountID, _setting );
		}

		bool s_changeGlobalSetting( const ConstString & _setting, const WString & _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSetting( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingBool( const ConstString & _setting, bool _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingBool( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingInt( const ConstString & _setting, int32_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingInt( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingUInt( const ConstString & _setting, uint32_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingUInt( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingUInt64( const ConstString & _setting, uint64_t _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingUInt64( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingFloat( const ConstString & _setting, float _value )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingFloat( accountID, _setting, _value );
		}

		bool s_changeGlobalSettingStrings( const ConstString & _setting, const TVectorWString & _values )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("addGlobalSetting: account not found"
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_changeAccountSettingStrings( accountID, _setting, _values );
		}		

        bool s_hasSetting( const ConstString & _setting )
        {
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error hasSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();
			
			return s_hasAccountSetting( accountID, _setting );
        }

		PyObject * s_getCurrentAccountUID( pybind::kernel_interface * _kernel )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getCurrentAccountUID: currentAccount is none [%s]"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountUID( _kernel, accountID );
		}

		PyObject * s_getSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

                return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSetting( _kernel, accountID, _setting );
		}

		PyObject * s_getSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingBool: currentAccount is none [%s]"
					, _setting.c_str()
					);

                return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingBool( _kernel, accountID, _setting );
		}

		PyObject * s_getSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingInt( _kernel, accountID, _setting );
		}

		PyObject * s_getSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingUInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

                return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingUInt( _kernel, accountID, _setting );
		}

		PyObject * s_getSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingUInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingUInt64( _kernel, accountID, _setting );
		}		

		PyObject * s_getSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingFloat: currentAccount is none [%s]"
					, _setting.c_str()
					);

                return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingFloat( _kernel, accountID, _setting );
		}

		PyObject * s_getSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingFloat: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingStrings( _kernel, accountID, _setting );
		}

		float s_getSettingFloatDefault( const ConstString & _setting, float _default )
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("getSettingFloat: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return _default;
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getCurrentAccountID();

			return s_getAccountSettingFloatDefault( accountID, _setting, _default );
		}

		bool s_getConfigBool( const Char * _section, const Char * _key, bool _default )
		{
			bool result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		int32_t s_getConfigInt( const Char * _section, const Char * _key, int32_t _default )
		{
			int32_t result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		uint32_t s_getConfigUInt( const Char * _section, const Char * _key, uint32_t _default )
		{
			uint32_t result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		uint64_t s_getConfigUInt64( const Char * _section, const Char * _key, uint64_t _default )
		{
			uint64_t result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		float s_getConfigFloat( const Char * _section, const Char * _key, float _default )
		{
			float result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		String s_getConfigString( const Char * _section, const Char * _key, const char * _default )
		{
			String result = CONFIG_SERVICE()
				->getValue( _section, _key, _default );

			return result;
		}

		
		PyObject * s_getAccountUID( pybind::kernel_interface * _kernel, const ConstString & _accountID )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSetting account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const String & value = account->getUID();

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}

		PyObject * s_getAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );
						
			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSetting account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}

		PyObject * s_getAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			(void)_kernel;

			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingBool account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			if( value == L"True" ||
				value == L"true" )
			{
				return pybind::ret_true();
			}

			if( value == L"False" ||
				value == L"false" )
			{
				return pybind::ret_false();
			}

			LOGGER_ERROR("getAccountSettingBool account '%s' setting '%s' value '%ls' is not bool [True|False]"
				, _accountID.c_str()
				, _setting.c_str()
				, value.c_str()
				);

			return pybind::ret_none();
		}

		PyObject * s_getAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("s_getAccountSettingInt account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			int32_t value;
			if( Helper::wstringToInt( setting, value ) == false )
			{
				LOGGER_ERROR("s_getAccountSettingInt account '%s' can't scanf from '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}
				
		PyObject * s_getAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingUInt account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			uint32_t value;
			if( Helper::wstringToUnsigned( setting, value ) == false )
			{
				LOGGER_ERROR("getAccountSettingUInt account '%s'can't scanf from '%s'"
                    , _accountID.c_str()
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}

		PyObject * s_getAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingUInt account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			uint64_t value;
			if( Helper::wstringToUnsigned64( setting, value ) == false )
			{
				LOGGER_ERROR("getAccountSettingUInt64 account '%s' can't scanf from '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}

		PyObject * s_getAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingUInt account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			if( setting.empty() == true )
			{
				pybind::list l( _kernel );

				return l.ret();
			}

			TVectorWString strings;
			Helper::wsplit( strings, setting, true, L" ,,, " );

			pybind::list l = pybind::make_list_container_t( _kernel, strings );

			return l.ret();
		}		

		PyObject * s_getAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingFloat account '%s' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			float value;
			if( Helper::wstringToFloat( setting, value ) == false )
			{
				LOGGER_ERROR("getAccountSettingFloat account '%s' can't scanf from '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( _kernel, value );

			return py_value;
		}

		float s_getAccountSettingFloatDefault( const ConstString& _accountID, const ConstString & _setting, float _default )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("getAccountSettingFloatDefault account '%s' is none"
					, _accountID.c_str()
					);

				return _default;
			}

			if( account->hasSetting( _setting ) == false )
			{
				return _default;
			}

			const WString & setting = account->getSetting( _setting );

			float value;
			if( Helper::wstringToFloat( setting, value ) == false )
			{
				LOGGER_ERROR("getAccountSettingFloatDefault account '%s' can't scanf from '%s'"
					, _accountID.c_str()
					, _setting.c_str()
					);

				return _default;
			}

			return value;
		}

		PyObject * s_getGlobalAccountUID( pybind::kernel_interface * _kernel )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountUID( _kernel, accountID );
		}

		PyObject * s_getGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();
			
			return s_getAccountSetting( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			(void)_kernel;

			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountSettingBool( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountSettingInt( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountSettingUInt( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountSettingUInt64( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			return s_getAccountSettingFloat( _kernel, accountID, _setting );
		}

		PyObject * s_getGlobalSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
		{
			if( ACCOUNT_SERVICE()
				->hasGlobalAccount() == false )
			{
				LOGGER_ERROR("getGlobalSetting account is none"
					);

				return pybind::ret_none();
			}

			const ConstString & accountID = ACCOUNT_SERVICE()
				->getGlobalAccountID();

			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( accountID );

			return s_getAccountSettingStrings( _kernel, accountID, _setting );
		}

		PyObject * s_createAccount( pybind::kernel_interface * _kernel )
		{
            AccountInterfacePtr account = ACCOUNT_SERVICE()
                ->createAccount();

            if( account == nullptr )
            {
				return pybind::ret_none();
            }
            
			const ConstString & accountId = account->getID();

			PyObject * py_value = pybind::ptr( _kernel, accountId );

            return py_value;
		}

		PyObject * s_createGlobalAccount( pybind::kernel_interface * _kernel )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->createGlobalAccount();

			if( account == nullptr )
			{
				return pybind::ret_none();
			}

			const ConstString & accountId = account->getID();

			PyObject * py_value = pybind::ptr( _kernel, accountId );

			return py_value;
		}

		void s_selectAccount( const ConstString& _accountID )
		{
			ACCOUNT_SERVICE()
				->selectAccount( _accountID );
		}
		
		bool s_hasCurrentAccount()
		{
			return ACCOUNT_SERVICE()
				->hasCurrentAccount();
		}

		void s_setDefaultAccount( const ConstString & _accountID )
		{
			ACCOUNT_SERVICE()
				->setDefaultAccount( _accountID );
		}

		void s_setGlobalAccount( const ConstString & _accountID )
		{
			ACCOUNT_SERVICE()
				->setGlobalAccount( _accountID );
		}		

		bool s_hasGlobalAccount()
		{
			return ACCOUNT_SERVICE()
				->hasGlobalAccount();
		}

		const ConstString & s_getGlobalAccountName()
		{
			return ACCOUNT_SERVICE()
				->getGlobalAccountID();
		}

		const ConstString & s_getDefaultAccount()
		{
			return ACCOUNT_SERVICE()
				->getDefaultAccountID();
		}

		bool s_hasDefaultAccount()
		{
			return ACCOUNT_SERVICE()
				->hasDefaultAccount();
		}

		bool s_isCurrentDefaultAccount()
		{
			return ACCOUNT_SERVICE()
				->isCurrentDefaultAccount();
		}

		bool s_selectDefaultAccount()
		{
			return ACCOUNT_SERVICE()
				->selectDefaultAccount();
		}
		
		void s_saveAccount()
		{
			//AccountInterface * currentAccount = ACCOUNT_SERVICE()
			//	->getCurrentAccount();

			//if( currentAccount == NULL )
			//{
			//	LOGGER_ERROR("Error saveCurrentAccount: currentAccount is none"
			//		);

			//	return;
			//}

			//currentAccount->save();
		}
	
		void s_saveAccounts()
		{
            ACCOUNT_SERVICE()
                ->saveAccounts();
		}

		void s_saveAccountsInfo()
		{
			//ACCOUNT_SERVICE()
			//	->saveAccountsInfo();
		}

		void s_deleteAccount( const ConstString& _accountName )
		{
			ACCOUNT_SERVICE()
				->deleteAccount( _accountName );
		}
				
        const ConstString & s_getCurrentAccountName()
		{
			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("Error getCurrentAccountName: currentAccount is none"
					);

				return ConstString::none();
			}

			const ConstString & name = ACCOUNT_SERVICE()
  				->getCurrentAccountID();

			return name;
		}

		bool s_writeAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountID, const WString & _fileName, PyObject * _data, PyObject * _pickleTypes )
		{
            String utf8_fileName;
            if( Helper::unicodeToUtf8( _fileName, utf8_fileName ) == false )
            {
                LOGGER_ERROR("s_writeAccountBinaryFile: invalid file '%s' convert to utf8"                    
                    , _fileName.c_str()
                    );

                return false;                     
            }

			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("writeAccountPickleFile: invalid account '%s'"
					, _accountID.c_str()
					);

				return false;
			}
			
			FilePath filepath = Helper::stringizeFilePath( utf8_fileName.c_str(), utf8_fileName.size() );

			size_t size;
			if( pybind::pickle( _kernel, _data, _pickleTypes, nullptr, 0, size ) == false )
			{
				LOGGER_ERROR("writeAccountPickleFile: account '%s' invalid get pickle size"
					, _accountID.c_str()
					);

				return false;
			}

			MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( size, __FILE__, __LINE__ );

			if( buffer == nullptr )
			{
				LOGGER_ERROR("writeAccountPickleFile: account '%s' invalid get memory for '%d' size"
					, _accountID.c_str()
					, size
					);

				return false;
			}

			void * memory_buffer = buffer->getMemory();
			size_t memory_size = buffer->getSize();
			
			if( pybind::pickle( _kernel, _data, _pickleTypes, memory_buffer, memory_size, size ) == false )
			{
				LOGGER_ERROR("writeAccountPickleFile: account '%s' invalid pickle"
					, _accountID.c_str()
					);

				return false;
			}

            if( account->writeBinaryFile( filepath, memory_buffer, memory_size ) == false )
            {
                LOGGER_ERROR("writeAccountPickleFile: account '%s' invalid write file '%s'"
                    , _accountID.c_str()
                    , _fileName.c_str()
                    );
                
                return false;
            }
      
			return true;
		}

		PyObject * s_loadAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountID, const WString & _fileName, PyObject * _pickleTypes )
		{
            String utf8_fileName;
            if( Helper::unicodeToUtf8( _fileName, utf8_fileName ) == false )
            {
                LOGGER_ERROR("loadAccountPickleFile: invalid convert filename %s to utf8"
                    , _fileName.c_str()
                    );

				return pybind::ret_none();
            }

			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("loadAccountPickleFile: invalid get account '%s'"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			FilePath filename = Helper::stringizeFilePath( utf8_fileName.c_str(), utf8_fileName.size() );

			MemoryInterfacePtr binaryBuffer = account->loadBinaryFile( filename );

			if( binaryBuffer == nullptr )
            {
                LOGGER_ERROR("loadAccountPickleFile: account %s invalid load file %s"
                    , _accountID.c_str()
                    , _fileName.c_str()
                    );

				return pybind::ret_none();
            }

			void * binaryBuffer_memory = binaryBuffer->getMemory();
			size_t binaryBuffer_size = binaryBuffer->getSize();
	
			PyObject * py_data = pybind::unpickle( _kernel, binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

			if( py_data == nullptr )
			{
				LOGGER_ERROR("loadAccountPickleFile: account %s invalid unpickle file %s"
					, _accountID.c_str()
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			return py_data;
		}

		bool s_hasAccountPickleFile( const ConstString & _accountID, const WString & _fileName )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE()
				->getAccount( _accountID );

			if( ACCOUNT_SERVICE()
				->hasCurrentAccount() == false )
			{
				LOGGER_ERROR("hasAccountPickleFile: invalid get account %s"
					, _accountID.c_str()
					);

				return false;
			}

			String utf8_fileName;
			if( Helper::unicodeToUtf8( _fileName, utf8_fileName ) == false )
			{
				LOGGER_ERROR("hasAccountPickleFile: invalid convert filename %s to utf8"
					, _fileName.c_str()
					);

				return false;
			}

			FilePath filename = Helper::stringizeFilePath( utf8_fileName.c_str(), utf8_fileName.size() );

			bool exist = account->hasBinaryFile( filename );

			return exist;
		}

		void s_setParticlesEnabled( bool _enable )
		{
			APPLICATION_SERVICE()
				->setParticleEnable( _enable );
		}

		bool s_hasTextByKey( const ConstString& _key )
		{
			bool value = TEXT_SERVICE()
				->existText( _key, nullptr );

			return value;
		}

		WString s_getTextByKey( const ConstString& _key )
		{
			TextEntryInterfacePtr entry;
			if( TEXT_SERVICE()
				->existText( _key, &entry ) == false )
			{
				pybind::throw_exception("Mengine.getTextByKey invalid get key %s"
					, _key.c_str()
					);
			}

			const String & text = entry->getValue();

            WString unicode;
            if( Helper::utf8ToUnicode(text, unicode ) == false )
            {
				pybind::throw_exception("Mengine.getTextByKey invalid text key '%s' convert '%s' to unicode"
					, _key.c_str()
					, text.c_str()
					);
            }

			return unicode;
		}

		uint32_t s_getTextCharCountByKey( const ConstString& _key )
		{
			TextEntryInterfacePtr entry;
			if( TEXT_SERVICE()
				->existText( _key, &entry ) == false )
			{				
				pybind::throw_exception("Mengine.getTextCharCountByKey invalid get key %s"
					, _key.c_str()
					);
			}

			const String & text = entry->getValue();

			uint32_t count = (uint32_t)text.size();

			return count;
		}

		void s_setVSync( bool _vSync )
		{
			APPLICATION_SERVICE()
				->setVSync( _vSync );
		}

		bool s_getVSync()
		{
			bool vsync = APPLICATION_SERVICE()
				->getVSync();

            return vsync;
		}

		void s_setCursorMode( bool _mode )
		{
			APPLICATION_SERVICE()
				->setCursorMode( _mode );
		}

		bool s_getCursorMode()
		{
			bool cursorMode = APPLICATION_SERVICE()
				->getCursorMode();

            return cursorMode;
		}

		bool s_setCursorIcon(const ConstString & _resourceName)
		{
			APPLICATION_SERVICE()
				->setCursorIcon( _resourceName );

			return true;
		}
    };

	//////////////////////////////////////////////////////////////////////////
	void PythonScriptWrapper::helperWrap()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		//srand( (unsigned)std::time( NULL ) );
        HelperScriptMethod * helperScriptMethod = new HelperScriptMethod();

  //      pybind::def_functor( "addInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearVector );
		//pybind::def_functor( "addInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearFloat);

		//pybind::def_functor( "addGlobalInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearVector );
		//pybind::def_functor( "addGlobalInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearFloat);		

		pybind::def_functor( kernel, "enumerator", helperScriptMethod, &HelperScriptMethod::mt_enumerator );

		pybind::def_functor( kernel, "rand", helperScriptMethod, &HelperScriptMethod::mt_rand );
		pybind::def_functor( kernel, "randf", helperScriptMethod, &HelperScriptMethod::mt_randf );
		pybind::def_functor( kernel, "range_rand", helperScriptMethod, &HelperScriptMethod::mt_range_rand );
		pybind::def_functor( kernel, "range_randf", helperScriptMethod, &HelperScriptMethod::mt_range_randf );
		pybind::def_functor( kernel, "around_randf", helperScriptMethod, &HelperScriptMethod::mt_around_randf );
		pybind::def_functor( kernel, "radius_randf", helperScriptMethod, &HelperScriptMethod::mt_radius_randf );

		pybind::def_functor( kernel, "rounding", helperScriptMethod, &HelperScriptMethod::mt_rounding );
		pybind::def_functor( kernel, "roundingf", helperScriptMethod, &HelperScriptMethod::mt_roundingf );
				
			
		pybind::def_functor( kernel, "sqrtf", helperScriptMethod, &HelperScriptMethod::mt_sqrtf );
		pybind::def_functor( kernel, "powf", helperScriptMethod, &HelperScriptMethod::mt_powf );
		pybind::def_functor( kernel, "absf", helperScriptMethod, &HelperScriptMethod::mt_absf );
		pybind::def_functor( kernel, "cosf", helperScriptMethod, &HelperScriptMethod::mt_cosf );
		pybind::def_functor( kernel, "sinf", helperScriptMethod, &HelperScriptMethod::mt_sinf );
		pybind::def_functor( kernel, "tanf", helperScriptMethod, &HelperScriptMethod::mt_tanf );
		pybind::def_functor( kernel, "cosf_deg", helperScriptMethod, &HelperScriptMethod::mt_cosf_deg );
		pybind::def_functor( kernel, "sinf_deg", helperScriptMethod, &HelperScriptMethod::mt_sinf_deg );
		pybind::def_functor( kernel, "acosf", helperScriptMethod, &HelperScriptMethod::mt_acosf );
		pybind::def_functor( kernel, "atanf", helperScriptMethod, &HelperScriptMethod::mt_atanf );
		pybind::def_functor( kernel, "logf", helperScriptMethod, &HelperScriptMethod::mt_logf );
		pybind::def_functor( kernel, "log10f", helperScriptMethod, &HelperScriptMethod::mt_log10f );
		pybind::def_functor( kernel, "log10", helperScriptMethod, &HelperScriptMethod::mt_log10 );
		pybind::def_functor( kernel, "fibo", helperScriptMethod, &HelperScriptMethod::mt_fibo );
		pybind::def_functor( kernel, "fibo_bine", helperScriptMethod, &HelperScriptMethod::mt_fibo_bine );		
		pybind::def_functor( kernel, "dot_v2_v2", helperScriptMethod, &HelperScriptMethod::mt_dot_v2_v2 );
		pybind::def_functor( kernel, "direction_v2_v2", helperScriptMethod, &HelperScriptMethod::mt_direction_v2_v2 );
		pybind::def_functor( kernel, "direction_v3_v3", helperScriptMethod, &HelperScriptMethod::mt_direction_v3_v3 );
		pybind::def_functor( kernel, "angle_from_v2_v2", helperScriptMethod, &HelperScriptMethod::mt_angle_from_v2_v2 );
        pybind::def_functor( kernel, "norm_v2", helperScriptMethod, &HelperScriptMethod::mt_norm_v2 );
		pybind::def_functor( kernel, "norm_v3", helperScriptMethod, &HelperScriptMethod::mt_norm_v3 );

        pybind::def_functor_kernel( kernel, "angle_correct_interpolate_from_to", helperScriptMethod, &HelperScriptMethod::s_angle_correct_interpolate_from_to );
        pybind::def_functor( kernel, "angle_between_two_vectors", helperScriptMethod, &HelperScriptMethod::s_angle_between_two_vectors );

		pybind::def_functor( kernel, "projectionPointToLine", helperScriptMethod, &HelperScriptMethod::projectionPointToLine );

		pybind::def_functor( kernel, "isPointInsidePolygon", helperScriptMethod, &HelperScriptMethod::s_isPointInsidePolygon );

		pybind::def_functor( kernel, "getTime", helperScriptMethod, &HelperScriptMethod::s_getTime );
        pybind::def_functor( kernel, "getTimeMs", helperScriptMethod, &HelperScriptMethod::s_getTimeMs );

		pybind::def_functor( kernel, "getDate", helperScriptMethod, &HelperScriptMethod::s_getDate );
        
        
		pybind::def_functor( kernel, "getTimeString", helperScriptMethod, &HelperScriptMethod::s_getTimeString );

		pybind::def_functor( kernel, "getConfigBool", helperScriptMethod, &HelperScriptMethod::s_getConfigBool );
		pybind::def_functor( kernel, "getConfigInt", helperScriptMethod, &HelperScriptMethod::s_getConfigInt );
		pybind::def_functor( kernel, "getConfigUInt", helperScriptMethod, &HelperScriptMethod::s_getConfigUInt );
		pybind::def_functor( kernel, "getConfigUInt64", helperScriptMethod, &HelperScriptMethod::s_getConfigUInt64 );
		pybind::def_functor( kernel, "getConfigFloat", helperScriptMethod, &HelperScriptMethod::s_getConfigFloat );
		pybind::def_functor( kernel, "getConfigString", helperScriptMethod, &HelperScriptMethod::s_getConfigString );

		pybind::def_functor( kernel, "getAccounts", helperScriptMethod, &HelperScriptMethod::s_getAccounts );
		
		pybind::def_functor_kernel( kernel, "getCurrentAccountUID", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountUID );

		pybind::def_functor_kernel( kernel, "getSetting", helperScriptMethod, &HelperScriptMethod::s_getSetting );
		pybind::def_functor_kernel( kernel, "getSettingBool", helperScriptMethod, &HelperScriptMethod::s_getSettingBool );
		pybind::def_functor_kernel( kernel, "getSettingInt", helperScriptMethod, &HelperScriptMethod::s_getSettingInt );
		pybind::def_functor_kernel( kernel, "getSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getSettingUInt );
		pybind::def_functor_kernel( kernel, "getSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getSettingUInt64 );
		pybind::def_functor_kernel( kernel, "getSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getSettingFloat );
		pybind::def_functor_kernel( kernel, "getSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getSettingStrings );
		pybind::def_functor( kernel, "getSettingFloatDefault", helperScriptMethod, &HelperScriptMethod::s_getSettingFloatDefault );

		pybind::def_functor( kernel, "addSetting", helperScriptMethod, &HelperScriptMethod::s_addSetting );
		pybind::def_functor( kernel, "hasSetting", helperScriptMethod, &HelperScriptMethod::s_hasSetting );
		pybind::def_functor( kernel, "changeSetting", helperScriptMethod, &HelperScriptMethod::s_changeSetting );
		pybind::def_functor( kernel, "changeSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeSettingBool );
		pybind::def_functor( kernel, "changeSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeSettingInt );
		pybind::def_functor( kernel, "changeSettingUInt", helperScriptMethod, &HelperScriptMethod::s_changeSettingUInt );
		pybind::def_functor( kernel, "changeSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeSettingUInt64 );
		pybind::def_functor( kernel, "changeSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeSettingFloat );
		pybind::def_functor( kernel, "changeSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeSettingStrings );

		pybind::def_functor_kernel( kernel, "getAccountUID", helperScriptMethod, &HelperScriptMethod::s_getAccountUID );

		pybind::def_functor_kernel( kernel, "getAccountSetting", helperScriptMethod, &HelperScriptMethod::s_getAccountSetting );
		pybind::def_functor_kernel( kernel, "getAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingBool );
		pybind::def_functor_kernel( kernel, "getAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingInt );
		pybind::def_functor_kernel( kernel, "getAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingUInt );
		pybind::def_functor_kernel( kernel, "getAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingUInt64 );
		pybind::def_functor_kernel( kernel, "getAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingFloat );
		pybind::def_functor_kernel( kernel, "getAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingStrings );
		pybind::def_functor( kernel, "getAccountSettingFloatDefault", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingFloatDefault );

		pybind::def_functor( kernel, "addAccountSetting", helperScriptMethod, &HelperScriptMethod::s_addAccountSetting );
		pybind::def_functor( kernel, "hasAccountSetting", helperScriptMethod, &HelperScriptMethod::s_hasAccountSetting );
		pybind::def_functor( kernel, "changeAccountSetting", helperScriptMethod, &HelperScriptMethod::s_changeAccountSetting );
		pybind::def_functor( kernel, "changeAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingBool );
		pybind::def_functor( kernel, "changeAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingInt );
		pybind::def_functor( kernel, "changeAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingUInt );
		pybind::def_functor( kernel, "changeAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingUInt64 );
		pybind::def_functor( kernel, "changeAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingFloat );
		pybind::def_functor( kernel, "changeAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingStrings );

		pybind::def_functor_kernel( kernel, "getGlobalAccountUID", helperScriptMethod, &HelperScriptMethod::s_getGlobalAccountUID );

		pybind::def_functor_kernel( kernel, "getGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_getGlobalSetting );
		pybind::def_functor_kernel( kernel, "getGlobalSettingBool", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingBool );
		pybind::def_functor_kernel( kernel, "getGlobalSettingInt", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingInt );
		pybind::def_functor_kernel( kernel, "getGlobalSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingUInt );
		pybind::def_functor_kernel( kernel, "getGlobalSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingUInt64 );
		pybind::def_functor_kernel( kernel, "getGlobalSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingFloat );
		pybind::def_functor_kernel( kernel, "getGlobalSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingStrings );

		pybind::def_functor( kernel, "addGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_addGlobalSetting );
		pybind::def_functor( kernel, "hasGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_hasGlobalSetting );
		pybind::def_functor( kernel, "changeGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSetting );
		pybind::def_functor( kernel, "changeGlobalSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingBool );
		pybind::def_functor( kernel, "changeGlobalSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingInt );
		pybind::def_functor( kernel, "changeGlobalSettingUint", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingUInt );
		pybind::def_functor( kernel, "changeGlobalSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingUInt64 );
		pybind::def_functor( kernel, "changeGlobalSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingFloat );
		pybind::def_functor( kernel, "changeGlobalSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingStrings );

		pybind::def_functor_kernel( kernel, "createAccount", helperScriptMethod, &HelperScriptMethod::s_createAccount );
		pybind::def_functor_kernel( kernel, "createGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_createGlobalAccount );

		pybind::def_functor( kernel, "selectAccount", helperScriptMethod, &HelperScriptMethod::s_selectAccount );
		pybind::def_functor( kernel, "deleteAccount", helperScriptMethod, &HelperScriptMethod::s_deleteAccount );
				
		pybind::def_functor( kernel, "saveAccount", helperScriptMethod, &HelperScriptMethod::s_saveAccount );
		pybind::def_functor( kernel, "saveAccounts", helperScriptMethod, &HelperScriptMethod::s_saveAccounts );
		pybind::def_functor( kernel, "saveAccountsInfo", helperScriptMethod, &HelperScriptMethod::s_saveAccountsInfo );
		pybind::def_functor( kernel, "hasCurrentAccount", helperScriptMethod, &HelperScriptMethod::s_hasCurrentAccount );
		pybind::def_functor( kernel, "getCurrentAccountName", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountName );
		
		pybind::def_functor( kernel, "setGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_setGlobalAccount );
		pybind::def_functor( kernel, "hasGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_hasGlobalAccount );
		pybind::def_functor( kernel, "getGlobalAccountName", helperScriptMethod, &HelperScriptMethod::s_getGlobalAccountName );

		pybind::def_functor( kernel, "setDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_setDefaultAccount );
		pybind::def_functor( kernel, "getDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_getDefaultAccount );
		pybind::def_functor( kernel, "hasDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_hasDefaultAccount );		
		pybind::def_functor( kernel, "isCurrentDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_isCurrentDefaultAccount );
		pybind::def_functor( kernel, "selectDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_selectDefaultAccount );

        pybind::def_functor_kernel( kernel, "writeAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_writeAccountPickleFile );
		pybind::def_functor_kernel( kernel, "loadAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_loadAccountPickleFile );
		pybind::def_functor( kernel, "hasAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_hasAccountPickleFile );

		pybind::def_functor( kernel, "setParticlesEnabled", helperScriptMethod, &HelperScriptMethod::s_setParticlesEnabled );

		//pybind::def_function( "unicode", &ScriptHelper::s_unicode );
		//pybind::def_function( "ansi", &ScriptHelper::s_ansi );

		
		pybind::def_functor( kernel, "hasTextByKey", helperScriptMethod, &HelperScriptMethod::s_hasTextByKey );
		pybind::def_functor( kernel, "getTextByKey", helperScriptMethod, &HelperScriptMethod::s_getTextByKey );
		pybind::def_functor( kernel, "getTextCharCountByKey", helperScriptMethod, &HelperScriptMethod::s_getTextCharCountByKey );
		

		pybind::def_functor( kernel, "setVSync", helperScriptMethod, &HelperScriptMethod::s_setVSync );
		pybind::def_functor( kernel, "getVSync", helperScriptMethod, &HelperScriptMethod::s_getVSync );
		pybind::def_functor( kernel, "setCursorMode", helperScriptMethod, &HelperScriptMethod::s_setCursorMode );
		pybind::def_functor( kernel, "getCursorMode", helperScriptMethod, &HelperScriptMethod::s_getCursorMode );
		pybind::def_functor( kernel, "setCursorIcon", helperScriptMethod, &HelperScriptMethod::s_setCursorIcon );

		pybind::def_functor( kernel, "intersectsEllipseVsPoint", helperScriptMethod, &HelperScriptMethod::s_intersectsEllipseVsPoint );
		pybind::def_functor( kernel, "intersectsBoxes", helperScriptMethod, &HelperScriptMethod::s_intersectsBoxes );

		pybind::def_functor_kernel( kernel, "getPolygonPoints", helperScriptMethod, &HelperScriptMethod::s_getPolygonPoints );
		pybind::def_functor( kernel, "intersectionPolygons", helperScriptMethod, &HelperScriptMethod::s_intersectionPolygons );
		pybind::def_functor( kernel, "intersectsPolygons", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygons );
		pybind::def_functor( kernel, "intersectsPolygonsWM", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWM );
		pybind::def_functor( kernel, "intersectsPolygonsWMP", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWMP );
		pybind::def_functor( kernel, "intersectsMoviesHotspot", helperScriptMethod, &HelperScriptMethod::s_intersectsMoviesHotspot );
		pybind::def_functor( kernel, "intersectMoviesHotspotVsPolygon", helperScriptMethod, &HelperScriptMethod::s_intersectMoviesHotspotVsPolygon );
		

		pybind::def_functor_kernel( kernel, "objects", helperScriptMethod, &HelperScriptMethod::s_objects );
        pybind::def_functor_kernel( kernel, "textures", helperScriptMethod, &HelperScriptMethod::s_textures );
		
		pybind::def_functor( kernel, "watchdog", helperScriptMethod, &HelperScriptMethod::s_watchdog );

		pybind::def_functor( kernel, "is_class", helperScriptMethod, &HelperScriptMethod::s_is_class );
		pybind::def_functor( kernel, "addGlobalModule", helperScriptMethod, &HelperScriptMethod::s_addGlobalModule );

        pybind::def_function( kernel, "angle_norm", &mt::angle_norm );
		pybind::def_function( kernel, "angle_delta_deg", &mt::angle_delta_deg );

		pybind::def_function( kernel, "length_v2", &mt::length_v2 );

		pybind::def_function( kernel, "length_v2_v2", &mt::length_v2_v2 );
		pybind::def_function( kernel, "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );

		pybind::def_function( kernel, "length_v3_v3", &mt::length_v3_v3 );
		pybind::def_function( kernel, "sqrlength_v3_v3", &mt::sqrlength_v3_v3 );

		pybind::def_function( kernel, "length_v3", &mt::length_v3 );
		pybind::def_function( kernel, "sqrlength_v3", &mt::sqrlength_v3 );

        pybind::def_function( kernel, "signed_angle", &mt::signed_angle );
        pybind::def_function( kernel, "angle_length", &mt::angle_length );
        pybind::def_function( kernel, "perp", &mt::perp );
        pybind::def_function( kernel, "perp_left", &mt::perp_left );

        pybind::def_function( kernel, "getrefcount", &pybind::refcount );
		pybind::def_function( kernel, "is_class", &pybind::is_class );
		pybind::def_function( kernel, "is_type_class", &pybind::is_type_class );
        pybind::def_function( kernel, "is_wrap", &pybind::is_wrap );

		pybind::def_functor( kernel, "getLanguagePack", helperScriptMethod, &HelperScriptMethod::s_getLanguagePack );

		pybind::def_functor( kernel, "isometric_length_v3_v3", helperScriptMethod, &HelperScriptMethod::s_isometric_length_v3_v3 );
		pybind::def_functor( kernel, "isometric_sqrlength_v3_v3", helperScriptMethod, &HelperScriptMethod::s_isometric_sqrlength_v3_v3 );

		pybind::def_functor( kernel, "isValidWindowMode", helperScriptMethod, &HelperScriptMethod::s_isValidWindowMode );

        pybind::def_functor( kernel, "utf8ToUnicode", helperScriptMethod, &HelperScriptMethod::s_utf8ToUnicode );
        pybind::def_functor( kernel, "unicodeToUtf8", helperScriptMethod, &HelperScriptMethod::s_unicodeToUtf8 );
        pybind::def_functor( kernel, "setCursorPosition", helperScriptMethod, &HelperScriptMethod::s_setCursorPosition );

		pybind::def_functor( kernel, "debug", helperScriptMethod, &HelperScriptMethod::s_debug );

		pybind::def_functor( kernel, "hasGameData", helperScriptMethod, &HelperScriptMethod::s_hasGameData );
		pybind::def_functor_kernel( kernel, "writeGameData", helperScriptMethod, &HelperScriptMethod::s_writeGameData );
		pybind::def_functor_kernel( kernel, "loadGameData", helperScriptMethod, &HelperScriptMethod::s_loadGameData );


		pybind::def_functor( kernel, "isAltDown", helperScriptMethod, &HelperScriptMethod::s_isAltDown );
		pybind::def_functor( kernel, "isShiftDown", helperScriptMethod, &HelperScriptMethod::s_isShiftDown );
		pybind::def_functor( kernel, "isCtrlDown", helperScriptMethod, &HelperScriptMethod::s_isCtrlDown );

		pybind::def_functor( kernel, "isKeyDown", helperScriptMethod, &HelperScriptMethod::s_isKeyDown );
		pybind::def_functor( kernel, "isExclusiveKeyDown", helperScriptMethod, &HelperScriptMethod::s_isExclusiveKeyDown );
		pybind::def_functor( kernel, "isAnyKeyDown", helperScriptMethod, &HelperScriptMethod::s_isAnyKeyDown );
		
		pybind::def_functor( kernel, "isAnyMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isAnyMouseButtonDown );
		pybind::def_functor( kernel, "isMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isMouseButtonDown );

		pybind::def_functor( kernel, "printChildren", helperScriptMethod, &HelperScriptMethod::s_printChildren );

		pybind::def_functor( kernel, "getGroupResourcesMemoryUse", helperScriptMethod, &HelperScriptMethod::s_getGroupResourcesMemoryUse );

		pybind::def_functor( kernel, "intersectPathVsCircle", helperScriptMethod, &HelperScriptMethod::s_intersectPathVsCircle );
		pybind::def_functor_kernel( kernel, "selectRandomPointFromPathRadius", helperScriptMethod, &HelperScriptMethod::s_selectRandomPointFromPathRadius );

		pybind::def_functor( kernel, "reloadModule", helperScriptMethod, &HelperScriptMethod::s_reloadModule );

		pybind::def_functor( kernel, "makeUID", helperScriptMethod, &HelperScriptMethod::s_makeUID );
			
	}
}
