// Spline.h

#pragma once

#include "Function.h"
#include "Vector.h"

namespace _3DMath
{
	class Spline;
	class BezierSpline;
}

class _3DMath::Spline : public _3DMath::VectorValuedFunctionOfRealVariable
{
public:

	Spline( void );
	virtual ~Spline( void );

	virtual bool Evaluate( double input, Vector& output ) const override;
	virtual double CalcLength( double maxSegmentLength ) const;
	virtual void CalcSplinePolyline( double maxSegmentLength, VectorArray& pointArray ) const;

	VectorList controlPointList;

protected:

	void CalcSplinePolylineRecursively( double maxSegmentLength, double t0, double t1, const Vector& p0, const Vector& p1, VectorArray& pointArray, int maxDepth = 5, int depth = 1 ) const;
};

class _3DMath::BezierSpline : public _3DMath::Spline
{
public:

	BezierSpline( void );
	virtual ~BezierSpline( void );

	virtual bool Evaluate( double input, Vector& output ) const override;
};

// Spline.h