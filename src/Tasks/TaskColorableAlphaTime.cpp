#include "TaskColorableAlphaTime.h"

#include "Kernel/FactorableUnique.h"

#include "GOAP/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskColorableAlphaTimeAffector
            : public Affector
        {
        public:
            TaskColorableAlphaTimeAffector( GOAP::NodeInterface * _node, const ColorablePtr & _colorable, float  _from, float _to, float _time )
                : m_node( _node )
                , m_colorable( _colorable )
                , m_from( _from )
                , m_to( _to )
                , m_progress( 0.f )
                , m_time( _time )
            {
            }

            ~TaskColorableAlphaTimeAffector() override
            {
            }

        public:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                float time = _context->time;

                if( m_progress + time < m_time )
                {
                    *_used = time;

                    m_progress += time;

                    float t = m_progress / m_time;

                    const EasingInterfacePtr & easing = this->getEasing();

                    float easing_value = 0.f;
                    if( easing == nullptr )
                    {
                        easing_value = t;
                    }
                    else
                    {
                        easing_value = easing->easing( t );
                    }

                    float result = mt::linerp_f1( m_from, m_to, easing_value );

                    m_colorable->setLocalAlpha( result );

                    return false;
                }

                *_used = m_time - m_progress;

                m_colorable->setLocalAlpha( m_to );

                return true;
            }

        protected:
            void _complete( bool _isEnd ) override
            {
                if( _isEnd == true )
                {
                    m_node->complete();
                }
                else
                {
                    m_node->skip();
                }
            }

        protected:
            GOAP::NodeInterfacePtr m_node;

            ColorablePtr m_colorable;

            float m_from;
            float m_to;

            float m_progress;
            float m_time;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskColorableAlphaTime::TaskColorableAlphaTime( const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time )
        : m_colorable( _colorable )
        , m_affectorable( _affectorable )
        , m_easing( _easing )
        , m_to( _to )
        , m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskColorableAlphaTime::~TaskColorableAlphaTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskColorableAlphaTime::_onRun( GOAP::NodeInterface * _node )
    {
        const float alpha = m_colorable->getLocalAlpha();

        AffectorPtr affector = Helper::makeFactorableUnique<Detail::TaskColorableAlphaTimeAffector>( _node, m_colorable, alpha, m_to, m_time );

        affector->setEasing( m_easing );

        AFFECTOR_ID id = m_affectorable->addAffector( affector );

        if( id == 0 )
        {
            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskColorableAlphaTime::_onSkip()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_colorable->setLocalAlpha( m_to );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskColorableAlphaTime::_onFinally()
    {
        if( m_id != 0 )
        {
            m_affectorable->stopAffector( m_id );
            m_id = 0;
        }

        m_affectorable = nullptr;
        m_colorable = nullptr;
    }
}