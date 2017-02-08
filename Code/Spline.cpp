// Spline.cpp

#include "Spline.h"

using namespace _3DMath;

//------------------------------------------------------------------------------------
//                                      Spline
//------------------------------------------------------------------------------------

Spline::Spline( void )
{
}

/*virtual*/ Spline::~Spline( void )
{
}

/*virtual*/ bool Spline::Evaluate( double input, Vector& output ) const
{
	return false;
}

/*virtual*/ double Spline::CalcLength( double maxSegmentLength ) const
{
	double totalLength = 0.0;
	VectorArray pointArray;
	CalcSplinePolyline( maxSegmentLength, pointArray );
	for( int i = 0; i < ( signed )pointArray.size(); i++ )
		totalLength += pointArray[i].Distance( pointArray[ i + 1 ] );
	return totalLength;
}

/*virtual*/ void Spline::CalcSplinePolyline( double maxSegmentLength, VectorArray& pointArray ) const
{
	Vector p0, p1;
	Evaluate( 0.0, p0 );
	Evaluate( 1.0, p1 );

	CalcSplinePolylineRecursively( maxSegmentLength, 0.0, 1.0, p0, p1, pointArray );
	pointArray.push_back( p1 );
}

void Spline::CalcSplinePolylineRecursively( double maxSegmentLength, double t0, double t1, const Vector& p0, const Vector& p1, VectorArray& pointArray, int maxDepth /*= 5*/, int depth /*= 1*/ ) const
{
	if( ( p0 - p1 ).Length() <= maxSegmentLength || depth >= maxDepth )
		pointArray.push_back( p0 );
	else
	{
		double t = ( t0 + t1 ) / 2.0;
		Vector p;
		Evaluate( t, p );
		CalcSplinePolylineRecursively( maxSegmentLength, t0, t, p0, p, pointArray, maxDepth, depth + 1 );
		CalcSplinePolylineRecursively( maxSegmentLength, t, t1, p, p1, pointArray, maxDepth, depth + 1 );
	}
}

//------------------------------------------------------------------------------------
//                                   BezierSpline
//------------------------------------------------------------------------------------

BezierSpline::BezierSpline( void )
{
}

/*virtual*/ BezierSpline::~BezierSpline( void )
{
}

/*virtual*/ bool BezierSpline::Evaluate( double input, Vector& output ) const
{
	if( controlPointList.size() < 2 )
		return false;

	VectorArray pointArray;
	for( VectorList::const_iterator iter = controlPointList.cbegin(); iter != controlPointList.cend(); iter++ )
		pointArray.push_back( *iter );

	for( int i = 1; i < ( signed )pointArray.size(); i++ )
		for( int j = 0; j < ( signed )pointArray.size() - i; j++ )
			pointArray[j].Lerp( pointArray[j], pointArray[ j + 1 ], input );
	
	output = pointArray[0];
	return true;
}

// Spline.cpp