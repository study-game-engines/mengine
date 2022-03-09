#pragma once

#include "Interface/FileInputStreamInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "SDLFileInputStream.h"

#include "Kernel/Factorable.h"

#include "SDL_rwops.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLMutexFileInputStream
        : public FileInputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLMutexFileInputStream );

    public:
        SDLMutexFileInputStream();
        ~SDLMutexFileInputStream() override;

    public:
        bool initialize( const SDLFileInputStreamPtr & _stream, const ThreadMutexInterfacePtr & _mutex );

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        bool close() override;

    public:
        size_t read( void * const _buf, size_t _size ) override;
        bool seek( size_t _pos ) override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _size ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t *  const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        bool read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read );
        bool seek_( size_t _pos );

#ifdef MENGINE_DEBUG
    protected:
        const FilePath & getRelationPath() const override;
        const FilePath & getFolderPath() const override;
        const FilePath & getFilePath() const override;
#endif

    protected:
        SDLFileInputStreamPtr m_stream;
        ThreadMutexInterfacePtr m_mutex;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;        

        uint8_t m_readCache[MENGINE_FILE_STREAM_BUFFER_SIZE];

#ifdef MENGINE_DEBUG
        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLMutexFileInputStream, FileInputStreamInterface> SDLMutexFileInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
