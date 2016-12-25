// AffineTransform.cpp

#include "AffineTransform.h"

using namespace _3DMath;

AffineTransform::AffineTransform( void )
{
	Identity();
}

AffineTransform::AffineTransform( const AffineTransform& affineTransform )
{
	linearTransform = affineTransform.linearTransform;
	translation = affineTransform.translation;
}

AffineTransform::AffineTransform( const LinearTransform& linearTransform, const Vector& translation )
{
	this->linearTransform = linearTransform;
	this->translation = translation;
}

AffineTransform::AffineTransform( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& translation )
{
	linearTransform.Set( xAxis, yAxis, zAxis );
	this->translation = translation;
}

AffineTransform::~AffineTransform( void )
{
}

void AffineTransform::Identity( void )
{
	linearTransform.Identity();
	translation.Set( 0.0, 0.0, 0.0 );
}

double AffineTransform::Determinant( void ) const
{
	return linearTransform.Determinant();
}

void AffineTransform::Transform( Vector& vector ) const
{
	Transform( vector, vector );
}

void AffineTransform::Transform( const Vector& vectorA, Vector& vectorB ) const
{
	linearTransform.Transform( vectorA, vectorB );
	vectorB.Add( translation );
}

void AffineTransform::Transform( Vector* vectorArray, int arraySize ) const
{
	for( int i = 0; i < arraySize; i++ )
		Transform( vectorArray[i] );
}

void AffineTransform::Transform( Vertex& vertex, const LinearTransform* normalTransform /*= nullptr*/ ) const
{
	Transform( vertex.position );

	if( !normalTransform )
	{
		static LinearTransform normalTransformStorage;
		normalTransform = &normalTransformStorage;
		linearTransform.GetNormalTransform( normalTransformStorage );
	}

	normalTransform->Transform( vertex.normal );
	vertex.normal.Normalize();		// Account for accumulated round-off error.
}

bool AffineTransform::Transform( VertexArray& vertexArray ) const
{
	LinearTransform normalTransform;
	if( !linearTransform.GetNormalTransform( normalTransform ) )
		return false;

	for( int i = 0; i < ( signed )vertexArray.size(); i++ )
		Transform( vertexArray[i], &normalTransform );

	return true;
}

bool AffineTransform::Invert( void )
{
	return GetInverse( *this );
}

bool AffineTransform::GetInverse( AffineTransform& affineTransform ) const
{
	if( !linearTransform.GetInverse( affineTransform.linearTransform ) )
		return false;

	translation.GetNegated( affineTransform.translation );
	affineTransform.linearTransform.Transform( affineTransform.translation );
	return true;
}

bool AffineTransform::SetInverse( const AffineTransform& affineTransform )
{
	return affineTransform.GetInverse( *this );
}

void AffineTransform::Transpose( void )
{
	GetTranspose( *this );
}
void AffineTransform::GetTranspose( AffineTransform& affineTransform ) const
{
	linearTransform.GetTranspose( affineTransform.linearTransform );
	translation.GetNegated( affineTransform.translation );
	affineTransform.linearTransform.Transform( affineTransform.translation );
}

void AffineTransform::SetTranspose( const AffineTransform& affineTransform )
{
	affineTransform.GetTranspose( *this );
}

void AffineTransform::Concatinate( const AffineTransform& affineTransform )
{
	AffineTransform concatinatedTransform;
	concatinatedTransform.Concatinate( *this, affineTransform );
	*this = concatinatedTransform;
}

void AffineTransform::Concatinate( const AffineTransform& affineTransformA, const AffineTransform& affineTransformB )
{
	linearTransform.Concatinate( affineTransformA.linearTransform, affineTransformB.linearTransform );
	translation = affineTransformA.translation;
	affineTransformB.Transform( translation );
}

void AffineTransform::SetRigidBodyMotion( const Vector& unitAxis, double angle, const Vector& translation )
{
	linearTransform.SetRotation( unitAxis, angle );
	this->translation = translation;
}

bool AffineTransform::GetRigidBodyMotion( Vector& unitAxis, double& angle, Vector& translation ) const
{
	if( !linearTransform.GetRotation( unitAxis, angle ) )
		return false;

	translation = this->translation;
	return true;
}

// AffineTransform.cpp