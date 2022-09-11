#include "OSXUtils.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool OSXGetBuildShortVersionString( Char * const _version )
        {
            NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    
            if( version == nil )
            {
                return false;
            }
    
            const Char * version_str = [version UTF8String];
    
            MENGINE_STRCPY( _version, version );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool OSXGetBuildVersionKey( Char * const _version )
        {
            NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString *)kCFBundleVersionKey];
    
            if( version == nil )
            {
                return false;
            }
    
            const Char * version_str = [version UTF8String];
    
            MENGINE_STRCPY( _version, version );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool OSXGetMusicDirectory( Char * const _directory )
        {
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES);
    
            if( paths == nil )
            {
                return false;
            }
    
            NSString * path = [paths objectAtIndex:0];
    
            if( path == nil )
            {
                return false;
            }
    
            const Char * path_str = [path UTF8String];
    
            MENGINE_STRCPY( _directory, path_str );
            MENGINE_STRCAT( _directory, "/" );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool OSXGetPicturesDirectory( Char * const _directory )
        {
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSPicturesDirectory, NSUserDomainMask, YES);
    
            if( paths == nil )
            {
                return false;
            }
    
            NSString * path = [paths objectAtIndex:0];
    
            if( path == nil )
            {
                return false;
            }
        
            const Char * path_str = [path UTF8String];
        
            MENGINE_STRCPY( _directory, path_str );
            MENGINE_STRCAT( _directory, "/" );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool OSXSetDesktopWallpaper( const char * _url )
        { 
	        NSURL * url = [NSURL fileURLWithPath:@(_url)];

            NSScreen * currentScreen = [NSScreen mainScreen];
    
            if( currentScreen == nil )
            {
                return false;
            }
    	
            NSDictionary * screenOptions = [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:currentScreen];
    
            if( screenOptions == nil )
            {
                return false;
            }

            NSError *error = nil;
    
            if( [[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                        forScreen:currentScreen
                                                          options:screenOptions
                                                            error:&error] == FALSE )
            {
                return false;
            }

            return true;
        }
    }
}
