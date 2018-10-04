// ParticleSystem.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Plane.h"
#include "Random.h"
#include "LineSegment.h"
#include "HandleObject.h"

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

	class _3DMATH_API Particle : public HandleObject	// This is a bit expensive, but I'm going to see if I can get away with it.
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
		double friction;
	};

	class _3DMATH_API GenericParticle : public _3DMath::ParticleSystem::Particle
	{
	public:

		GenericParticle( const Vector* position = nullptr );
		virtual ~GenericParticle( void );

		virtual void GetPosition( Vector& position ) const override;
		virtual void SetPosition( const Vector& position ) override;

		_3DMath::Vector position;
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

	class _3DMATH_API Force : public HandleObject
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

		virtual void Render( Renderer& renderer ) const override;
		virtual void Apply( void ) override;

		void ResetEquilibriumLength( void );

		int endPointParticleHandles[2];
		double equilibriumLength;
		double stiffness;
	};

	class _3DMATH_API FrictionForce : public Force
	{
	public:

		FrictionForce( ParticleSystem* system );
		virtual ~FrictionForce( void );

		virtual void Apply( void ) override;

		int particleHandle;
		Vector netForceAtImpact;
		Vector contactUnitNormal;
		double friction;
	};

	class _3DMATH_API CollisionObject : public HandleObject
	{
	public:

		CollisionObject( void );
		virtual ~CollisionObject( void );

		virtual bool ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal ) = 0;

		double friction;
	};

	class _3DMATH_API CollisionPlane : public CollisionObject
	{
	public:

		CollisionPlane( void );
		virtual ~CollisionPlane( void );

		virtual bool ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal ) override;

		Plane plane;
	};

	class _3DMATH_API ConvexTriangleMeshCollisionObject : public CollisionObject
	{
	public:

		ConvexTriangleMeshCollisionObject( void );
		virtual ~ConvexTriangleMeshCollisionObject( void );

		virtual bool ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal ) override;

		AxisAlignedBox* boundingBox;	// It is up to the user to keep this bounding box in sync with the mesh.
		TriangleMesh* mesh;	// We assume the mesh forms a convex shape; if it doesn't, the behavior is left undefined.
	};

	class _3DMATH_API BoundingBoxTreeCollisionObject : public CollisionObject
	{
	public:

		BoundingBoxTreeCollisionObject( void );
		virtual ~BoundingBoxTreeCollisionObject( void );

		virtual bool ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal ) override;

		BoundingBoxTree* boxTree;
		double detectionDistance;
	};

	class _3DMATH_API Emitter : public HandleObject
	{
	public:

		Emitter( void );
		virtual ~Emitter( void );

		virtual void EmitParticles( ParticleSystem* system, double currentTime ) = 0;
	};

	void Clear( void );
	void Simulate( const TimeKeeper& timeKeeper );
	void ResetMotion( void );

	typedef std::list< Particle* > ParticleList;
	typedef std::list< Force* > ForceList;
	typedef std::list< CollisionObject* > CollisionObjectList;
	typedef std::list< Emitter* > EmitterList;

	ParticleList* particleList;
	ForceList* forceList;
	CollisionObjectList* collisionObjectList;
	EmitterList* emitterList;

	double damping;
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
