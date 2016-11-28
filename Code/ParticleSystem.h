// ParticleSystem.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Plane.h"
#include "ObjectCollection.h"
#include "Random.h"
#include "LineSegment.h"

namespace _3DMath
{
	class ParticleSystem;
	class LineSegment;
	class TriangleMesh;
}

class _3DMATH_API _3DMath::ParticleSystem
{
public:

	ParticleSystem( void );
	virtual ~ParticleSystem( void );

	class Particle : public Object
	{
	public:

		Particle( void );
		virtual ~Particle( void );

		virtual void GetPosition( Vector& position ) const = 0;
		virtual void SetPosition( const Vector& position ) = 0;

		virtual void Integrate( double deltaTime );

		Vector velocity;
		Vector acceleration;
		Vector netForce;
		Vector previousPosition;
		double mass;
		double timeOfDeath;
	};

	class MeshVertexParticle : public Particle
	{
	public:

		MeshVertexParticle( void );
		virtual ~MeshVertexParticle( void );

		virtual void GetPosition( Vector& position ) const override;
		virtual void SetPosition( const Vector& position ) override;

		TriangleMesh* mesh;
		int index;
	};

	class Force : public Object
	{
	public:

		Force( void );
		virtual ~Force( void );

		virtual void Apply( ParticleSystem* system );
		virtual void Apply( ParticleSystem* system, Particle* particle );

		bool enabled;
		bool transient;
	};

	class GenericForce : public Force
	{
	public:

		GenericForce( void );
		virtual ~GenericForce( void );

		virtual void Apply( ParticleSystem* system, Particle* particle ) override;

		Vector force;
	};

	class WindForce : public Force
	{
	public:

		WindForce( void );
		virtual ~WindForce( void );

		virtual void Apply( ParticleSystem* system, Particle* particle ) override;

		Vector generalUnitDir;
		double coneAngle;
		double minStrength, maxStrength;
	};

	class GravityForce : public Force
	{
	public:

		GravityForce( void );
		virtual ~GravityForce( void );

		virtual void Apply( ParticleSystem* system, Particle* particle ) override;

		Vector accelDueToGravity;
	};

	class TorqueForce : public Force
	{
	public:

		TorqueForce( void );
		virtual ~TorqueForce( void );

		virtual void Apply( ParticleSystem* system, Particle* particle ) override;

		Vector torque;
	};

	class SpringForce : public Force
	{
	public:

		SpringForce( void );
		virtual ~SpringForce( void );

		virtual void Apply( ParticleSystem* system ) override;

		int endPointParticleIds[2];
		double equilibriumLength;
		double stiffness;
	};

	struct ImpactInfo
	{
		LineSegment lineOfMotion;
		Vector netForceAtImpact;
		Vector contactUnitNormal;
		Vector contactPosition;
		double friction;
	};

	class FrictionForce : public Force
	{
	public:

		FrictionForce( void );
		virtual ~FrictionForce( void );

		virtual void Apply( ParticleSystem* system ) override;

		int particleId;
		ImpactInfo impactInfo;
	};

	class CollisionObject : public Object
	{
	public:

		CollisionObject( void );
		virtual ~CollisionObject( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) = 0;
	};

	class CollisionPlane : public CollisionObject
	{
	public:

		CollisionPlane( void );
		virtual ~CollisionPlane( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) override;

		Plane plane;
	};

	class TriangleMeshCollisionObject : public CollisionObject
	{
	public:

		TriangleMeshCollisionObject( void );
		virtual ~TriangleMeshCollisionObject( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) override;

		TriangleMesh* mesh;
	};

	void Simulate( double currentTime );

	ObjectCollection particleCollection;
	ObjectCollection forceCollection;
	ObjectCollection collisionObjectCollection;

	Vector centerOfMass;
	double previousTime;
	Random random;

private:

	void CullDeadParticles( double currentTime );
	void ResetParticlePhysics( void );
	void AccumulateForces( void );
	void IntegrateParticles( double currentTime );
	void ResolveCollisions( void );
	void CalculateCenterOfMass( void );
};

// ParticleSystem.h
