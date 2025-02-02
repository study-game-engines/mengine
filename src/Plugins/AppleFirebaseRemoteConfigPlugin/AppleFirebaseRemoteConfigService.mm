#include "AppleFirebaseRemoteConfigService.h"

#import <Firebase/Firebase.h>
#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFirebaseRemoteConfigService, Mengine::AppleFirebaseRemoteConfigService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigService::AppleFirebaseRemoteConfigService()
        : m_remoteConfig( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigService::~AppleFirebaseRemoteConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigService::_initializeService()
    {
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigService::_finalizeService()
    {
        m_remoteConfig = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigService::setRemoteConfig( NSMutableDictionary * _remoteConfig )
    {
        m_remoteConfig = _remoteConfig;
    }
    //////////////////////////////////////////////////////////////////////////
    NSMutableDictionary * AppleFirebaseRemoteConfigService::getRemoteConfig() const
    {
        return m_remoteConfig;
    }
    //////////////////////////////////////////////////////////////////////////
}
