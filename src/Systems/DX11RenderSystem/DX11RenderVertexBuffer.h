#pragma once

#include "Interface/RenderVertexBufferInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderVertexBuffer
        : public RenderVertexBufferInterface
        , public Factorable
        , public DX11RenderResourceHandler
    {
    public:
        DX11RenderVertexBuffer();
        ~DX11RenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _vertexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    protected:
        void enable() override;
        void disable() override;

    protected:
        bool resize( uint32_t _vertexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_vertexSize;
        uint32_t m_vertexCapacity;
        uint32_t m_vertexCount;

        DWORD m_usage;
        D3DFORMAT m_format;
        D3DPOOL m_pool;
        IDirect3DVertexBuffer9 * m_pD3DVertexBuffer;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderVertexBuffer, RenderVertexBufferInterface> DX11RenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}