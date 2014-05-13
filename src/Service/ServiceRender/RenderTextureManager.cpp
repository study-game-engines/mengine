#   include "RenderTextureManager.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/PrefetcherInterface.h"

#	include "stdex/memorycopy.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureManager, Menge::RenderTextureServiceInterface, Menge::RenderTextureManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	namespace
	{
		class RenderTextureFactoryListener
			: public FactoryListenerInterface
		{
		public:
			RenderTextureFactoryListener( RenderTextureManager * _textureManager )
				: m_textureManager(_textureManager)
			{
			}

		protected:
			void onFactoryDestroyObject( Factorable * _object ) override
			{
				RenderTexture * texture = static_cast<RenderTexture *>(_object);
				m_textureManager->onRenderTextureRelease_( texture );
			}

		protected:
			void destroy() override
			{
				delete this;
			}

		protected:
			RenderTextureManager * m_textureManager;
		};
	}
    //////////////////////////////////////////////////////////////////////////
    static uint32_t s_firstPOW2From( uint32_t n )
    {
        --n;            
        n |= n >> 16;
        n |= n >> 8;
        n |= n >> 4;
        n |= n >> 2;
        n |= n >> 1;
        ++n;
        return n;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::RenderTextureManager()
        : m_serviceProvider(nullptr)
        , m_textureEnumerator(0)
		, m_supportA8(false)
		, m_supportR8G8B8(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::~RenderTextureManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * RenderTextureManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::initialize()
    {
        m_debugInfo.textureMemory = 0;
        m_debugInfo.textureCount = 0;

		m_supportA8 = RENDER_SYSTEM(m_serviceProvider)
			->supportTextureFormat( PF_A8 );

		m_supportR8G8B8 = RENDER_SYSTEM(m_serviceProvider)
			->supportTextureFormat( PF_R8G8B8 );

		m_factoryRenderTexture.setListener( new RenderTextureFactoryListener(this) );
	
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::finalize()
    {        
        m_textures.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::hasTexture( const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const
    {
		RenderTextureInterface * texture;
		bool result = m_textures.has( _fileName, &texture );

		if( _texture != nullptr )
		{
			*_texture = texture;
		}
		
		return result;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::getTexture( const FilePath& _fileName ) const
    {
        TMapTextures::const_iterator it_find = m_textures.find( _fileName );

        if( it_find == m_textures.end() )
        {
            return nullptr;
        }

		RenderTextureInterface * texture = m_textures.get_value( it_find );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::createTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
    {
		size_t HWWidth = _width;
		size_t HWHeight = _height;
		size_t HWChannels = _channels;
		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWFormat );

		RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
			->createImage( HWWidth, HWHeight, HWChannels, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createTexture_ couldn't create image %dx%d"
                , HWWidth
                , HWHeight
                );

            return nullptr;
        }

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _width, _height, _channels, id );

        size_t memroy_size = texture->getMemoryUse();

        m_debugInfo.textureMemory += memroy_size;
        ++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createTexture creating texture %dx%d %d memory %d:%d"
            , HWWidth
            , HWHeight
            , HWFormat
            , memroy_size
            , m_debugInfo.textureMemory
            );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
    {
		size_t HWWidth = _width;
		size_t HWHeight = _height;
		size_t HWChannels = _channels;
		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWFormat );

        RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createDynamicImage( HWWidth, HWHeight, HWChannels, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createDynamicTexture couldn't create image %dx%d channels %d"
                , HWWidth
                , HWHeight
                , HWChannels
                );

            return nullptr;
        }

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _width, _height, _channels, id );

        size_t memroy_size = texture->getMemoryUse();

        m_debugInfo.textureMemory += memroy_size;
        ++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createDynamicTexture creating dynamic texture %dx%d channels %d use memory %d:%d"
            , HWWidth
            , HWHeight
            , HWChannels 
            , memroy_size
            , m_debugInfo.textureMemory
            );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
    {
		size_t HWWidth = _width;
		size_t HWHeight = _height;
		size_t HWChannels = _channels;
		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWFormat );

        RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createRenderTargetImage( HWWidth, HWHeight, HWChannels, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createRenderTargetTexture couldn't create RenderTargetTexture %dx%d %d"
                , HWWidth
                , HWHeight 
                , HWChannels
                );

            return nullptr;
        }

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _width, _height, _channels, id );

        size_t memroy_size = texture->getMemoryUse();

        m_debugInfo.textureMemory += memroy_size;
        ++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createRenderTargetTexture creating render target texture %dx%d %d memory %d:%d"
            , HWWidth
            , HWHeight 
            , HWChannels 
            , memroy_size
            , m_debugInfo.textureMemory
            );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::saveImage( const RenderTextureInterfacePtr & _texture, const ConstString & _fileGroupName, const ConstString & _codecName, const FilePath & _fileName )
    {
        OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( _fileGroupName, _fileName );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::saveImage : can't create file '%s' '%s'"
                , _fileGroupName.c_str()
                , _fileName.c_str() 
                );

            return false;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(m_serviceProvider)
            ->createEncoderT<ImageEncoderInterfacePtr>( _codecName );

        if( imageEncoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::saveImage : can't create encoder for filename '%s'"
                , _fileName.c_str() 
                );

            return false;
        }

		if( imageEncoder->initialize( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::saveImage : can't initialize encoder for filename '%s'"
				, _fileName.c_str() 
				);

			return false;
		}

        ImageCodecDataInfo dataInfo;
        //dataInfo.format = _image->getHWPixelFormat();
        dataInfo.width = _texture->getWidth();
        dataInfo.height = _texture->getHeight();		
        dataInfo.depth = 1;
        dataInfo.mipmaps = 0;
        dataInfo.flags = 0;
        dataInfo.size = 0;	// we don't need this
        int pitch = 0;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = dataInfo.width;
        rect.bottom = dataInfo.height;

        unsigned char * buffer = _texture->lock( &pitch, rect, true );

		if( buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::saveImage : can't lock texture '%s'"
				, _fileName.c_str() 
				);

			return false;
		}

        ImageCodecOptions options;		

        options.pitch = pitch;
        options.channels = 4;
        //options.flags |= DF_CUSTOM_PITCH;

        imageEncoder->setOptions( &options );

        unsigned int bytesWritten = imageEncoder->encode( buffer, &dataInfo );

        _texture->unlock();

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::saveImage : Error while encoding image data"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::visitTexture( VisitorRenderTextureInterface * _visitor ) const
    {
        for( TMapTextures::const_iterator
            it = m_textures.begin(),
            it_end = m_textures.end();
        it != it_end;
        ++it )
        {
			const RenderTextureInterface * texture = m_textures.get_value( it );

            _visitor->visitRenderTexture( texture );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::cacheFileTexture( const FilePath& _fileName, const RenderTextureInterfacePtr & _texture )
    {
        _texture->setFileName( _fileName );

        RenderTextureInterface * texture_ptr = _texture.get();

        m_textures.insert( _fileName, texture_ptr );

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::cacheFileTexture cache texture %s"
            , _fileName.c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::loadTexture( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
    {
        TMapTextures::iterator it_found = m_textures.find( _fileName );

        if( it_found != m_textures.end() )
        {
			RenderTextureInterface * cache_texture = m_textures.get_value( it_found );

            return cache_texture;
        }
			
		ImageDecoderInterfacePtr imageDecoder;
		if( PREFETCHER_SERVICE(m_serviceProvider)
			->getImageDecoder( _fileName, imageDecoder ) == false )
		{
			imageDecoder = this->createImageDecoder_( _pakName, _fileName, _codec );

			if( imageDecoder == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::loadTexture invalid create decoder '%s':'%s'"
					, _pakName.c_str()
					, _fileName.c_str()
					);

				return nullptr;
			}
		}

		RenderTextureInterfacePtr texture = this->createTextureFromDecoder_( imageDecoder );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::loadTexture invalid create texure '%s':'%s'"
				, _pakName.c_str()
				, _fileName.c_str()				
				);

			return nullptr;
		}

        this->cacheFileTexture( _fileName, texture );

        return texture;
    }
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderInterfacePtr RenderTextureManager::createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec )
	{
		InputStreamInterfacePtr stream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( _pakName, _fileName );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createImageDecoder_: Image file '%s' was not found"
				, _fileName.c_str() 
				);

			return nullptr;
		}	

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( _codec );

		if( imageDecoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createImageDecoder_: Image decoder for file '%s' was not found"
				, _fileName.c_str() 
				);

			return nullptr;
		}

		if( imageDecoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createImageDecoder_: Image decoder for file '%s' was not initialize"
				, _fileName.c_str() 
				);

			return nullptr;
		}

		return imageDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderTextureManager::createTextureFromDecoder_( const ImageDecoderInterfacePtr & _decoder )
	{
		const ImageCodecDataInfo* dataInfo = _decoder->getCodecDataInfo();

		size_t image_width = dataInfo->width;
		size_t image_height = dataInfo->height;
		size_t image_channels = dataInfo->channels;

		RenderTextureInterfacePtr texture = this->createTexture( image_width, image_height, image_channels, dataInfo->format );
		
		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createTextureFromDecoder_: invalid create texture %d:%d channels %d format %d"
				, image_width
				, image_height
				, image_channels
				, dataInfo->format
				);

			return nullptr;
		}

		const Rect & rect = texture->getHWRect();

		if( this->loadTextureRectImageData( texture, rect, _decoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createTextureFromDecoder_: invalid decode texture"
				);

			return nullptr;
		}

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////	
	void RenderTextureManager::updateImageParams_( size_t & _width, size_t & _height, size_t & _channels, PixelFormat & _format ) const
	{
		if( ( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0 )
		{
			_width = s_firstPOW2From( _width );
			_height = s_firstPOW2From( _height );
		}

		switch( _format )
		{
		case PF_UNKNOWN:
			{
				if( _channels == 1 )
				{
					if( m_supportA8 == true )
					{
						_format = PF_A8;
					}
					else
					{
						_format = PF_X8R8G8B8;
						_channels = 4;
					}
				}
				else if( _channels == 3 )
				{
					if( m_supportR8G8B8 == true )
					{
						_format = PF_R8G8B8;
					}
					else
					{
						_format = PF_X8R8G8B8;
						_channels = 4;
					}
				}
				else if( _channels == 4 )
				{
					_format = PF_A8R8G8B8;
				}
			}break;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::onRenderTextureRelease_( const RenderTextureInterface * _texture )
    {
        const FilePath & filename = _texture->getFileName();

		m_textures.erase( filename );

        size_t memroy_size = _texture->getMemoryUse();

		const RenderImageInterfacePtr & image = _texture->getImage(); 

        size_t HWWidth = image->getHWWidth();
        size_t HWHeight = image->getHWHeight();
        PixelFormat HWPixelFormat = image->getHWPixelFormat();

        m_debugInfo.textureMemory -= memroy_size;
        --m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::destroyTexture_ destroy %s texture %dx%d %d memory %d:%d"
			, filename.c_str()
            , HWWidth
            , HWHeight
            , HWPixelFormat
            , memroy_size
            , m_debugInfo.textureMemory
            );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder )
    {
        int pitch = 0;
        unsigned char * textureBuffer = _texture->lock( &pitch, _rect, false );

        if( textureBuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::loadTextureImageData Invalid lock");

            return false;
        }

        ImageCodecOptions options;

		const RenderImageInterfacePtr & image = _texture->getImage();

        options.channels = image->getHWChannels();
        options.pitch = pitch;

        _imageDecoder->setOptions( &options );

        const ImageCodecDataInfo * data = _imageDecoder->getCodecDataInfo();

        BEGIN_WATCHDOG(m_serviceProvider, "texture decode");

        unsigned int bufferSize = pitch * data->height;
        if( _imageDecoder->decode( textureBuffer, bufferSize ) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::loadTextureImageData Invalid decode");

            _texture->unlock();

            return false;
        }

        END_WATCHDOG(m_serviceProvider, "texture decode", 1)("texture decode %s"
            , _texture->getFileName().c_str()			
            );

        //this->sweezleAlpha( _texture, textureBuffer, pitch );
        this->imageQuality( _texture, textureBuffer, pitch );

        BEGIN_WATCHDOG(m_serviceProvider, "texture unlock");

        _texture->unlock();

        END_WATCHDOG(m_serviceProvider, "texture unlock", 1)("texture unlock %s"
            , _texture->getFileName().c_str()
            );
		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch )
    {
        size_t width = _texture->getWidth();
        size_t height = _texture->getHeight();


		const RenderImageInterfacePtr & image = _texture->getImage(); 

        size_t hwWidth = image->getHWWidth();
        size_t hwHeight = image->getHWHeight();
		
        // copy pixels on the edge for better image quality
        if( hwWidth > width )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ width %d - %d"
                , width
                , hwWidth
                );

            unsigned char * image_data = _textureBuffer;
            unsigned int pixel_size = _texturePitch / hwWidth;

            for( size_t j = 0; j != height; ++j )
            {
     //           std::copy( image_data + (width - 1) * pixel_size
					//, image_data + width * pixel_size
					//, image_data + width * pixel_size );

				stdex::memorycopy( image_data + width * pixel_size, image_data + (width - 1) * pixel_size, pixel_size );

                image_data += _texturePitch;
            }
        }

        if( hwHeight > height )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ height %d - %d"
                , height
                , hwHeight
                );

            unsigned char * image_data = _textureBuffer;

    //        std::copy( image_data + (height - 1) * _texturePitch
				//, image_data + height * _texturePitch
				//, image_data + height * _texturePitch );

			stdex::memorycopy( image_data + height * _texturePitch, image_data + (height - 1) * _texturePitch, _texturePitch );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureDebugInfo & RenderTextureManager::getDebugInfo()
    {
        return m_debugInfo;
    }
}