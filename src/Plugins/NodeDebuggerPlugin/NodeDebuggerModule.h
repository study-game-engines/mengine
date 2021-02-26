#pragma once

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/DebuggerBoundingBoxInterface.h"

#include "Plugins/MoviePlugin/Movie2Interface.h"
#include "Plugins/SpinePlugin/SpineInterface.h"

#include "Engine/TextField.h"

#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/Deque.h"
#include "Kernel/ModuleBase.h"
#include "Kernel/NodeUniqueFinder.h"
#include "Kernel/Shape.h"
#include "Kernel/Arrow.h"
#include "Kernel/Scene.h"
#include "Kernel/ResourceImageSequence.h"

#include "NodeDebuggerSerialization.h"
#include "cURLRequestListener.h"
#include "SceneDataProviderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ENodeDebuggerServerState : uint32_t
    {
        Invalid,
        WaitingForClient,
        Connected
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ENodeDebuggerPacketType : uint32_t
    {
        // asking packets
        Ask_Scene,

        // answering packets
        Answer_Scene
    };
    //////////////////////////////////////////////////////////////////////////
    struct NodeDebuggerPacket
    {
        Blobject payload;
    };
    //////////////////////////////////////////////////////////////////////////
    class NodeDebuggerModule
        : public ModuleBase
        , public ThreadWorkerInterface
        , public SceneDataProviderInterface
    {
    public:
        NodeDebuggerModule();
        ~NodeDebuggerModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _availableModule() const override;

    public:
        void onThreadWorkerUpdate( uint32_t _id ) override;
        bool onThreadWorkerWork( uint32_t _id ) override;
        void onThreadWorkerDone( uint32_t _id ) override;

    public:
        void setArrow( const ArrowPtr & _arrow );
        void updateArrow();

    public:
        void setScene( const ScenePtr & _scene );
        void updateScene();

    public:
        void _update( bool _focus ) override;
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        bool privateInit();
        void recreateServer();
        void compressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr );
        void uncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData );
        void sendPacket( NodeDebuggerPacket & _packet );
        void sendArrow( const ArrowPtr & _arrow );
        void sendScene( const ScenePtr & _scene );
        void sendPickerable( const ScenePtr & _scene );
        void sendRenderable( const ScenePtr & _scene );
        void sendSettings();
        void sendMemory();
        void sendObjectsLeak();
        void sendNetwork();
        void sendSelectedNode();

    protected:
        void serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode );
        void serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode );
        void serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeTransformation( const TransformablePtr & _transformable, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const Compilable * _compilable, const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode );
        void serializeMovie2( const Compilable * _compilable, const UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode );
        void serializeSpine( const UnknownSpineInterface * _unknownSpine, pugi::xml_node & _xmlParentNode );
        void serializeShape( const ShapePtr & _shape, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImage( const SurfaceImagePtr & _surfaceImage, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImageSequence( const SurfaceImageSequencePtr & _surfaceImageSequence, pugi::xml_node & _xmlParentNode );
        void serializeContent( const ContentInterfacePtr & _content, pugi::xml_node & _xmlParentNode );

    protected:
        void processPacket( NodeDebuggerPacket & _packet );
        void receiveChangedNode( const pugi::xml_node & _xmlNode );
        void receiveGameControlCommand( const String & _command );
        void stringToPath( const String & _str, VectorNodePath * const _path ) const;
        void pathToString( const VectorNodePath & _path, String * const _outStr ) const;

    protected:
        void notifyChangeArrow( const ArrowPtr & _arrow );
        void notifyChangeSceneComplete( const ScenePtr & _scene );
        void notifyChangeSceneDestroy( const ScenePtr & _scene );
        void notifyRemoveSceneDestroy();
        void notifyIncrefFactoryGeneration( uint32_t _generator );

    public:
        void setUpdateSceneFlag( bool _flag ) override;

    protected:
        void findChildRecursive( const NodePtr & _currentNode );
        bool checkOnInfinityAndIntersectForSelectedNode( const mt::box2f & _boundingBox );
        bool cheHitWithAlpha( const NodePtr & _currentNode );

    protected:
        uint32_t m_globalKeyHandlerF2;
        uint32_t m_globalKeyHandlerForSendingSelectedNode;

        ArrowPtr m_arrow;
        ScenePtr m_scene;
        NodePtr m_selectedNode;

        SocketInterfacePtr m_socket;
        AtomicBool m_shouldRecreateServer;
        AtomicBool m_shouldUpdateScene;
        ThreadJobPtr m_threadJob;
        uint32_t m_workerId;
        ThreadMutexInterfacePtr m_dataMutex;
        Atomic<ENodeDebuggerServerState> m_serverState;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Blobject m_receivedData;
        Blobject m_receivedDataAux;
        VectorNodePath m_selectedNodePath;
        ArchivatorInterfacePtr m_archivator;
        String m_currentTab;

        cURLRequestListenerPtr m_networkLogger;
        UniqueId m_requestListenerId;

        mt::vec2f m_cursorWorldPosition;
    };
}