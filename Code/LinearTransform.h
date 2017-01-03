// LinearTransform.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Function.h"

namespace _3DMath
{
	class LinearTransform;
	class Vector;
}

class _3DMATH_API _3DMath::LinearTransform : public _3DMath::VectorField
{
public:

	LinearTransform( void );
	LinearTransform( const LinearTransform& linearTransform );
	LinearTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis );
	virtual ~LinearTransform( void );

	virtual bool Evaluate( const Vector& input, Vector& output ) const override;
	virtual bool EvaluateDirectionalDerivative( const Vector& input, const Vector& direction, Vector& output, double approxDelta = 1e-4 ) const override;

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

	void SetScale( double scale );

	void Multiply( const Vector& vectorA, const Vector& vectorB );

	bool Orthogonalize( void );
	bool Decompose( LinearTransform& scale, LinearTransform& shear, LinearTransform& rotation );

	bool GetNormalTransform( LinearTransform& normalTransform ) const;

	bool BuildFrameUsingVector( const Vector& vector );

	Vector xAxis, yAxis, zAxis;
};

namespace _3DMath
{
	inline Vector operator*( const Vector& vector, const LinearTransform& transform )
	{
		Vector result;
		transform.Transform( vector, result );
		return result;
	}
}

// LinearTransform.h
