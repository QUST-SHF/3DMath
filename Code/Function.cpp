// Function.cpp

#include "Function.h"

using namespace _3DMath;

//-------------------------------------------------------------------------------------
//                                      Function
//-------------------------------------------------------------------------------------

Function::Function( void )
{
}

/*virtual*/ Function::~Function( void )
{
}

//-------------------------------------------------------------------------------------
//                            RealValuedFunctionOfRealVariable
//-------------------------------------------------------------------------------------

RealValuedFunctionOfRealVariable::RealValuedFunctionOfRealVariable( void )
{
}

/*virtual*/ RealValuedFunctionOfRealVariable::~RealValuedFunctionOfRealVariable( void )
{
}

/*virtual*/ bool RealValuedFunctionOfRealVariable::Evaluate( double input, double& output ) const
{
	return false;
}

/*virtual*/ bool RealValuedFunctionOfRealVariable::EvaluateDerivative( double input, double& output, double approxDelta /*= 1e-4*/ ) const
{
	double outputA, outputB;

	if( !Evaluate( input + approxDelta, outputA ) )
		return false;

	if( !Evaluate( input - approxDelta, outputB ) )
		return false;

	output = ( outputA - outputB ) / ( 2.0 * approxDelta );
	return true;
}

/*virtual*/ RealValuedFunctionOfRealVariable* RealValuedFunctionOfRealVariable::Differentiate( void ) const
{
	return nullptr;
}

/*virtual*/ RealValuedFunctionOfRealVariable* RealValuedFunctionOfRealVariable::AntiDifferentiate( void ) const
{
	return nullptr;
}

/*virtual*/ bool RealValuedFunctionOfRealVariable::FindZeros( RealArray& realArray ) const
{
	// TODO: Can we provide some sort of default implementation that finds zeros using a calculus-based method?
	//       For example, if the function is continuous on a compact interval, and we find a value above zero,
	//       and one below, then by the intermediate value theorem, there must be a zero between them.  We could
	//       do a binary search for it.  Should we be given an initial search interval?
	return false;
}

//-------------------------------------------------------------------------------------
//                                     ScalarField
//-------------------------------------------------------------------------------------

ScalarField::ScalarField( void )
{
}

/*virtual*/ ScalarField::~ScalarField( void )
{
}

/*virtual*/ bool ScalarField::Evaluate( const Vector& input, double& output ) const
{
	return false;
}

/*virtual*/ ScalarField* ScalarField::Differentiate( const Vector& direction ) const
{
	return nullptr;
}

/*virtual*/ bool ScalarField::FindZeros( VectorArray& vectorArray ) const
{
	return false;
}

//-------------------------------------------------------------------------------------
//                                     VectorField
//-------------------------------------------------------------------------------------

VectorField::VectorField( void )
{
}

/*virtual*/ VectorField::~VectorField( void )
{
}

/*virtual*/ bool VectorField::Evaluate( const Vector& input, Vector& output ) const
{
	return false;
}

/*virtual*/ bool VectorField::EvaluateDirectionalDerivative( const Vector& input, const Vector& direction, Vector& output, double approxDelta /*= 1e-4*/ ) const
{
	Vector inputA, inputB;
	inputA.AddScale( input, direction, approxDelta );
	inputB.AddScale( input, direction, -approxDelta );

	Vector outputA, outputB;

	if( !Evaluate( inputA, outputA ) )
		return false;

	if( !Evaluate( inputB, outputB ) )
		return false;

	output.Subtract( outputA, outputB );
	output.Scale( 1.0 / ( 2.0 * approxDelta ) );
	return true;
}

/*virtual*/ VectorField* VectorField::Differentiate( void ) const
{
	return nullptr;
}

/*virtual*/ VectorField* VectorField::Differentiate( const Vector& direction ) const
{
	return nullptr;
}

/*virtual*/ bool VectorField::FindZeros( VectorArray& vectorArray ) const
{
	return false;
}

//-------------------------------------------------------------------------------------
//                         VectorValuedFunctionOfRealVariable
//-------------------------------------------------------------------------------------

VectorValuedFunctionOfRealVariable::VectorValuedFunctionOfRealVariable( void )
{
}

/*virtual*/ VectorValuedFunctionOfRealVariable::~VectorValuedFunctionOfRealVariable( void )
{
}

/*virtual*/ bool VectorValuedFunctionOfRealVariable::Evaluate( double input, Vector& output ) const
{
	return false;
}

// Innaccurracy here is going to grow with the order of the derivative desired.
/*virtual*/ bool VectorValuedFunctionOfRealVariable::EvaluateDerivative( double input, Vector& output, int order /*= 1*/, double approxDelta /*= 1e-4*/ ) const
{
	if( order == 0 )
		return Evaluate( input, output );
	
	Vector outputA, outputB;

	if( !EvaluateDerivative( input + approxDelta, outputA, order - 1, approxDelta ) )
		return false;

	if( !EvaluateDerivative( input - approxDelta, outputB, order - 1, approxDelta ) )
		return false;

	output = ( outputA - outputB ) * ( 2.0 * approxDelta );
	return true;
}

/*virtual*/ void VectorValuedFunctionOfRealVariable::CalcFrame( double input, Vector& tangent, Vector& normal, Vector& binormal ) const
{
	EvaluateDerivative( input, tangent );
	EvaluateDerivative( input, normal );

	tangent.Normalize();
	normal.Normalize();

	binormal.Cross( tangent, normal );
}

//-------------------------------------------------------------------------------------
//                                     Quadratic
//-------------------------------------------------------------------------------------

Quadratic::Quadratic( void )
{
	A = 1.0;
	B = 0.0;
	C = 0.0;
}

/*virtual*/ Quadratic::~Quadratic( void )
{
}

/*virtual*/ bool Quadratic::Evaluate( double input, double& output ) const
{
	output = A * input * input + B * input + C;
	return true;
}

/*virtual*/ bool Quadratic::EvaluateDerivative( double input, double& output, double approxDelta /*= 1e-4*/ ) const
{
	output = 2.0 * A * input + B;
	return true;
}

/*virtual*/ bool Quadratic::FindZeros( RealArray& realArray ) const
{
	realArray.clear();

	double descriminant = B * B - 4.0 * A * C;
	if( descriminant == 0.0 )
		realArray.push_back( -B / ( 2.0 * A ) );
	else if( descriminant > 0.0 )
	{
		realArray.push_back( ( -B + sqrt( descriminant ) ) / ( 2.0 * A ) );
		realArray.push_back( ( -B - sqrt( descriminant ) ) / ( 2.0 * A ) );
	}

	return true;
}

// Function.cpp