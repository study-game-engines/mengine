#include "AppleSKAdNetworkService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleSKAdNetworkService, Mengine::AppleSKAdNetworkService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkService::AppleSKAdNetworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkService::~AppleSKAdNetworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSKAdNetworkService::_initializeService()
    {

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleSKAdNetworkService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
