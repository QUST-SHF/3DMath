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
	class BoundingBoxTree;
	class AxisAlignedBox;
	class TimeKeeper;
}

class _3DMATH_API _3DMath::ParticleSystem
{
public:

	ParticleSystem( void );
	virtual ~ParticleSystem( void );

	class _3DMATH_API Particle : public Object
	{
	public:

		Particle( void );
		virtual ~Particle( void );

		virtual void GetPosition( Vector& position ) const = 0;
		virtual void SetPosition( const Vector& position ) = 0;

		virtual void Integrate( const _3DMath::TimeKeeper& timeKeeper, double damping = 0.0 );

		Vector velocity;
		Vector acceleration;
		Vector netForce;
		Vector previousPosition;
		double mass;
		double timeOfDeath;
	};

	class _3DMATH_API MeshVertexParticle : public Particle
	{
	public:

		MeshVertexParticle( void );
		virtual ~MeshVertexParticle( void );

		virtual void GetPosition( Vector& position ) const override;
		virtual void SetPosition( const Vector& position ) override;

		TriangleMesh* mesh;
		int index;
	};

	// TODO: We'll need to tell the particle system where the camera is and how it's oriented.
	//class _3DMATH_API BillboardParticle : public Particle
	//{
	//public:
	//};

	class _3DMATH_API Force : public Object
	{
	public:

		Force( ParticleSystem* system );
		virtual ~Force( void );

		virtual void Apply( void );
		virtual void Apply( Particle* particle );

		ParticleSystem* system;
		bool enabled;
		bool transient;
	};

	class _3DMATH_API GenericForce : public Force
	{
	public:

		GenericForce( ParticleSystem* system );
		virtual ~GenericForce( void );

		virtual void Apply( Particle* particle ) override;

		Vector force;
	};

	class _3DMATH_API WindForce : public Force
	{
	public:

		WindForce( ParticleSystem* system );
		virtual ~WindForce( void );

		virtual void Apply( Particle* particle ) override;

		Vector generalUnitDir;
		double coneAngle;
		double minStrength, maxStrength;
	};

	class _3DMATH_API ResistanceForce : public Force
	{
	public:

		ResistanceForce( ParticleSystem* system );
		virtual ~ResistanceForce( void );

		virtual void Apply( Particle* particle ) override;

		double resistance;
	};

	class _3DMATH_API GravityForce : public Force
	{
	public:

		GravityForce( ParticleSystem* system );
		virtual ~GravityForce( void );

		virtual void Apply( Particle* particle ) override;

		Vector accelDueToGravity;
	};

	class _3DMATH_API TorqueForce : public Force
	{
	public:

		TorqueForce( ParticleSystem* system );
		virtual ~TorqueForce( void );

		virtual void Apply( Particle* particle ) override;

		Vector torque;
	};

	class _3DMATH_API SpringForce : public Force
	{
	public:

		SpringForce( ParticleSystem* system );
		virtual ~SpringForce( void );

		virtual void Render( Renderer* renderer ) const override;
		virtual void Apply( void ) override;

		int endPointParticleIds[2];
		double equilibriumLength;
		double stiffness;
	};

	struct _3DMATH_API ImpactInfo
	{
		LineSegment lineOfMotion;
		Vector netForceAtImpact;
		Vector contactUnitNormal;
		Vector contactPosition;
		double friction;
	};

	class _3DMATH_API FrictionForce : public Force
	{
	public:

		FrictionForce( ParticleSystem* system );
		virtual ~FrictionForce( void );

		virtual void Apply( void ) override;

		int particleId;
		ImpactInfo impactInfo;
	};

	class _3DMATH_API CollisionObject : public Object
	{
	public:

		CollisionObject( void );
		virtual ~CollisionObject( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) = 0;
	};

	class _3DMATH_API CollisionPlane : public CollisionObject
	{
	public:

		CollisionPlane( void );
		virtual ~CollisionPlane( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) override;

		Plane plane;
		double friction;
	};

	class _3DMATH_API TriangleMeshCollisionObject : public CollisionObject
	{
	public:

		TriangleMeshCollisionObject( void );
		virtual ~TriangleMeshCollisionObject( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) override;

		AxisAlignedBox* boundingBox;
		TriangleMesh* mesh;
	};

	class _3DMATH_API BoundingBoxTreeCollisionObject : public CollisionObject
	{
	public:

		BoundingBoxTreeCollisionObject( void );
		virtual ~BoundingBoxTreeCollisionObject( void );

		virtual bool ResolveCollision( ImpactInfo& impactInfo ) override;

		BoundingBoxTree* boxTree;
		double friction;
	};

	class _3DMATH_API Emitter : public Object
	{
	public:

		Emitter( void );
		virtual ~Emitter( void );

		virtual void EmitParticles( ParticleSystem* system, double currentTime ) = 0;
	};

	void Clear( void );
	void Simulate( const TimeKeeper& timeKeeper );

	ObjectCollection particleCollection;
	ObjectCollection forceCollection;
	ObjectCollection collisionObjectCollection;
	ObjectCollection emitterCollection;

	Vector centerOfMass;
	Random random;

private:

	void CullDeadParticles( const _3DMath::TimeKeeper& timeKeeper );
	void ResetParticlePhysics( void );
	void AccumulateForces( void );
	void IntegrateParticles( const _3DMath::TimeKeeper& timeKeeper );
	void ResolveCollisions( void );
	void CalculateCenterOfMass( void );
};

// ParticleSystem.h
