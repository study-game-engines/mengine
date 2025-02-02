#pragma once

#include "Interface/RenderTargetInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
        DECLARE_FACTORABLE( DX9RenderTargetTexture );

    public:
        DX9RenderTargetTexture();
        ~DX9RenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, EPixelFormat _format );
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;

        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        bool begin() const override;
        void end() const override;

    public:
        const mt::uv4f & getUV() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        IDirect3DDevice9 * getDirect3dDevice9() const;
        IDirect3DTexture9 * getDirect3dTexture9() const;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_width;
        uint32_t m_height;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        EPixelFormat m_hwPixelFormat;

        float m_hwWidthInv;
        float m_hwHeightInv;

        mt::uv4f m_uv;
        
        IDirect3DTexture9 * m_pD3DTexture;
        
        mutable IDirect3DSurface9 * m_pD3DSurfaceCurrent;
        mutable IDirect3DSurface9 * m_pD3DSurfaceOld;

        mutable D3DVIEWPORT9 m_VPOld;

        bool m_upscalePow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderTargetTexture, RenderTargetInterface> DX9RenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}