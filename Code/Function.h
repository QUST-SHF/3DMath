// Function.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Function;
	class RealValuedFunctionOfRealVariable;
	class VectorValuedFunctionOfRealVariable;
	class ScalarField;
	class VectorField;
	class Vector;
	class Quadratic;
}

class _3DMATH_API _3DMath::Function
{
public:

	Function( void );
	virtual ~Function( void );

	typedef std::vector< double > RealArray;
	typedef std::vector< Vector > VectorArray;
};

class _3DMATH_API _3DMath::RealValuedFunctionOfRealVariable : public _3DMath::Function
{
public:

	RealValuedFunctionOfRealVariable( void );
	virtual ~RealValuedFunctionOfRealVariable( void );

	virtual bool Evaluate( double input, double& output ) const;
	virtual bool EvaluateDerivative( double input, double& output, double approxDelta = 1e-4 ) const;
	virtual RealValuedFunctionOfRealVariable* Differentiate( void ) const;
	virtual RealValuedFunctionOfRealVariable* AntiDifferentiate( void ) const;
	virtual bool FindZeros( RealArray& realArray ) const;
};

class _3DMATH_API _3DMath::ScalarField : public _3DMath::Function
{
public:

	ScalarField( void );
	virtual ~ScalarField( void );

	virtual bool Evaluate( const Vector& input, double& output ) const;
	virtual ScalarField* Differentiate( const Vector& direction ) const;
	virtual bool FindZeros( VectorArray& vectorArray ) const;
};

class _3DMATH_API _3DMath::VectorField : public _3DMath::Function
{
public:

	VectorField( void );
	virtual ~VectorField( void );

	virtual bool Evaluate( const Vector& input, Vector& output ) const;
	virtual bool EvaluateDirectionalDerivative( const Vector& input, const Vector& direction, Vector& output, double approxDelta = 1e-4 ) const;
	virtual VectorField* Differentiate( void ) const;
	virtual VectorField* Differentiate( const Vector& direction ) const;
	virtual bool FindZeros( VectorArray& vectorArray ) const;
};

class _3DMATH_API _3DMath::VectorValuedFunctionOfRealVariable : public _3DMath::Function
{
public:

	VectorValuedFunctionOfRealVariable( void );
	virtual ~VectorValuedFunctionOfRealVariable( void );

	virtual bool Evaluate( double input, Vector& output ) const;
	virtual bool EvaluateDerivative( double input, Vector& output, int order = 1, double approxDelta = 1e-4 ) const;
	virtual void CalcFrame( double input, Vector& tangent, Vector& normal, Vector& binormal ) const;
};

class _3DMATH_API _3DMath::Quadratic : public _3DMath::RealValuedFunctionOfRealVariable
{
public:

	Quadratic( void );
	virtual ~Quadratic( void );

	virtual bool Evaluate( double input, double& output ) const override;
	virtual bool EvaluateDerivative( double input, double& output, double approxDelta = 1e-4 ) const override;
	virtual bool FindZeros( RealArray& realArray ) const override;

	double A, B, C;
};

// Function.h