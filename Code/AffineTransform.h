// AffineTransform.h

#pragma once

#include "Defines.h"
#include "LinearTransform.h"
#include "Renderer.h"

namespace _3DMath
{
	class AffineTransform;
	class Vertex;
}

class _3DMATH_API _3DMath::AffineTransform
{
public:

	AffineTransform( void );
	AffineTransform( const AffineTransform& affineTransform );
	AffineTransform( const LinearTransform& linearTransform, const Vector& translation );
	AffineTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& translation );
	~AffineTransform( void );

	void Identity( void );

	double Determinant( void ) const;

	void Transform( Vector& vector ) const;
	void Transform( const Vector& vectorA, Vector& vectorB ) const;
	void Transform( Vector* vectorArray, int arraySize ) const;
	void Transform( VertexArray& vertexArray, bool transformNormals = false ) const;

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

	LinearTransform linearTransform;
	Vector translation;
};

// AffineTransform.h
