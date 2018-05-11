#pragma once

#include "Kernel/Loadable.h"

#include "Kernel/Servant.h"
#include "Kernel/Resource.h"
#include "Kernel/Identity.h"
#include "Kernel/Reference.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Comable.h"

#include "Kernel/ResourceHolder.h"

#include "Factory/Factorable.h"
#include "Factory/Factory.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

namespace Mengine
{
    class ResourceVisitor;
	
	class ResourceReference
		: public Servant
		, public Resource
		, public Identity
		, public Reference
		, public Loadable
        , public Scriptable
		, public Visitable
	{
		DECLARE_VISITABLE_BASE();

	public:
		ResourceReference();
		~ResourceReference() override;

	public:
		void setLocale( const ConstString & _locale );
		inline const ConstString & getLocale() const;

		void setCategory( const ConstString & _category );
		inline const ConstString & getCategory() const;

		void setGroup( const ConstString & _group );
		inline const ConstString & getGroup() const;

	public:
		bool initialize();

	protected:
		virtual bool _initialize();

	public:
		bool isValid() const;

    protected:
        virtual bool _isValid() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		
    public:
        bool convert();

    protected:
        virtual bool _convert();

	protected:
		bool convertDefault2_( const ConstString & _converter, const FilePath & _path, FilePath & _out );
		bool convertDefault_( const ConstString & _converter, const FilePath & _path, FilePath & _out, ConstString & _codecType );

	public:
		bool cache();
		void uncache();

	public:
		inline bool isCache() const;

	protected:
		virtual void _cache();
		virtual void _uncache();

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:       
		ConstString m_locale;
		ConstString m_category;
		ConstString m_group;

		bool m_cache;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceReference> ResourceReferencePtr;
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getGroup() const
	{
		return m_group;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool ResourceReference::isCache() const
	{
		return m_cache;
	}
}
