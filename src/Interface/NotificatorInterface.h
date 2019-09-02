#pragma once

#include "Interface/RenderTextureInterface.h"

#include "Config/Typedef.h"
#include "Config/Tuple.h"
#include "Config/Char.h"

#include "Kernel/ConstString.h"
#include "Kernel/Factory.h"
#include "Kernel/Factorable.h"
#include "Kernel/Resolution.h"

#ifndef MENGINE_NOTIFICATOR_MAX_COUNT
#define MENGINE_NOTIFICATOR_MAX_COUNT 64
#endif

namespace Mengine
{
    typedef IntrusivePtr<class Resource> ResourcePtr;
    typedef IntrusivePtr<class Scene, class Node> ScenePtr;

    template<uint32_t ID>
    struct Notificator;

#define DECLARE_NOTIFICATOR(ID, NAME, ...)\
	static const uint32_t NAME = ID;\
	template<> struct Notificator<NAME> { typedef Tuple<__VA_ARGS__> args_type; }

    DECLARE_NOTIFICATOR( 1, NOTIFICATOR_DEBUG_OPEN_FILE, const Char *, const Char *, bool );
    DECLARE_NOTIFICATOR( 2, NOTIFICATOR_DEBUG_CLOSE_FILE, const Char *, const Char *, bool );
    DECLARE_NOTIFICATOR( 3, NOTIFICATOR_DEBUG_CREATE_OBJECT, Factory *, Factorable *, const Char * );
    DECLARE_NOTIFICATOR( 4, NOTIFICATOR_DEBUG_DESTROY_OBJECT, Factory *, Factorable * );
    DECLARE_NOTIFICATOR( 5, NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, bool, Resolution );
    DECLARE_NOTIFICATOR( 6, NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, ResourcePtr );
    DECLARE_NOTIFICATOR( 7, NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, Resource * );
    DECLARE_NOTIFICATOR( 8, NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, Resource * );
    DECLARE_NOTIFICATOR( 9, NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, ScenePtr, ScenePtr );
    DECLARE_NOTIFICATOR( 10, NOTIFICATOR_CHANGE_SCENE_DESTROY, ScenePtr );
    DECLARE_NOTIFICATOR( 11, NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, ScenePtr );
    DECLARE_NOTIFICATOR( 12, NOTIFICATOR_CHANGE_SCENE_INITIALIZE, ScenePtr );
    DECLARE_NOTIFICATOR( 13, NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 14, NOTIFICATOR_CHANGE_SCENE_ENABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 15, NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, ScenePtr );
    DECLARE_NOTIFICATOR( 16, NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, ScenePtr );
    DECLARE_NOTIFICATOR( 17, NOTIFICATOR_CHANGE_SCENE_COMPLETE, ScenePtr );
    DECLARE_NOTIFICATOR( 18, NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 19, NOTIFICATOR_RESTART_SCENE_DISABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 20, NOTIFICATOR_RESTART_SCENE_INITIALIZE, ScenePtr );
    DECLARE_NOTIFICATOR( 21, NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 22, NOTIFICATOR_RESTART_SCENE_ENABLE, ScenePtr );
    DECLARE_NOTIFICATOR( 23, NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, ScenePtr );
    DECLARE_NOTIFICATOR( 24, NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, ScenePtr );
    DECLARE_NOTIFICATOR( 25, NOTIFICATOR_RESTART_SCENE_COMPLETE, ScenePtr );
    DECLARE_NOTIFICATOR( 26, NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, ScenePtr );
    DECLARE_NOTIFICATOR( 27, NOTIFICATOR_REMOVE_SCENE_DESTROY );
    DECLARE_NOTIFICATOR( 28, NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE );
    DECLARE_NOTIFICATOR( 29, NOTIFICATOR_REMOVE_SCENE_COMPLETE );
    DECLARE_NOTIFICATOR( 30, NOTIFICATOR_CHANGE_LOCALE_PREPARE, ConstString, ConstString );
    DECLARE_NOTIFICATOR( 31, NOTIFICATOR_CHANGE_LOCALE, ConstString, ConstString );
    DECLARE_NOTIFICATOR( 32, NOTIFICATOR_CHANGE_LOCALE_POST, ConstString, ConstString );
    DECLARE_NOTIFICATOR( 33, NOTIFICATOR_DEBUG_TEXT_MODE, bool );
    DECLARE_NOTIFICATOR( 34, NOTIFICATOR_CHANGE_TEXT_ALIAS, ConstString, ConstString );
    DECLARE_NOTIFICATOR( 35, NOTIFICATOR_RELOAD_LOCALE_PREPARE );
    DECLARE_NOTIFICATOR( 36, NOTIFICATOR_RELOAD_LOCALE );
    DECLARE_NOTIFICATOR( 37, NOTIFICATOR_RELOAD_LOCALE_POST );
    DECLARE_NOTIFICATOR( 38, NOTIFICATOR_ENGINE_FINALIZE );
    DECLARE_NOTIFICATOR( 39, NOTIFICATOR_ENGINE_ENABLE_PACKAGES );
    DECLARE_NOTIFICATOR( 40, NOTIFICATOR_ENGINE_TEXTURE_DESTROY, RenderTextureInterface * );
    DECLARE_NOTIFICATOR( 41, NOTIFICATOR_SCRIPT_EMBEDDING );
    DECLARE_NOTIFICATOR( 42, NOTIFICATOR_SCRIPT_EMBEDDING_END );
    DECLARE_NOTIFICATOR( 43, NOTIFICATOR_SCRIPT_EJECTING );
    DECLARE_NOTIFICATOR( 44, NOTIFICATOR_BOOTSTRAPPER_RUN_FRAMEWORKS );
    DECLARE_NOTIFICATOR( 45, NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );

#undef DECLARE_NOTIFICATOR
}
