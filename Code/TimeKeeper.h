// TimeKeeper.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class TimeKeeper;
}

class _3DMATH_API _3DMath::TimeKeeper
{
public:

	TimeKeeper( void );
	virtual ~TimeKeeper( void );

	virtual void MarkCurrentTime( void );
	virtual double AskSystemForCurrentTimeMilliseconds( void );

	double GetCurrentTimeMilliseconds( void ) const;
	double GetCurrentTimeSeconds( void ) const;
	double GetDeltaTimeMilliseconds( void ) const;
	double GetDeltaTimeSeconds( void ) const;

	mutable double fixedDeltaTimeMilliseconds;

protected:

	double currentTimeMilliseconds;
	double lastTimeMilliseconds;
	double deltaTimeMilliseconds;
	double baseTimeMilliseconds;
};

// TimeKeeper.h