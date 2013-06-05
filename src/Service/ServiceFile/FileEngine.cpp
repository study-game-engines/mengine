#	include "FileEngine.h"

#	include "Logger/Logger.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"

#	include "FileGroupDirectory.h"
#	include "FileGroupZip.h"
//#	include "FileInput.h"

#	include "Core/String.h"
#	include "Core/FilePath.h"

#	include <vector>
#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileService, Menge::FileServiceInterface, Menge::FileEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine()
		: m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileEngine::~FileEngine()
	{
		for( TMapFileSystem::iterator 
			it = m_fileSystemMap.begin(), 
			it_end = m_fileSystemMap.end();
		it != it_end;
		++it )
		{
			it->second->destroy();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void FileEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        FactoryManager::setServiceProvider( m_serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * FileEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initialize()
	{
		FactoryManager::registerFactory( Helper::stringizeString(m_serviceProvider, "dir"), new FactoryDefault<FileGroupDirectory>() );
        FactoryManager::registerFactory( Helper::stringizeString(m_serviceProvider, "dir"), new FactoryDefault<FileGroupDirectory>() );
		FactoryManager::registerFactory( Helper::stringizeString(m_serviceProvider, "zip"), new FactoryDefault<FileGroupZip>() );
       
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileGroup( const ConstString& _fileSystemName, const FilePath& _folder, const FilePath& _path, const ConstString & _type, bool _create )
	{
        LOGGER_INFO(m_serviceProvider)( "FileEngine:mountFileSystem _fileSystemName '%s' _path '%s':'%s' _type '%s'"
                       , _fileSystemName.c_str() 
                       , _folder.c_str()
                       , _path.c_str()
                       , _type.c_str()
                       );
        
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find != m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::mountFileSystem FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		FileGroupInterface * fs = FactoryManager::createObjectT<FileGroupInterface>( _type );

		if( fs == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::mountFileSystem can't create fileGroup '%s' type '%s' for object '%s':'%s'"
                , _fileSystemName.c_str()
                , _type.c_str()
                , _folder.c_str()
				, _path.c_str() 
				);

			return false;

			// try mount as Directory
			//fs = FactoryManager::createObjectT<FileSystem>( Consts::get()->c_builtin_empty );
		}

		if( fs->initialize( m_serviceProvider, _folder, _path, _type, _create ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::mountFileSystem can't initialize FileSystem for object '%s'"
				, _path.c_str() 
				);

			fs->destroy();

			return false;
		}

		m_fileSystemMap.insert( std::make_pair( _fileSystemName, fs ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unmountFileGroup( const ConstString& _fileSystemName )
	{
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Error: (FileEngine::unmountFileSystem) FileSystem with name '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return;
		}

        FileGroupInterface * groupInterface = it_find->second;

        groupInterface->finalize();
		groupInterface->destroy();

		m_fileSystemMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const ConstString& _fileSystemName, const FilePath & _filename, FileGroupInterface ** _group ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );
		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}
		
		FileGroupInterface * fileGroup = it_find->second;

		if( fileGroup->existFile( _filename ) == false )
        {
            return false;
        }

        if( _group != nullptr )
        {
            *_group = fileGroup;
        }

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool FileEngine::hasFileGroup( const ConstString& _fileSystemName ) const
    {
        TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );

        if( it_find == m_fileSystemMap.end() )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	FileGroupInterface * FileEngine::getFileGroup( const ConstString& _fileSystemName ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );

		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Error: (FileEngine::createInputFile) FileSystem '%s' not mount"
				, _fileSystemName.c_str()
				);

			return NULL;
		}

		FileGroupInterface * fileSystem = it_find->second;

		return fileSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileEngine::openInputFile( const ConstString& _fileSystemName, const FilePath & _filename )
	{
		FileGroupInterface * group = this->getFileGroup( _fileSystemName );

		if( group == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't get group '%s'"
                , _fileSystemName.c_str()
                );

			return nullptr;
		}

		InputStreamInterfacePtr file = group->createInputFile();

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't create input file '%s'"
				, _fileSystemName.c_str()
				);

			return nullptr;
		}

		if( group->openInputFile( _filename, file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't open input file '%s' '%s'"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);

			return nullptr;
		}		

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr FileEngine::openOutputFile( const ConstString & _fileSystemName, const FilePath & _filename )
	{
        FileGroupInterface * group = this->getFileGroup( _fileSystemName );

        if( group == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't get group '%s'"
                , _fileSystemName.c_str()
                );

            return nullptr;
        }

        OutputStreamInterfacePtr file = group->createOutputFile();

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't create output file '%s'"
				, _fileSystemName.c_str()
				);

			return nullptr;
		}

		if( group->openOutputFile( _filename, file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't open output file '%s' '%s'"
				, _fileSystemName.c_str()
				, _filename.c_str()
				);
			
			return nullptr;
		}

		return file;
	}
    //////////////////////////////////////////////////////////////////////////
    MappedFileInputStreamInterfacePtr FileEngine::openMappedInputStream( const FilePath & _foldername, const FilePath& _filename )
    {
        MappedFileInputStreamInterfacePtr mappedFile = FILE_SYSTEM(m_serviceProvider)
            ->createMappedInputStream();

        if( mappedFile == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedInputStream can't create output file '%s'"
                , _filename.c_str()
                );

            return nullptr;
        }

        if( mappedFile->open( _foldername, _filename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedInputStream can't open output file '%s'"
                , _filename.c_str()
                );

            return nullptr;
        }

        return mappedFile;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileEngine::existDirectory( const ConstString& _fileSystemName, const FilePath& _path )
    {
        TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileSystemName );
        if( it_find == m_fileSystemMap.end() )
        {
            return false;
        }

        FileGroupInterface * fileGroup = it_find->second;

        if( fileGroup->existDirectory( _path ) == false )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const ConstString& _fileSystemName, const FilePath & _path )
	{
        FileGroupInterface * group = this->getFileGroup( _fileSystemName );

        if( group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::createDirectory '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		if( group->createDirectory( _path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::removeDirectory( const ConstString& _fileSystemName, const FilePath & _path )
	{
        FileGroupInterface * group = this->getFileGroup( _fileSystemName );

		if( group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::removeDirectory '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return false;
		}

		if( group->removeDirectory( _path ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::removeFile( const ConstString& _fileSystemName, const FilePath & _filename )
	{
        FileGroupInterface * group = this->getFileGroup( _fileSystemName );

        if( group == NULL )
        {
			LOGGER_ERROR(m_serviceProvider)( "FileEngine::removeFile '%s' not mount"
				, _fileSystemName.c_str() 
				);

			return false;
		}
		
		if( group->removeFile( _filename ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
