#import "Environment/iOS/UIKitProxyApplicationDelegateInterface.h"

#import <FirebaseMessaging/FirebaseMessaging.h>

@interface AppleFirebaseMessagingApplicationDelegate : NSObject<UIKitProxyApplicationDelegateInterface, FIRMessagingDelegate>

@end
