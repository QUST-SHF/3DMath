// AffineTransform.h

#pragma once

#include "Defines.h"
#include "LinearTransform.h"
#include "Renderer.h"
#include "Function.h"

namespace _3DMath
{
	class AffineTransform;
	class Vertex;
	class Line;
}

class _3DMATH_API _3DMath::AffineTransform : public _3DMath::VectorField
{
public:

	AffineTransform( void );
	AffineTransform( const AffineTransform& affineTransform );
	AffineTransform( const LinearTransform& linearTransform, const Vector& translation );
	AffineTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& translation );
	virtual ~AffineTransform( void );

	virtual bool Evaluate( const Vector& input, Vector& output ) const override;

	void Identity( void );

	double Determinant( void ) const;

	void Transform( Vector& vector ) const;
	void Transform( const Vector& vectorA, Vector& vectorB ) const;
	void Transform( Vector* vectorArray, int arraySize ) const;
	void Transform( Vertex& vertex, const LinearTransform* normalTransform = nullptr ) const;
	bool Transform( VertexArray& vertexArray ) const;
	bool Transform( VectorArray& vectorArray ) const;

	bool Invert( void );
	bool GetInverse( AffineTransform& affineTransform ) const;
	bool SetInverse( const AffineTransform& affineTransform );

	void Transpose( void );
	void GetTranspose( AffineTransform& affineTransform ) const;
	void SetTranspose( const AffineTransform& affineTransform );

	void Concatinate( const AffineTransform& affineTransform );
	void Concatinate( const AffineTransform& affineTransformA, const AffineTransform& affineTransformB );

	void SetRigidBodyMotion( const Vector& unitAxis, double angle, const Vector& translation );
	bool GetRigidBodyMotion( Vector& unitAxis, double& angle, Vector& translation ) const;

	void SetRotation( const Line& line, double angle );
	bool GetRotation( Line& line, double& angle ) const;

	LinearTransform linearTransform;
	Vector translation;
};

namespace _3DMath
{
	inline Vector operator*( const Vector& vector, const AffineTransform& transform )
	{
		Vector result;
		transform.Transform( vector, result );
		return result;
	}
}

// AffineTransform.h