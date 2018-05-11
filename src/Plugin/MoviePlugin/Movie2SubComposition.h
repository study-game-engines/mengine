#pragma once

#include "Factory/Factorable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Animatable.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
    class Movie2SubCompositionEventReceiver
        : public AnimatableEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
	class Movie2SubComposition
        : public Factorable
		, public Eventable
        , public Animatable
	{
        EVENT_RECEIVER( Movie2SubCompositionEventReceiver );

    public:
        Movie2SubComposition();
        ~Movie2SubComposition() override;

    public:
        void setMovie( const Movie2Ptr & _movie );
        const Movie2Ptr & getMovie() const;
        
    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        Movie2Ptr m_movie;

        const aeMovieComposition * m_composition;
        const aeMovieSubComposition * m_subcomposition;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
}
