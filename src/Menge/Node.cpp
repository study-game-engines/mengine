#	include "Node.h"
#	include "Visitor.h"

#	include "Layer2D.h"

#	include "SceneManager.h"

#	include "RenderEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"
#	include "XmlEngine.h"

#	include "Player.h"
#	include "Camera2D.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

#	include "Material.h"
#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "NodeAffector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_active(false)
		, m_enable(true)
		, m_updatable(true)
		, m_state(NODE_IDLE)
		, m_parent(0)
		, m_listener(0)
		, m_layer(0)
		, m_colorLocal( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_colorWorld( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_invalidateColor( true )
		, m_debugMaterial( NULL )
		, m_angularSpeed( 0.0f )
		, m_linearSpeed( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node::~Node()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
		this->release();

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->destroy();
		}

		Holder<RenderEngine>::hostage()
			->releaseMaterial( m_debugMaterial );

		ScriptEngine::decref( m_listener );
	}	//////////////////////////////////////////////////////////////////////////
	void Node::visit( Visitor * _visitor )
	{
		_visitor->visit_impl( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::visitChildren( Visitor * _visitor )
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->visit( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::activate()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->activate();
		}

		if( m_active )
		{
			return true;
		}

		if( isCompile() == false )
		{
			if( compile() == false )
			{
				/*MENGE_LOG( MENGE_TEXT("Error: activation of Node '%s' is failed, because compilation is failed\n")
					, m_name.c_str() 
					);*/

				return false;
			}
		}

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
	{
		if( m_state == NODE_UPDATING )
		{
			m_state = NODE_DEACTIVATING;
			return;
		}

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->deactivate();
		}

		if( m_active )
		{
			m_active = false;
			_deactivate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::enable()
	{
		m_enable = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::disable()
	{
		m_enable = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setParent( Node * _parent )
	{
		m_parent = _parent;
	}
	//////////////////////////////////////////////////////////////////////////
	Node* Node::getParent()
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren( Node * _node )
	{
		return addChildren_( _node, m_children.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildrenFront( Node* _node )
	{
		return addChildren_( _node, m_children.begin() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::addChildren_( Node * _node, TContainerChildren::iterator _insert )
	{
		/*if( this->isChildren( _node, false ) )
		{
			//MENGE_LOG_ERROR( "Node \"%s\" type \"%s\" addChildren failed \"%s\" because type \"%s\" is already exist"
			//, this->getName().c_str()
			//, this->getType().c_str()
			//, _node->getName().c_str()
			//, _node->getType().c_str() );
			return false;
		}*/
		if( _node->getParent() == this )
		{
			TContainerChildren::iterator it_find 
				= std::find( m_children.begin(), m_children.end(), _node );
			assert( it_find != m_children.end() );
			m_children.splice( _insert, m_children, it_find );
		}
		else
		{

			Node * parent = _node->getParent();

			if( parent )
			{
				parent->removeChildren( _node );
			}

			_node->setParent( this );
			_node->setLayer( m_layer );

			m_children.insert( _insert, _node );

			_node->invalidateWorldMatrix();

			_addChildren( _node );
			invalidateBoundingBox();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::removeChildren( Node * _node )
	{
		this->_removeChildren( _node );

		TContainerChildren::iterator it_find = 
			std::find( m_children.begin(), m_children.end(), _node );

		if( it_find != m_children.end() )
		{
			(*it_find)->setParent( 0 );
			m_children.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Node::getChildren( const String& _name, bool _recursion ) const
	{
		for( TContainerChildren::const_iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			Node * children = *it;
			if( children->getName() == _name )
			{
				return children;
			}

			if( _recursion )
			{
				if( Node * result = children->getChildren( _name, _recursion ) )
				{
					return result;
				}
			}
		}
		
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isChildren( Node * _node, bool _recursive ) const
	{
		TContainerChildren::const_iterator it_find = 
			std::find( m_children.begin(), m_children.end(), _node );

		bool found = ( it_find != m_children.end() );
		if( !found && _recursive )
		{
			for( TContainerChildren::const_iterator it = m_children.begin(), it_end = m_children.end();
				it != it_end;
				it++ )
			{
				if( (*it)->isChildren( _node, _recursive ) == true )
				{
					return true;
				}
			}
		}
		return found;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_addChildren( Node * _node )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_removeChildren( Node * _node )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::registerEvent( EEventName _name, const String & _method )
	{
		bool result = Eventable::registerEvent( _name, _method, this );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setUpdatable( bool _updatable )
	{
		m_updatable = _updatable;

		//// !!!! ������, �����!! �����������, ��� ��� ����
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->setUpdatable( _updatable );
		}
		//// !!!!
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::update( float _timing )
	{
		if( this->isUpdatable() == false )
		{
			return;
		}

		if( m_updatable )	// !!!!
		{
			m_state = NODE_UPDATING;
			_update( _timing );
			
			if( m_state == NODE_DEACTIVATING )
			{
				this->deactivate();				
			}

			m_state = NODE_IDLE;
		}

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		/*++it*/)
		{
			(*it++)->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::loader( XmlElement * _xml )
	{
		Allocator2D::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Enable", "Value", m_enable );

			XML_CASE_NODE( "Node" )
			{
				String name;
				String type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
					XML_CASE_ATTRIBUTE( "Type", type );
				}

				Node * node = Holder<SceneManager>::hostage()
					->createNode( type );

				if(node == 0)
				{
					continue;
				}

				node->setName( name );
				addChildren( node );

				XML_PARSE_ELEMENT( node, &Node::loader );
			}

			XML_CASE_NODE( "Hide" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", m_hide);
				}
			}
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Color", "Value", &Node::setLocalColor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_activate()
	{
		m_debugMaterial = Holder<RenderEngine>::hostage()
							->createMaterial();

		//m_debugMaterial->textureStages = 1;
		m_debugMaterial->textureStage[0].colorOp = TOP_SELECTARG2;

		ApplyColor2D applyColor( 0xFF00FF00 );

		std::for_each( m_debugBox, m_debugBox + 4, applyColor );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_deactivate()
	{
		Holder<RenderEngine>::hostage()
			->releaseMaterial( m_debugMaterial );
		m_debugMaterial = 0;

		for( TAffectorVector::const_iterator
			it = m_affectorListToProcess.begin(),
			it_end = m_affectorListToProcess.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_affectorListToProcess.clear();

		for( TAffectorVector::const_iterator
			it = m_affectorsToAdd.begin(),
			it_end = m_affectorsToAdd.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_affectorsToAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_update( float _timing )
	{
		if( m_affectorsToAdd.empty() == false )
		{
			m_affectorListToProcess.insert( m_affectorListToProcess.end()
				, m_affectorsToAdd.begin()
				, m_affectorsToAdd.end()
				);
	
			m_affectorsToAdd.clear();
		}

 		for( TAffectorVector::iterator 
 			it = m_affectorListToProcess.begin();
 		it != m_affectorListToProcess.end();
 			/*++it*/ )
 		{
 			bool end = (*it)->affect( this, _timing );
 			if( end == true )
 			{
 				delete (*it);
 				it = m_affectorListToProcess.erase( it );
 			}
 			else
 			{
 				++it;
 			}
 		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::updatable() const
	{
		return m_updatable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		bool done = true;

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->compile();
		}

		//if( done == false )
		//{
		//	return false;
		//}

		bool result = Resource::compile();

		if( result == false )
		{
			/*MENGE_LOG( MENGE_TEXT("Error: compiled Node '%s' is failed\n")
				, getName().c_str() 
				);*/

			return false;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->release();
		}

		deactivate();

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::recompile()
	{
		//if( isCompile() == false )
		//{
		//	return true;
		//}

		bool status = isActivate();

		release();
		compile();

		if( isCompile() == false )
		{
			return false;
		}

		if( status )
		{
			activate();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isRenderable() const
	{
		if( isEnable() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( isHide() == true )
		{
			return false;
		}

		if( m_colorLocal.getA() < 0.001f )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::render( unsigned int _debugMask, Camera2D* _camera )
	{
		if( isRenderable() == false )
		{
			return;
		}

		const Viewport& viewPort = _camera->getViewport();
		if( checkVisibility( viewPort ) == false )
		{
			return;
		}

		_render( _debugMask );

		renderChild( _debugMask, _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::renderChild( unsigned int _debugMask, Camera2D* _camera )
	{
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->render( _debugMask, _camera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_render( unsigned int _debugMask )
	{
#	ifndef MENGE_MASTER_RELEASE
		if( _debugMask & MENGE_DEBUG_NODES )
		{
			const mt::box2f& bbox = getBoundingBox();
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			//mt::vec2f size = box_size( bbox );
			m_debugBox[0].pos[0] = bbox.minimum.x;
			m_debugBox[0].pos[1] = bbox.minimum.y;
			m_debugBox[1].pos[0] = bbox.maximum.x;
			m_debugBox[1].pos[1] = bbox.minimum.y;
			m_debugBox[2].pos[0] = bbox.maximum.x;
			m_debugBox[2].pos[1] = bbox.maximum.y;
			m_debugBox[3].pos[0] = bbox.minimum.x;
			m_debugBox[3].pos[1] = bbox.maximum.y;
			renderEngine->renderObject2D( m_debugMaterial, NULL, 1, m_debugBox, 4, LPT_RECTANGLE );
			//renderEngine->renderLine( 0xFF00FF00, bbox.minimum, bbox.minimum + mt::vec2f( size.x, 0.0f ) );
			//renderEngine->renderLine( 0xFF00FF00, bbox.minimum, bbox.minimum + mt::vec2f( 0.0f, size.y ) );
			//renderEngine->renderLine( 0xFF00FF00, bbox.maximum, bbox.maximum - mt::vec2f( size.x, 0.0f ) );
			//renderEngine->renderLine( 0xFF00FF00, bbox.maximum, bbox.maximum - mt::vec2f( 0.0f, size.y ) );
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::_checkVisibility( const Viewport & _viewport )
	{
		const mt::box2f & bbox = this->getBoundingBox();

		bool result = _viewport.testBBox( bbox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = Holder<ScriptEngine>::hostage()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateWorldMatrix()
	{
		Allocator2D::invalidateWorldMatrix();

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->invalidateWorldMatrix();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_invalidateWorldMatrix()
	{
		Allocator2D::_invalidateWorldMatrix();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Node::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return Allocator2D::getLocalMatrix();
		}

		if( isInvalidateWorldMatrix() == false )
		{
			return Allocator2D::getWorldMatrix();
		}

		const mt::mat3f & wm = m_parent->getWorldMatrix();

		return Allocator2D::updateWorldMatrix( wm );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Node::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setListener( PyObject * _listener )
	{
		m_listener = _listener;
		ScriptEngine::incref( m_listener );

		this->_setListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setListener()
	{
		Eventable::registerEvent( EVENT_MOVE_STOP, ("onMoveStop"), m_listener );
		Eventable::registerEvent( EVENT_MOVE_END, ("onMoveEnd"), m_listener );
		Eventable::registerEvent( EVENT_COLOR_END, ("onColorEnd"), m_listener );
		Eventable::registerEvent( EVENT_COLOR_STOP, ("onColorStop"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLayer( Layer2D * _layer )
	{
		m_layer = _layer;

		// ������ �������
		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			(*it)->setLayer( _layer );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Layer2D * Node::getLayer() const
	{
		return m_layer;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Node::getScreenPosition()
	{
		const mt::vec2f & pos = getWorldPosition();
		mt::vec2f screen_pos = pos;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		if( m_layer )
		{
			screen_pos = m_layer->calcScreenPosition( viewport, this );
		}

		return screen_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setAlpha( float _alpha ) 
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			(*it)->setAlpha( _alpha );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveToCb( float _time, const mt::vec2f& _point, PyObject* _cb )
	{
		moveToStop();

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateLinear<mt::vec2f, Node>(
			_cb, MENGE_AFFECTOR_POSITION, getLocalPosition(), _point, _time
			, &mt::length_v2, &Node::setLocalPosition );

		float invTime = 1.0f / _time;
		m_linearSpeed = ( _point - getLocalPosition() ) * invTime;

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::moveToStop()
	{
		stopAffectors_( MENGE_AFFECTOR_POSITION );

		m_linearSpeed = mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & Node::getBoundingBox()
	{
		if( m_invalidateBoundingBox == true )
		{
			_updateBoundingBox( m_boundingBox );

			for( TContainerChildren::iterator
				it = m_children.begin(),
				it_end = m_children.end();
			it != it_end;
			++it )
			{
				/*if( (*it)->isRenderable() == false )
				{
					continue;
				}*/
				const mt::box2f & childrenBoundingBox = (*it)->getBoundingBox();

				mt::merge_box( m_boundingBox, childrenBoundingBox );
			}

			m_invalidateBoundingBox = false;
		}

		return m_boundingBox;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateBoundingBox()
	{
		m_invalidateBoundingBox = true;

		if( m_parent )
		{
			m_parent->invalidateBoundingBox();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		const mt::vec2f & wp = this->getWorldPosition();

		mt::reset( _boundingBox, wp );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::getUpdatable()
	{
		if( ( isUpdatable() /*&& m_updatable*/ ) == false )
		{
			return false;
		}
		if( m_parent )
		{
			return m_parent->getUpdatable();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLocalColor( const ColourValue& _color )
	{
		m_colorLocal = _color;
		invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::invalidateColor()
	{
		m_invalidateColor = true;
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end()
			; it != it_end
			; it++ )
		{
			(*it)->invalidateColor();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Node::getWorldColor()
	{
		if( m_invalidateColor == true )
		{
			m_colorWorld = m_colorLocal;
			if( m_parent != NULL )
			{
				m_colorWorld = m_colorWorld * m_parent->getWorldColor();
			}
			m_invalidateColor = false;
		}
		return m_colorWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setLocalAlpha( float _alpha )
	{
		m_colorLocal.setA( _alpha );
		setLocalColor( m_colorLocal );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::angleToCb( float _time, float _angle, PyObject* _cb )
	{
		/*m_angleToCb = _cb;
		pybind::incref( m_angleToCb );
		m_angleTo.start( getAngle(), _angle, _time, ::fabsf );*/
		angleToStop();

		NodeAffector* affector =
			NodeAffectorCreator::newNodeAffectorInterpolateLinear<float, Node>
			( _cb, MENGE_AFFECTOR_ANGLE, getAngle(), _angle, _time, &fabsf, &Node::setRotate );
		m_affectorsToAdd.push_back( affector );

		float invTime = 1.0f / _time;
		m_angularSpeed = ( _angle - getAngle() ) * invTime;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::scaleToCb( float _time, const mt::vec2f& _scale, PyObject* _cb )
	{
		//m_scaleToCb = _cb;
		//pybind::incref( m_scaleToCb );
		//m_scaleTo.start( getScale(), _scale, _time, mt::length_v2 );
		scaleToStop();

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateLinear<mt::vec2f, Node>(
			_cb, MENGE_AFFECTOR_SCALE, getScale(), _scale, _time
			, &mt::length_v2, &Node::setScale
			);

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::accMoveToCb( float _time, const mt::vec2f& _point, PyObject* _cb )
	{
		mt::vec2f linearSpeed = m_linearSpeed;

		moveToStop();

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateQuadratic<mt::vec2f>(
			_cb, MENGE_AFFECTOR_POSITION, getLocalPosition(), _point, linearSpeed, _time
			, &mt::length_v2, &Node::setLocalPosition
			);

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::angleToStop()
	{
		stopAffectors_( MENGE_AFFECTOR_ANGLE );
		m_angularSpeed = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::scaleToStop()
	{
		stopAffectors_( MENGE_AFFECTOR_SCALE );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::accAngleToCb( float _time, float _angle, PyObject* _cb )
	{
		float angularSpeed = m_angularSpeed;
		angleToStop();

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateQuadratic<float>(
			_cb, MENGE_AFFECTOR_ANGLE, getAngle(), _angle, angularSpeed, _time
			, &fabsf, &Node::setRotate
			);

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorToCb( float _time, const ColourValue& _color, PyObject* _cb )
	{
		stopAffectors_( MENGE_AFFECTOR_COLOR );

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateLinear<ColourValue, Node>(
			_cb, MENGE_AFFECTOR_COLOR, getLocalColor(), _color, _time, 
			&length_color, &Node::setLocalColor );

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localAlphaToCb( float _time, float _alpha, PyObject* _cb )
	{
		ColourValue color = getLocalColor();
		color.setA( _alpha );
		localColorToCb( _time, color, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::localColorToStop()
	{
		stopAffectors_( MENGE_AFFECTOR_COLOR );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::addAffector( NodeAffector* _affector )
	{
		if( _affector == NULL )
		{
			return;
		}

		int affectorType = _affector->getType();
		stopAffectors_( affectorType );

		m_affectorsToAdd.push_back( _affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::stopAffectors_( int _type )
	{
		for( TAffectorVector::iterator it = m_affectorListToProcess.begin(), it_end = m_affectorListToProcess.end()
			; it != it_end
			; ++it )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();
			}
		}

		for( TAffectorVector::iterator it = m_affectorsToAdd.begin(), it_end = m_affectorsToAdd.end()
			; it != it_end
			; ++it )
		{
			if( (*it)->getType() == _type )
			{
				(*it)->stop();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject* Node::getListener()
	{
		ScriptEngine::incref( m_listener );
		return m_listener;
	}
	//////////////////////////////////////////////////////////////////////////

}
