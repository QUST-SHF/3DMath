// LinearTransform.cpp

#include "LinearTransform.h"

using namespace _3DMath;

LinearTransform::LinearTransform( void )
{
	Identity();
}

LinearTransform::LinearTransform( const LinearTransform& linearTransform )
{
	xAxis = linearTransform.xAxis;
	yAxis = linearTransform.yAxis;
	zAxis = linearTransform.zAxis;
}

LinearTransform::LinearTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis )
{
	Set( xAxis, yAxis, zAxis );
}

/*virtual*/ LinearTransform::~LinearTransform( void )
{
}

/*virtual*/ bool LinearTransform::Evaluate( const Vector& input, Vector& output ) const
{
	Transform( input, output );
	return true;
}

/*virtual*/ bool LinearTransform::EvaluateDirectionalDerivative( const Vector& input, const Vector& direction, Vector& output, double approxDelta /*= 1e-4*/ ) const
{
	// Interestingly, if I've done my math right, because we're a linear function, we get...
	Transform( direction, output );
	return true;
}

void LinearTransform::Set( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis )
{
	this->xAxis = xAxis;
	this->yAxis = yAxis;
	this->zAxis = zAxis;
}

void LinearTransform::Get( Vector& xAxis, Vector& yAxis, Vector& zAxis ) const
{
	xAxis = this->xAxis;
	yAxis = this->yAxis;
	zAxis = this->zAxis;
}

void LinearTransform::Identity( void )
{
	xAxis.Set( 1.0, 0.0, 0.0 );
	yAxis.Set( 0.0, 1.0, 0.0 );
	zAxis.Set( 0.0, 0.0, 1.0 );
}

double LinearTransform::Determinant( void ) const
{
	Vector cross;
	cross.Cross( yAxis, zAxis );
	double dot = xAxis.Dot( cross );
	return dot;
}

void LinearTransform::Transform( Vector& vector ) const
{
	Transform( vector, vector );
}

void LinearTransform::Transform( const Vector& vectorA, Vector& vectorB ) const
{
	Vector transformedVector;
	transformedVector.AddScale( xAxis, vectorA.x, yAxis, vectorA.y );
	transformedVector.AddScale( zAxis, vectorA.z );
	vectorB = transformedVector;
}

void LinearTransform::Transform( Vector* vectorArray, int arraySize ) const
{
	for( int i = 0; i < arraySize; i++ )
		Transform( vectorArray[i] );
}

bool LinearTransform::Invert( void )
{
	return GetInverse( *this );
}

bool LinearTransform::GetInverse( LinearTransform& linearTransform ) const
{
	double det = Determinant();
	if( det == 0.0 )
		return false;

	double recipDet = 1.0 / det;

	Vector xAxisA, yAxisA, zAxisA;

	xAxisA.x = yAxis.y * zAxis.z - zAxis.y * yAxis.z;
	xAxisA.y = xAxis.z * zAxis.y - zAxis.z * xAxis.y;
	xAxisA.z = xAxis.y * yAxis.z - yAxis.y * xAxis.z;

	yAxisA.x = yAxis.z * zAxis.x - zAxis.z * yAxis.x;
	yAxisA.y = xAxis.x * zAxis.z - zAxis.x * xAxis.z;
	yAxisA.z = xAxis.z * yAxis.x - yAxis.z * xAxis.x;

	zAxisA.x = yAxis.x * zAxis.y - zAxis.x * yAxis.y;
	zAxisA.y = xAxis.y * zAxis.x - zAxis.y * xAxis.x;
	zAxisA.z = xAxis.x * yAxis.y - yAxis.x * xAxis.y;

	linearTransform.xAxis.SetScaled( xAxisA, recipDet );
	linearTransform.yAxis.SetScaled( yAxisA, recipDet );
	linearTransform.zAxis.SetScaled( zAxisA, recipDet );

	return true;
}

bool LinearTransform::SetInverse( const LinearTransform& linearTransform )
{
	return linearTransform.GetInverse( *this );
}

void LinearTransform::Tranpose( void )
{
	GetTranspose( *this );
}

void LinearTransform::GetTranspose( LinearTransform& linearTransform ) const
{
	Vector xAxisT, yAxisT, zAxisT;

	xAxisT.Set( xAxis.x, yAxis.x, zAxis.x );
	yAxisT.Set( xAxis.y, yAxis.y, zAxis.y );
	zAxisT.Set( xAxis.z, yAxis.z, zAxis.z );

	linearTransform.xAxis = xAxisT;
	linearTransform.yAxis = yAxisT;
	linearTransform.zAxis = zAxisT;
}

void LinearTransform::SetTranspose( const LinearTransform& linearTransform )
{
	return linearTransform.GetTranspose( *this );
}

void LinearTransform::Concatinate( const LinearTransform& linearTransform )
{
	LinearTransform concatinatedTransform;
	concatinatedTransform.Concatinate( *this, linearTransform );
	*this = concatinatedTransform;
}

void LinearTransform::Concatinate( const LinearTransform& linearTransformA, const LinearTransform& linearTransformB )
{
	linearTransformB.Transform( linearTransformA.xAxis, xAxis );
	linearTransformB.Transform( linearTransformA.yAxis, yAxis );
	linearTransformB.Transform( linearTransformA.zAxis, zAxis );
}

void LinearTransform::SetRotation( const Vector& unitAxis, double angle )
{
	Identity();

	xAxis.Rotate( unitAxis, angle );
	yAxis.Rotate( unitAxis, angle );
	zAxis.Rotate( unitAxis, angle );
}

void LinearTransform::Multiply( const Vector& vectorA, const Vector& vectorB )
{
	xAxis.SetScaled( vectorB, vectorA.x );
	yAxis.SetScaled( vectorB, vectorA.y );
	zAxis.SetScaled( vectorB, vectorA.z );
}

bool LinearTransform::GetRotation( Vector& unitAxis, double& angle ) const
{
	// The rotation axis is an Eigen vector.
	return false;
}

void LinearTransform::SetScale( double scale )
{
	xAxis.Set( scale, 0.0, 0.0 );
	yAxis.Set( 0.0, scale, 0.0 );
	zAxis.Set( 0.0, 0.0, scale );
}

// TODO: We may want to provide an argument that lets the caller specify which of the 3 axes to use as anchor.
bool LinearTransform::Orthogonalize( void )
{
	if( Determinant() == 0.0 )
		return false;

	yAxis.RejectFrom( xAxis );
	zAxis.RejectFrom( xAxis );
	zAxis.RejectFrom( yAxis );

	xAxis.Normalize();
	yAxis.Normalize();
	zAxis.Normalize();

	return true;
}

bool LinearTransform::Decompose( LinearTransform& scale, LinearTransform& shear, LinearTransform& rotation )
{
	// This exists if and only if we have a linearly independent set.
	// As I recall, we can get the shear matrix using the Grahm-Schmidt process.
	// Still other decompositions may be more useful.
	return false;
}

bool LinearTransform::GetNormalTransform( LinearTransform& normalTransform ) const
{
	if( !GetInverse( normalTransform ) )
		return false;

	normalTransform.Tranpose();
	return true;
}

bool LinearTransform::BuildFrameUsingVector( const Vector& vector )
{
	if( !vector.GetNormalized( zAxis ) )
		return false;

	zAxis.Orthogonal( xAxis );
	xAxis.Normalize();
	yAxis.Cross( zAxis, xAxis );
	return true;
}

// LinearTransform.cpp