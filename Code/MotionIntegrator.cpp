// MotionIntegrator.cpp

#include "MotionIntegrator.h"
#include "Vector.h"

using namespace _3DMath;

//----------------------------------------------------------------------------
//                              MotionIntegrator
//----------------------------------------------------------------------------

MotionIntegrator::MotionIntegrator( void )
{
	dampingFactor = 0.99;
	fixedTimeStepSeconds = 0.005;
}

/*virtual*/ MotionIntegrator::~MotionIntegrator( void )
{
}

//----------------------------------------------------------------------------
//                            EulerMotionIntegrator
//----------------------------------------------------------------------------

EulerMotionIntegrator::EulerMotionIntegrator( void )
{
}

/*virtual*/ EulerMotionIntegrator::~EulerMotionIntegrator( void )
{
}

/*virtual*/ void EulerMotionIntegrator::Integrate( Vector& position, Vector& velocity, const Vector& acceleration, double deltaTimeSeconds )
{
	while( deltaTimeSeconds > 0.0 )
	{
		double timeStepSeconds = MIN( fixedTimeStepSeconds, deltaTimeSeconds );

		velocity.AddScale( acceleration, timeStepSeconds );
		position.AddScale( velocity, timeStepSeconds );
		velocity.Scale( dampingFactor );

		deltaTimeSeconds -= timeStepSeconds;
	}
}

//----------------------------------------------------------------------------
//                            VerletMotionIntegrator
//----------------------------------------------------------------------------

VerletMotionIntegrator::VerletMotionIntegrator( void )
{
}

/*virtual*/ VerletMotionIntegrator::~VerletMotionIntegrator( void )
{
}

/*virtual*/ void VerletMotionIntegrator::Integrate( Vector& position, Vector& velocity, const Vector& acceleration, double deltaTimeSeconds )
{
	Vector previousPosition = position;

	double timeStepSeconds = MIN( fixedTimeStepSeconds, deltaTimeSeconds );

	position.AddScale( velocity, timeStepSeconds );
	position.AddScale( acceleration, 0.5 * timeStepSeconds * timeStepSeconds );

	deltaTimeSeconds -= timeStepSeconds;

	double airFriction = 1.0 - dampingFactor;

	while( deltaTimeSeconds > 0.0 )
	{
		double timeStepSeconds = MIN( fixedTimeStepSeconds, deltaTimeSeconds );

		velocity.AddScale( acceleration, timeStepSeconds );
		position.AddScale( position, 2.0 - airFriction, previousPosition, -( 1.0 - airFriction ) );
		position.AddScale( acceleration, 0.5 * timeStepSeconds * timeStepSeconds );

		deltaTimeSeconds -= timeStepSeconds;
	}
}

// MotionIntegrator.cpp