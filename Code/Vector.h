// Vector.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class Vector;

	typedef std::list< Vector > VectorList;
	typedef std::vector< Vector > VectorArray;
}

class _3DMATH_API _3DMath::Vector
{
public:

	Vector( void );
	Vector( const Vector& vector );
	Vector( double x, double y, double z );
	~Vector( void );

	void Set( double x, double y, double z );
	void Get( double& x, double& y, double& z ) const;

	void Add( const Vector& vector );
	void Add( const Vector& vectorA, const Vector& vectorB );

	void Subtract( const Vector& vector );
	void Subtract( const Vector& vectorA, const Vector& vectorB );

	void Multiply( const Vector& vector );
	void Multiply( const Vector& vectorA, const Vector& vectorB );

	void AddScale( const Vector& vector, double scale );
	void AddScale( const Vector& vectorA, double scaleA, const Vector& vectorB, double scaleB );
	void AddScale( const Vector& vectorA, double scaleA, const Vector& vectorB );
	void AddScale( const Vector& vectorA, const Vector& vectorB, double scaleB );

	void Negate( void );
	void GetNegated( Vector& vector ) const;
	void SetNegated( const Vector& vector );

	bool Normalize( void );
	bool GetNormalized( Vector& vector ) const;
	bool SetNormalized( const Vector& vector );

	void Scale( double scale );
	void GetScaled( Vector& vector, double scale ) const;
	void SetScaled( const Vector& vector, double scale );

	void Cross( const Vector& vector );
	void Cross( const Vector& vectorA, const Vector& vectorB );

	double Dot( const Vector& vector ) const;
	double Length( void ) const;
	double Distance( const Vector& vector ) const;
	double AngleBetween( const Vector& vector ) const;

	void Rotate( const Vector& unitAxis, double angle, Vector& vector ) const;
	void Rotate( const Vector& unitAxis, double angle );

	void ProjectOnto( const Vector& unitVector, Vector& vector ) const;
	void ProjectOnto( const Vector& unitVector );

	void RejectFrom( const Vector& unitVector, Vector& vector ) const;
	void RejectFrom( const Vector& unitVector );

	bool IsOrthogonalTo( const Vector& vector, double eps = EPSILON ) const;
	bool IsParallelWith( const Vector& vector, double eps = EPSILON ) const;
	bool IsEqualTo( const Vector& vector, double eps = EPSILON ) const;
	bool IsZero( double eps = EPSILON ) const;

	void Min( const Vector& vectorA, const Vector& vectorB );
	void Max( const Vector& vectorA, const Vector& vectorB );

	void Lerp( const Vector& vectorA, const Vector& vectorB, double lambda );
	bool Slerp( const Vector& unitVectorA, const Vector& unitVectorB, double lambda );

	bool Orthogonal( Vector& orthogonalVector ) const;

	void Reflect( Vector& vector ) const;

	double x, y, z;
};

namespace _3DMath
{
	inline Vector operator+( const Vector& vectorA, const Vector& vectorB )
	{
		Vector sum;
		sum.Add( vectorA, vectorB );
		return sum;
	}

	inline Vector operator-( const Vector& vectorA, const Vector& vectorB )
	{
		Vector diff;
		diff.Subtract( vectorA, vectorB );
		return diff;
	}

	// No, this is not the geometric product, but it is an important product.
	inline Vector operator*( const Vector& vectorA, const Vector& vectorB )
	{
		Vector prod;
		prod.Multiply( vectorA, vectorB );
		return prod;
	}

	inline Vector operator*( const Vector& vector, double scalar )
	{
		Vector prod;
		prod.SetScaled( vector, scalar );
		return prod;
	}

	inline Vector operator*( double scalar, const Vector& vector )
	{
		Vector prod;
		prod.SetScaled( vector, scalar );
		return prod;
	}
}

// Vector.h
