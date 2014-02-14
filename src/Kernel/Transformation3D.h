#	pragma once

#   include "stdex/intrusive_slug_list_size.h"
#   include "stdex/intrusive_slug.h"

#	include "math/mat4.h"

namespace Menge
{
    class Transformation3D;

    typedef stdex::intrusive_slug_list_size<Transformation3D> TListTransformation3DChild;
    typedef stdex::intrusive_slug<TListTransformation3DChild> TSlugTransformation3DChild;

	class Transformation3D
        : public stdex::intrusive_slug_linked<Transformation3D>
	{
	public:
		Transformation3D();

	public:
		void setRelationTransformation( Transformation3D * _relationTransformation );
		inline Transformation3D * getRelationTransformation() const;

    protected:
        void addRelationChildren( Transformation3D * _transformation );
        void removeRelationChildren( Transformation3D * _transformation );

	public:
		inline const mt::mat4f & getWorldMatrix() const;
		inline const mt::vec3f & getWorldPosition() const;

	public:
		void setWorldPosition( const mt::vec3f & _pos );

	public:
		inline const mt::mat4f & getLocalMatrix() const;

	public:
		void setLocalPosition( const mt::vec3f & _position );
		inline const mt::vec3f & getLocalPosition() const;

		void setOrigin( const mt::vec3f& _origin );
		inline const mt::vec3f& getOrigin() const;

		void setScale( const mt::vec3f& _scale );
		inline const mt::vec3f& getScale() const;

		void setRotateX( float _angle );
		inline float getRotateX() const;

		void setRotateY( float _angle );
		inline float getRotateY() const;

		void setRotateZ( float _angle );
		inline float getRotateZ() const;

		void setRotation( const mt::vec3f & _euler );
		inline const mt::vec3f & getRotation() const;

	public:
		void setCoordinate( const mt::vec3f& _coordinate );
		inline const mt::vec3f& getCoordinate() const;

	public:
		void setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f & _coordinate, const mt::vec3f& _scale, const mt::vec3f& _rotate );
        void getTransformation( mt::vec3f & _position, mt::vec3f& _origin, mt::vec3f & _coordinate, mt::vec3f& _scale, mt::vec3f& _rotate );

        void resetTransformation();
		
	public:
		void translate( const mt::vec3f & _delta );
		void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up );

	public:
		void invalidateWorldMatrix();        
		inline bool isInvalidateWorldMatrix() const;

	protected:
		virtual void _invalidateWorldMatrix();

    protected:
        void invalidateLocalMatrix();

	public:
		virtual void updateLocalMatrix() const;
		virtual void updateWorldMatrix() const;

	protected:
        mt::vec3f m_position;
		mt::vec3f m_origin;
		mt::vec3f m_coordinate;

		mt::vec3f m_scale;
		mt::vec3f m_euler;

		Transformation3D * m_relationTransformation;

        TListTransformation3DChild m_relationChild;

		mutable mt::mat4f m_localMatrix;
		mutable mt::mat4f m_worldMatrix;
		mutable bool m_invalidateWorldMatrix;
		mutable bool m_invalidateLocalMatrix;
	};
    //////////////////////////////////////////////////////////////////////////
    inline Transformation3D * Transformation3D::getRelationTransformation() const
    {
        return m_relationTransformation;
    }
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f & Transformation3D::getLocalPosition()const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getOrigin() const
	{
		return m_origin;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getCoordinate() const
	{
		return m_coordinate;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateX() const
	{
		return m_euler.x;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateY() const
	{
		return m_euler.y;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateZ() const
	{
		return m_euler.z;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation3D::getRotation() const
    {
        return m_euler;
    }
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation3D::isInvalidateWorldMatrix()const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getLocalMatrix() const
	{
		if( m_invalidateLocalMatrix == true )
		{
			this->updateLocalMatrix();
		}		

		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getWorldMatrix() const
	{
		if( m_invalidateWorldMatrix == true )
		{
			this->updateWorldMatrix();
		}

		return m_worldMatrix;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation3D::getWorldPosition() const
    {
        const mt::mat4f &wm = this->getWorldMatrix();

        const mt::vec3f &v3 = wm.v3.to_vec3f();

        return v3;
    }
}
