// Matrix4x4.cpp

#include "Matrix4x4.h"
#include "Vector.h"

using namespace _3DMath;

Matrix4x4::Matrix4x4( void )
{
	SetIdentity();
}

/*virtual*/ Matrix4x4::~Matrix4x4( void )
{
}

void Matrix4x4::SetRow( int row, const Vector& vector, double w )
{
	elements[row][0] = vector.x;
	elements[row][1] = vector.y;
	elements[row][2] = vector.z;
	elements[row][3] = w;
}

void Matrix4x4::SetCol( int col, const Vector& vector, double w )
{
	elements[0][col] = vector.x;
	elements[1][col] = vector.y;
	elements[2][col] = vector.z;
	elements[3][col] = w;
}

void Matrix4x4::SetIdentity( void )
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			elements[i][j] = ( i == j ) ? 1.0 : 0.0;
}

void Matrix4x4::GetRow( int row, Vector& vector, double& w ) const
{
	vector.x = elements[row][0];
	vector.y = elements[row][1];
	vector.z = elements[row][2];
	w = elements[row][3];
}

void Matrix4x4::GetCol( int col, Vector& vector, double& w ) const
{
	vector.x = elements[0][col];
	vector.y = elements[1][col];
	vector.z = elements[2][col];
	w = elements[3][col];
}

double Matrix4x4::Determinant( void ) const
{
	return 0.0; // TODO: Write this.
}

void Matrix4x4::GetCopy( Matrix4x4& copy ) const
{
	copy.SetCopy( *this );
}

void Matrix4x4::SetCopy( const Matrix4x4& copy )
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			elements[i][j] = copy.elements[i][j];
}

void Matrix4x4::GetTranspose( Matrix4x4& tranpose ) const
{
	tranpose.SetTranspose( *this );
}

void Matrix4x4::SetTranspose( const Matrix4x4& transpose )
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			elements[i][j] = transpose.elements[j][i];
}

void Matrix4x4::GetInverse( Matrix4x4& inverse ) const
{
	inverse.SetInverse( *this );
}

void Matrix4x4::SetInverse( const Matrix4x4& inverse )
{
	// TODO: Write this.
}

void Matrix4x4::Add( const Matrix4x4& leftMatrix, const Matrix4x4& rightMatrix )
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			elements[i][j] = leftMatrix.elements[i][j] + rightMatrix.elements[i][j];
}

void Matrix4x4::Add( const Matrix4x4& matrix )
{
	for( int i = 0; i < 4; i++ )
		for( int j = 0; j < 4; j++ )
			elements[i][j] += matrix.elements[i][j];
}

void Matrix4x4::Multiply( const Matrix4x4& leftMatrix, const Matrix4x4& rightMatrix )
{
	// TODO: Write this.
}

void Matrix4x4::MultiplyRight( const Matrix4x4& rightMatrix )
{
	Matrix4x4 leftMatrix;
	leftMatrix.SetCopy( *this );
	Multiply( leftMatrix, rightMatrix );
}

void Matrix4x4::MultiplyLeft( const Matrix4x4& leftMatrix )
{
	Matrix4x4 rightMatrix;
	rightMatrix.SetCopy( *this );
	Multiply( leftMatrix, rightMatrix );
}

void Matrix4x4::MultiplyRight( const Vector& vector_a, double w_a, Vector& vector_b, double& w_b ) const
{
	// Calculate b = Ma.
	vector_b.x = elements[0][0] * vector_a.x + elements[0][1] * vector_a.y + elements[0][2] * vector_a.z + elements[0][3] * w_a;
	vector_b.y = elements[1][0] * vector_a.x + elements[1][1] * vector_a.y + elements[1][2] * vector_a.z + elements[1][3] * w_a;
	vector_b.z = elements[2][0] * vector_a.x + elements[2][1] * vector_a.y + elements[2][2] * vector_a.z + elements[2][3] * w_a;
	w_b = elements[3][0] * vector_a.x + elements[3][1] * vector_a.y + elements[3][2] * vector_a.z + elements[3][3] * w_a;
}

bool Matrix4x4::SolveLinearSystem( Vector& vector_a, double& w_a, const Vector& vector_b, double w_b ) const
{
	// Solve the equation Ma = b for a given M and b.
	// We might just calculate a = M^{-1}b, but this doesn't work if b is zero.

	Matrix4x4 reduce;
	reduce.SetCopy( *this );

	// Here we reduce M into upper-triangular form.  Notice that we don't try to find the most numerically stable solution.
	
	for( int q = 0; q < 3; q++ )
	{
		for( int i = q + 1; i < 4; i++ )
		{
			double scale = -reduce.elements[i][q] / reduce.elements[q][q];
			for( int j = 0; j < 4; j++ )
				reduce.elements[i][j] += reduce.elements[q][j] * scale;
		}
	}

	for( int i = 0; i < 4; i++ )
	{
		if( reduce.elements[i][i] == 0.0 )
			return false; // Singular matrix.

		double scale = 1.0 / reduce.elements[i][i];
		for( int j = i; j < 4; j++ )
			reduce.elements[i][j] *= scale;
	}

	// We can now solve by substitution.

	w_a = w_b;
	vector_a.z = vector_b.z - reduce.elements[2][3] * w_a;
	vector_a.y = vector_b.y - reduce.elements[1][2] * vector_a.z + reduce.elements[1][3] * w_a;
	vector_a.x = vector_b.x - reduce.elements[0][1] * vector_a.y + reduce.elements[0][2] * vector_a.z + reduce.elements[0][3] * w_a;

	return true;
}

// Matrix4x4.cpp