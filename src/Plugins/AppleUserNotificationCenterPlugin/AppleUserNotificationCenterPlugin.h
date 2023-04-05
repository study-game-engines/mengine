#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleUserNotificationCenterPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleUserNotificationCenter" )

    public:
        AppleUserNotificationCenterPlugin();
        ~AppleUserNotificationCenterPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
