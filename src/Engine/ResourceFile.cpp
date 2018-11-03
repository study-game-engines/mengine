#include "ResourceFile.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceFile::ResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFile::~ResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceFile::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceFile::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
}
