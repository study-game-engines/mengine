#pragma once

#include "Kernel/PluginBase.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MoviePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Movie" )

    public:
        MoviePlugin();
        ~MoviePlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        const aeMovieInstance * m_instance;

        String m_hashkey;
    };
}