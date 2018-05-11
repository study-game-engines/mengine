#include "EntityPrototypeGenerator.h"

#include "Interface/StringizeInterface.h"

#include "Kernel/ScriptEventReceiver.h"

#include "pybind/pybind.hpp"

#include "Logger/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class PythonEntityEventReceiver
            : public ScriptEventReceiver
            , public EntityEventReceiver
        {
        public:
            void onEntityPreparation( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityActivate( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityPreparationDeactivate( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityDeactivate( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityCompile( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityRelease( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }

            void onEntityCreate( const pybind::object & _self, Node * _node ) override
            {
                m_cb.call( _self, _node );
            }

            void onEntityDestroy( const pybind::object & _self ) override
            {
                m_cb.call( _self );
            }
        };
    }
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::EntityPrototypeGenerator()
		: m_count(0)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::~EntityPrototypeGenerator()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityPrototypeGenerator::setScriptGenerator( const pybind::object & _generator )
	{
		m_generator = _generator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		m_category = _category;
		m_prototype = _prototype;

		if( m_generator.is_callable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object EntityPrototypeGenerator::preparePythonType()
	{
		if( m_type.is_invalid() == false )
		{
			return m_type;
		}
				
		pybind::object py_type = m_generator.call( m_prototype );

		if( py_type.is_invalid() == true || py_type.is_none() == true )
		{
			LOGGER_ERROR("PythonPrototypeGenerator type %s prototype %s invalid type create"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return pybind::make_invalid_object_t();
		}

		if( py_type.is_type_class() == true )
		{
			PyObject * py_type_ptr = py_type.ptr();

			if( pybind::type_initialize( py_type_ptr ) == false )
			{
				LOGGER_ERROR("PythonPrototypeGenerator prototype %s invalid type initialize"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return pybind::make_invalid_object_t();
			}
		}

		Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onCreate", EVENT_ENTITY_CREATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onDestroy", EVENT_ENTITY_DESTROY );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onPreparation", EVENT_ENTITY_PREPARATION );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onActivate", EVENT_ENTITY_ACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onPreparationDeactivate", EVENT_ENTITY_PREPARATION_DEACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onDeactivate", EVENT_ENTITY_DEACTIVATE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onCompile", EVENT_ENTITY_COMPILE );
        Helper::registerScriptEventReceiverMethod<PythonEntityEventReceiver>( py_type, this, "onRelease", EVENT_ENTITY_RELEASE );

		//this->registerEventMethod( EVENT_ENTITY_DESTROY, "onDestroy", py_type );

		//this->registerEventMethod( EVENT_ENTITY_PREPARATION, "onPreparation", py_type );
		//this->registerEventMethod( EVENT_ENTITY_ACTIVATE, "onActivate", py_type );
		//this->registerEventMethod( EVENT_ENTITY_PREPARATION_DEACTIVATE, "onPreparationDeactivate", py_type );
		//this->registerEventMethod( EVENT_ENTITY_DEACTIVATE, "onDeactivate", py_type );
		//this->registerEventMethod( EVENT_ENTITY_COMPILE, "onCompile", py_type );
		//this->registerEventMethod( EVENT_ENTITY_RELEASE, "onRelease", py_type );

		m_type = py_type;

		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	PointerFactorable EntityPrototypeGenerator::generate()
	{
		const pybind::object & py_type = this->preparePythonType();

		if( py_type.is_invalid() == true )
		{
			return nullptr;
		}

		pybind::object py_entity = py_type.call();

		if( py_entity.is_invalid() == true )
		{
			LOGGER_ERROR("EntityPrototypeGenerator.generate: can't create object '%s' '%s' (invalid create)"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		EntityPtr entity = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), m_category );

		if( entity == nullptr )
		{
			LOGGER_ERROR("EntityPrototypeGenerator can't generate '%s' '%s'"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		entity->setPrototype( m_prototype );

		entity->setScriptEventable( this );
		entity->setScriptObject( py_entity );

		entity->onCreate();

		++m_count;

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t EntityPrototypeGenerator::count() const
	{
		return m_count;
	}
}