// AxisAlignedBox.cpp

#include "AxisAlignedBox.h"
#include "Triangle.h"
#include "LineSegment.h"

using namespace _3DMath;

AxisAlignedBox::AxisAlignedBox( void )
{
	negCorner.Set( 0.0, 0.0, 0.0 );
	posCorner.Set( 0.0, 0.0, 0.0 );
}

AxisAlignedBox::AxisAlignedBox( const AxisAlignedBox& box )
{
	negCorner = box.negCorner;
	posCorner = box.posCorner;
}

AxisAlignedBox::AxisAlignedBox( const Vector& negCorner, const Vector& posCorner )
{
	this->negCorner = negCorner;
	this->posCorner = posCorner;
}

AxisAlignedBox::~AxisAlignedBox( void )
{
}

void AxisAlignedBox::GrowToIncludePoint( const Vector& point )
{
	ExpandInterval( negCorner.x, posCorner.x, point.x );
	ExpandInterval( negCorner.y, posCorner.y, point.y );
	ExpandInterval( negCorner.z, posCorner.z, point.z );
}

/*static*/ void AxisAlignedBox::ExpandInterval( double& min, double& max, double value )
{
	if( value < min )
		min = value;
	if( value > max )
		max = value;
}

bool AxisAlignedBox::Intersect( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB )
{
	return false;
}

void AxisAlignedBox::Combine( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB )
{
	negCorner.Min( boxA.negCorner, boxB.negCorner );
	posCorner.Max( boxA.posCorner, boxA.posCorner );
}

void AxisAlignedBox::GetCenter( Vector& center ) const
{
	center.Lerp( negCorner, posCorner, 0.5 );
}

void AxisAlignedBox::SplitInTwo( AxisAlignedBox& boxA, AxisAlignedBox& boxB ) const
{
	Vector vector;
	vector.Subtract( posCorner, negCorner );

	if( vector.x > vector.y )
	{
		if( vector.x > vector.z )
		{
			//...
		}
		else
		{
		}
	}
	else
	{
	}
}

/*static*/ bool AxisAlignedBox::InInterval( double min, double max, double value )
{
	if( min <= value && value <= max )
		return true;
	return false;
}

bool AxisAlignedBox::ContainsPoint( const Vector& point ) const
{
	if( !InInterval( negCorner.x, posCorner.x, point.x ) )
		return false;
	if( !InInterval( negCorner.y, posCorner.y, point.y ) )
		return false;
	if( !InInterval( negCorner.z, posCorner.z, point.z ) )
		return false;
	return true;
}

bool AxisAlignedBox::ContainsTriangle( const Triangle& triangle ) const
{
	for( int i = 0; i < 3; i++ )
		if( !ContainsPoint( triangle.vertex[i] ) )
			return false;
	return true;
}

bool AxisAlignedBox::ContainsLineSegment( const LineSegment& lineSegment ) const
{
	for( int i = 0; i < 2; i++ )
		if( !ContainsPoint( lineSegment.vertex[i] ) )
			return false;
	return true;
}

// AxisAlignedBox.cpp