#pragma once

#include "Interface/RenderVertexBufferInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderBuffer
        : public DX11RenderResourceHandler
    {
    public:
        DX11RenderBuffer();
        ~DX11RenderBuffer() override;

    public:
        bool initializeBuffer( uint32_t _elementSize, EBufferType _bufferType );
        void finalizeBuffer();

    protected:
        uint32_t getElementsCount() const;
        uint32_t getElementSize() const;

    protected:
        bool resizeBuffer( D3D11_BIND_FLAG _bindFlag, uint32_t _elementsCount, const void * _initData );

    protected:
        // only for write
        MemoryInterfacePtr lockBuffer( uint32_t _offset, uint32_t _size );
        bool unlockBuffer();

    protected:
        bool drawBuffer( const void * _buffer, uint32_t _offset, uint32_t _count );

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_elementSize;
        uint32_t m_elementsCount;
        uint32_t m_elementsCapacity;

        DXGI_FORMAT m_format;
        ID3D11BufferPtr m_pD3DBuffer;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    //typedef IntrusivePtr<DX11RenderVertexBuffer, RenderVertexBufferInterface> DX11RenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}