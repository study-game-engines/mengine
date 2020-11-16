#include "ImGUIPlugin.h"

#include "ImGUIModule.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ImGUI, Mengine::ImGUIPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIPlugin::ImGUIPlugin()
        : m_handlerId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIPlugin::~ImGUIPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleImGUI" )
            , Helper::makeModuleFactory<ImGUIModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_finalizePlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "imgui" );
    }
}
