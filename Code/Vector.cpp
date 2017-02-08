// Vector.cpp

#include "Vector.h"

using namespace _3DMath;

Vector::Vector( void )
{
	Set( 0.0, 0.0, 0.0 );
}

Vector::Vector( const Vector& vector )
{
	Set( vector.x, vector.y, vector.z );
}

Vector::Vector( double x, double y, double z )
{
	Set( x, y, z );
}

Vector::~Vector( void )
{
}

void Vector::Set( double x, double y, double z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector::Get( double& x, double& y, double& z ) const
{
	x = this->x;
	y = this->y;
	z = this->z;
}

void Vector::Add( const Vector& vector )
{
	Add( *this, vector );
}

void Vector::Add( const Vector& vectorA, const Vector& vectorB )
{
	x = vectorA.x + vectorB.x;
	y = vectorA.y + vectorB.y;
	z = vectorA.z + vectorB.z;
}

void Vector::Subtract( const Vector& vector )
{
	Subtract( *this, vector );
}

void Vector::Subtract( const Vector& vectorA, const Vector& vectorB )
{
	x = vectorA.x - vectorB.x;
	y = vectorA.y - vectorB.y;
	z = vectorA.z - vectorB.z;
}

void Vector::Multiply( const Vector& vector )
{
	Multiply( *this, vector );
}

void Vector::Multiply( const Vector& vectorA, const Vector& vectorB )
{
	x = vectorA.x * vectorB.x;
	y = vectorA.y * vectorB.y;
	z = vectorA.z * vectorB.z;
}

void Vector::AddScale( const Vector& vector, double scale )
{
	x += vector.x * scale;
	y += vector.y * scale;
	z += vector.z * scale;
}

void Vector::AddScale( const Vector& vectorA, double scaleA, const Vector& vectorB, double scaleB )
{
	x = vectorA.x * scaleA + vectorB.x * scaleB;
	y = vectorA.y * scaleA + vectorB.y * scaleB;
	z = vectorA.z * scaleA + vectorB.z * scaleB;
}

void Vector::AddScale( const Vector& vectorA, double scaleA, const Vector& vectorB )
{
	x = vectorA.x * scaleA + vectorB.x;
	y = vectorA.y * scaleA + vectorB.y;
	z = vectorA.z * scaleA + vectorB.z;
}

void Vector::AddScale( const Vector& vectorA, const Vector& vectorB, double scaleB )
{
	x = vectorA.x + vectorB.x * scaleB;
	y = vectorA.y + vectorB.y * scaleB;
	z = vectorA.z + vectorB.z * scaleB;
}

void Vector::Negate( void )
{
	GetNegated( *this );
}

void Vector::GetNegated( Vector& vector ) const
{
	vector.x = -x;
	vector.y = -y;
	vector.z = -z;
}

void Vector::SetNegated( const Vector& vector )
{
	vector.GetNegated( *this );
}

bool Vector::Normalize( void )
{
	return GetNormalized( *this );
}

bool Vector::GetNormalized( Vector& vector ) const
{
	double length = Length();
	if( length == 0.0 )
		return false;
	
	vector = *this;
	vector.Scale( 1.0 / length );
	return true;
}

bool Vector::SetNormalized( const Vector& vector )
{
	return vector.GetNormalized( *this );
}

void Vector::Scale( double scale )
{
	GetScaled( *this, scale );
}

void Vector::GetScaled( Vector& vector, double scale ) const
{
	vector.x = x * scale;
	vector.y = y * scale;
	vector.z = z * scale;
}

void Vector::SetScaled( const Vector& vector, double scale )
{
	x = vector.x * scale;
	y = vector.y * scale;
	z = vector.z * scale;
}

void Vector::Cross( const Vector& vector )
{
	Cross( *this, vector );
}

void Vector::Cross( const Vector& vectorA, const Vector& vectorB )
{
	double x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	double y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	double z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;

	this->x = x;
	this->y = y;
	this->z = z;
}

double Vector::Dot( const Vector& vector ) const
{
	double dot = 0.0;

	dot += x * vector.x;
	dot += y * vector.y;
	dot += z * vector.z;

	return dot;
}

double Vector::Length( void ) const
{
	return sqrt( Dot( *this ) );
}

double Vector::Distance( const Vector& vector ) const
{
	Vector diff;
	diff.Subtract( *this, vector );
	return diff.Length();
}

double Vector::AngleBetween( const Vector& vector ) const
{
	Vector unitVectorA, unitVectorB;
	GetNormalized( unitVectorA );
	vector.GetNormalized( unitVectorB );
	double angle = acos( unitVectorA.Dot( unitVectorB ) );
	return angle;
}

void Vector::Rotate( const Vector& unitAxis, double angle, Vector& vector ) const
{
	Vector rejVector, projVector;
	Vector xAxis, yAxis;

	unitAxis.GetScaled( projVector, Dot( unitAxis ) );
	rejVector.Subtract( *this, projVector );

	double rejVectorLength = rejVector.Length();
	if( rejVectorLength != 0.0 )
	{
		xAxis.SetScaled( rejVector, 1.0 / rejVectorLength );
		yAxis.Cross( unitAxis, xAxis );

		double cosAngle = cos( angle );
		double sinAngle = sin( angle );

		rejVector.AddScale( xAxis, cosAngle, yAxis, sinAngle );
		rejVector.Scale( rejVectorLength );
	}

	vector.Add( projVector, rejVector );
}

void Vector::Rotate( const Vector& unitAxis, double angle )
{
	Rotate( unitAxis, angle, *this );
}

void Vector::ProjectOnto( const Vector& unitVector, Vector& vector ) const
{
	double length = Dot( unitVector );
	unitVector.GetScaled( vector, length );
}

void Vector::ProjectOnto( const Vector& unitVector )
{
	ProjectOnto( unitVector, *this );
}

void Vector::RejectFrom( const Vector& unitVector, Vector& vector ) const
{
	Vector projVector;
	ProjectOnto( unitVector, projVector );
	vector.Subtract( *this, projVector );
}

void Vector::RejectFrom( const Vector& unitVector )
{
	RejectFrom( unitVector, *this );
}

bool Vector::IsOrthogonalTo( const Vector& vector, double eps /*= EPSILON*/ ) const
{
	double dot = Dot( vector );
	return( fabs( dot ) < eps ? true : false );
}

bool Vector::IsParallelWith( const Vector& vector, double eps /*= EPSILON*/ ) const
{
	Vector cross;
	cross.Cross( *this, vector );
	double area = cross.Length();
	return( area < eps ? true : false );
}

bool Vector::IsEqualTo( const Vector& vector, double eps /*= EPSILON*/ ) const
{
	Vector diff;
	diff.Subtract( vector, *this );
	double length = diff.Length();
	return( length < eps ? true : false );
}

bool Vector::IsZero( double eps /*= EPSILON*/ ) const
{
	double length = Length();
	return( length < eps ? true : false );
}

void Vector::Min( const Vector& vectorA, const Vector& vectorB )
{
	x = MIN( vectorA.x, vectorB.x );
	y = MIN( vectorA.y, vectorB.y );
	z = MIN( vectorA.z, vectorB.z );
}

void Vector::Max( const Vector& vectorA, const Vector& vectorB )
{
	x = MAX( vectorA.x, vectorB.x );
	y = MAX( vectorA.y, vectorB.y );
	z = MAX( vectorA.z, vectorB.z );
}

void Vector::Lerp( const Vector& vectorA, const Vector& vectorB, double lambda )
{
	AddScale( vectorA, 1.0 - lambda, vectorB, lambda );
}

bool Vector::Slerp( const Vector& unitVectorA, const Vector& unitVectorB, double lambda )
{
	double angle = acos( unitVectorA.Dot( unitVectorB ) );
	double sinAngle = sin( angle );
	if( sinAngle == 0.0 )
		return false;

	double scaleA = sin( ( 1.0 - lambda ) * angle ) / sinAngle;
	double scaleB = sin( lambda * angle ) / sinAngle;
	AddScale( unitVectorA, scaleA, unitVectorB, scaleB );
	return true;
}

bool Vector::Orthogonal( Vector& orthogonalVector ) const
{
	if( x != 0.0 )
		orthogonalVector.Set( -y, x, 0.0 );
	else if( y != 0.0 )
		orthogonalVector.Set( 0.0, -z, y );
	else if( z != 0.0 )
		orthogonalVector.Set( z, 0.0, -x );
	else
		return false;
	return true;
}

void Vector::Reflect( Vector& vector ) const
{
	//...
}

// Vector.cpp