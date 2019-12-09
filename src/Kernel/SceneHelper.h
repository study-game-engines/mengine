#pragma once

#include "Kernel/Scene.h"

namespace Mengine
{
    namespace Helper
    {
        ScenePtr makeScene( const SceneEventReceiverPtr & _eventReceiver, const DocumentPtr & _doc );
    }
}