// LinearTransform.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class LinearTransform;
	class Vector;
}

class _3DMATH_API _3DMath::LinearTransform
{
public:

	LinearTransform( void );
	LinearTransform( const LinearTransform& linearTransform );
	LinearTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis );
	~LinearTransform( void );

	void Identity( void );

	double Determinant( void ) const;

	void Set( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis );
	void Get( Vector& xAxis, Vector& yAxis, Vector& zAxis ) const;

	void Transform( Vector& vector ) const;
	void Transform( const Vector& vectorA, Vector& vectorB ) const;
	void Transform( Vector* vectorArray, int arraySize ) const;

	bool Invert( void );
	bool GetInverse( LinearTransform& linearTransform ) const;
	bool SetInverse( const LinearTransform& linearTransform );

	void Tranpose( void );
	void GetTranspose( LinearTransform& linearTransform ) const;
	void SetTranspose( const LinearTransform& linearTransform );

	void Concatinate( const LinearTransform& linearTransform );
	void Concatinate( const LinearTransform& linearTransformA, const LinearTransform& linearTransformB );

	void SetRotation( const Vector& unitAxis, double angle );
	bool GetRotation( Vector& unitAxis, double& angle ) const;

	void Multiply( const Vector& vectorA, const Vector& vectorB );

	bool Orthogonalize( void );
	bool Decompose( LinearTransform& scale, LinearTransform& shear, LinearTransform& rotation );

	Vector xAxis, yAxis, zAxis;
};

// LinearTransform.h
