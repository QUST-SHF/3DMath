// MotionIntegrator.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class MotionIntegrator;
	class EulerMotionIntegrator;
	class VerletMotionIntegrator;
	class Vector;
}

class _3DMATH_API _3DMath::MotionIntegrator
{
public:

	MotionIntegrator( void );
	virtual ~MotionIntegrator( void );

	virtual void Integrate( Vector& position, Vector& velocity, const Vector& acceleration, double deltaTimeSeconds ) = 0;

	double fixedTimeStepSeconds;
	double dampingFactor;
};

class _3DMATH_API _3DMath::EulerMotionIntegrator : public _3DMath::MotionIntegrator
{
public:

	EulerMotionIntegrator( void );
	virtual ~EulerMotionIntegrator( void );

	virtual void Integrate( Vector& position, Vector& velocity, const Vector& acceleration, double deltaTimeSeconds ) override;
};

class _3DMATH_API _3DMath::VerletMotionIntegrator : public _3DMath::MotionIntegrator
{
public:

	VerletMotionIntegrator( void );
	virtual ~VerletMotionIntegrator( void );

	virtual void Integrate( Vector& position, Vector& velocity, const Vector& acceleration, double deltaTimeSeconds ) override;
};

// MotionIntegrator.h