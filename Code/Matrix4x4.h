// Matrix4x4.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class Vector;
	class Matrix4x4;
}

class _3DMATH_API _3DMath::Matrix4x4
{
public:
	Matrix4x4( void );
	virtual ~Matrix4x4( void );

	void SetRow( int row, const Vector& vector, double w );
	void SetCol( int col, const Vector& vector, double w );
	void SetIdentity( void );

	void GetRow( int row, Vector& vector, double& w ) const;
	void GetCol( int col, Vector& vector, double& w ) const;

	double Determinant( void ) const;

	void GetCopy( Matrix4x4& copy ) const;
	void SetCopy( const Matrix4x4& copy );

	void GetTranspose( Matrix4x4& tranpose ) const;
	void SetTranspose( const Matrix4x4& transpose );

	void GetInverse( Matrix4x4& inverse ) const;
	void SetInverse( const Matrix4x4& inverse );

	void Add( const Matrix4x4& leftMatrix, const Matrix4x4& rightMatrix );
	void Add( const Matrix4x4& matrix );

	void Multiply( const Matrix4x4& leftMatrix, const Matrix4x4& rightMatrix );
	void MultiplyRight( const Matrix4x4& rightMatrix );
	void MultiplyLeft( const Matrix4x4& leftMatrix );
	void MultiplyRight( const Vector& vector_a, double w, Vector& vector_b, double& w_b ) const;

	bool SolveLinearSystem( Vector& vector_a, double& w_a, const Vector& vector_b, double w_b ) const;

	double elements[4][4];
};

// Matrix4x4.h