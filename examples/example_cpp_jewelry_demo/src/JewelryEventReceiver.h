#pragma once

#include "Interface/RandomizerInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Engine/TextField.h"

#include "Jewelry.h"
#include "JewelryMatrix.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JewelryEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        JewelryEventReceiver();
        ~JewelryEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void spawnJewelry_( const GOAP::SourcePtr & _source, EJewelrySuper _super, uint32_t _iterator );
        void collapseJewelry_( const GOAP::SourcePtr & _source, const JewelryPtr & _jewelry );
        void explosiveJewelry_( const GOAP::SourcePtr & _source, const JewelryPtr & _jewelry );

    protected:
        NodePtr spawnExplosive_();

    protected:
        JewelryPtr makeJewelry_( EJewelrySuper _super, uint32_t _type, uint32_t _column, uint32_t _row, const Char * _doc );

    protected:
        void makeUI_();
        void makeUITextStage_();
        void makeUITextTime_();

    protected:
        Scene * m_scene;

        NodePtr m_base;

        GOAP::ChainPtr m_chain;

        uint32_t m_jewelry_type_count;
        float m_jewelry_size;
        float m_jewelry_stride;

        float m_jewelry_cell_fall_time_ms;
        float m_jewelry_cell_explosive_time_ms;
        uint32_t m_jewelry_cell_explosive_count;
        float m_jewelry_spawn_time_ms;
        bool m_jewelry_collapse;

        uint32_t m_stage;
        uint64_t m_timemillisecond;

        TextFieldPtr m_textStage;
        TextFieldPtr m_textTime;

        FactoryPtr m_factoryJewelry;

        typedef Vector<JewelryPtr> VectorJewelryHand;
        VectorJewelryHand m_jewelryHand;

        GOAP::EventPtr m_eventFall;

        JewelryMatrixPtr m_jewelryMatrix;

        RandomizerInterfacePtr m_randomizer;

        JSONStorageInterfacePtr m_storageLevels;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryEventReceiver> JewelryEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}