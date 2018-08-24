#pragma once

#include "Kernel/Surface.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/ResourceHolder.h"

#include "ResourceVideo.h"

#include "Interface/VideoCodecInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum SurfaceVideoEventFlag
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceVideoEventReceiver
        : public AnimationEventReceiver
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class SurfaceVideo
        : public Surface
        , public BaseEventation
        , public BaseAnimation
    {
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE( SurfaceVideoEventReceiver );

    public:
        SurfaceVideo();
        ~SurfaceVideo() override;

    public:
        void setResourceVideo( const ResourceVideoPtr & _resourceVideo );
        const ResourceVideoPtr & getResourceVideo() const;

    public:
        float getWidth() const;
        float getHeight() const;
        float getDuration() const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) override;

        const ColourValue & getColor() const override;

    protected:
        void _activate() override;
        bool _update( const UpdateContext * _context ) override;

    protected:
        void _setTime( float _time ) override;
        float _getTime() const override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void updateVideoBuffer_();

        bool sync_( float _time );
        bool createDecoder_();
        bool fillVideoBuffer_();
        void updateUV_();
        void updateSize_();

    protected:
        ResourceHolder<ResourceVideo> m_resourceVideo;

        RenderTextureInterfacePtr m_textures[1];

        VideoDecoderInterfacePtr m_videoDecoder;

        float m_time;

        bool m_needUpdateVideoBuffer;
        bool m_updateFirstFrame;

        mt::vec2f m_size;
        mt::uv4f m_uv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceVideo> SurfaceVideoPtr;
}
