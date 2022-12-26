#include "AnalyticsContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContext::AnalyticsContext()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContext::~AnalyticsContext()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsContext::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
    {
        m_parameters.emplace( _name, _parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsEventParameterInterfacePtr & AnalyticsContext::getParameter( const ConstString & _name ) const
    {
        const AnalyticsEventParameterInterfacePtr & parameter = m_parameters.find( _name );

        return parameter;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterBoolean( const ConstString & _name, bool _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterBoolean( _value );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterString( const ConstString & _name, const String & _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterString( _value );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterInteger( const ConstString & _name, int64_t _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterInteger( _value );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterDouble( const ConstString & _name, double _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterDouble( _value );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventGetterParameterBoolean( _lambda );
        
        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventGetterParameterString( _lambda );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventGetterParameterInteger( _lambda );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsContext::setParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventGetterParameterDouble( _lambda );

        this->addParameter( _name, parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContextInterfacePtr AnalyticsContext::resolveContext() const
    {
        AnalyticsContextInterfacePtr context = m_analyticsFactory->makeAnalyticsContext();

        for( const HashtableAnalyticsEventParameter::value_type & value : m_parameters )
        {
            const ConstString & name = value.key;
            const AnalyticsEventParameterInterfacePtr & parameter = value.element;

            AnalyticsEventParameterInterfacePtr resolve_parameter = parameter->resolveParameter();

            context->addParameter( name, resolve_parameter );
        }

        return context;
    }
    //////////////////////////////////////////////////////////////////////////
}