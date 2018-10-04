// ParticleSystem.cpp

#include "ParticleSystem.h"
#include "TriangleMesh.h"
#include "AxisAlignedBox.h"
#include "BoundingBoxTree.h"
#include "TimeKeeper.h"
#include "ListFunctions.h"

using namespace _3DMath;

//-------------------------------------------------------------------------------------------------
//                                        ParticleSystem
//-------------------------------------------------------------------------------------------------

ParticleSystem::ParticleSystem( void )
{
	damping = 0.01;

	centerOfMass.Set( 0.0, 0.0, 0.0 );

	particleList = new ParticleList;
	forceList = new ForceList;
	collisionObjectList = new CollisionObjectList;
	emitterList = new EmitterList;
}

/*virtual*/ ParticleSystem::~ParticleSystem( void )
{
	Clear();

	delete particleList;
	delete forceList;
	delete collisionObjectList;
	delete emitterList;
}

void ParticleSystem::Clear( void )
{
	centerOfMass.Set( 0.0, 0.0, 0.0 );

	FreeList< Particle >( *particleList );
	FreeList< Force >( *forceList );
	FreeList< CollisionObject >( *collisionObjectList );
	FreeList< Emitter >( *emitterList );
}

void ParticleSystem::Simulate( const _3DMath::TimeKeeper& timeKeeper )
{
	CullDeadParticles( timeKeeper );
	ResetParticlePhysics();
	CalculateCenterOfMass();
	AccumulateForces();
	IntegrateParticles( timeKeeper );
	ResolveCollisions();
}

void ParticleSystem::CullDeadParticles( const _3DMath::TimeKeeper& timeKeeper )
{
	double currentTime = timeKeeper.GetCurrentTimeSeconds();

	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		ParticleList::iterator nextIter = iter;
		nextIter++;

		Particle* particle = ( Particle* )*iter;
		if( particle->timeOfDeath != 0.0 && particle->timeOfDeath <= currentTime )
		{
			particleList->erase( iter );
			delete particle;
		}

		iter = nextIter;
	}
}

void ParticleSystem::ResetParticlePhysics( void )
{
	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;
		particle->netForce.Set( 0.0, 0.0, 0.0 );
		iter++;
	}
}

void ParticleSystem::AccumulateForces( void )
{
	ForceList::iterator iter = forceList->begin();
	while( iter != forceList->end() )
	{
		ForceList::iterator nextIter = iter;
		nextIter++;
		
		Force* force = ( Force* )*iter;
		force->Apply();

		if( force->transient )
		{
			delete force;
			forceList->erase( iter );
		}

		iter = nextIter;
	}
}

void ParticleSystem::ResetMotion( void )
{
	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;
		Vector position;
		particle->GetPosition( position );
		particle->previousPosition = position;
		particle->velocity.Set( 0.0, 0.0, 0.0 );
		particle->acceleration.Set( 0.0, 0.0, 0.0 );
		particle->netForce.Set( 0.0, 0.0, 0.0 );
		iter++;
	}

	// Should we remove certain forces here too?
	// We can't remove them all; some were added by the user.
	// We should maybe delete all friction and torque forces.
}

void ParticleSystem::IntegrateParticles( const _3DMath::TimeKeeper& timeKeeper )
{
	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;
		particle->Integrate( timeKeeper, damping );
		iter++;
	}
}

void ParticleSystem::ResolveCollisions( void )
{
	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;

		LineSegment lineOfMotion;
		lineOfMotion.vertex[0] = particle->previousPosition;
		particle->GetPosition( lineOfMotion.vertex[1] );

		CollisionObjectList::iterator collisionIter = collisionObjectList->begin();
		while( collisionIter != collisionObjectList->end() )
		{
			CollisionObject* collisionObject = ( CollisionObject* )*collisionIter;

			Vector contactPosition, contactUnitNormal;
			if( collisionObject->ResolveCollision( lineOfMotion, contactPosition, contactUnitNormal ) )
			{
				particle->SetPosition( contactPosition );

				double friction = collisionObject->friction * particle->friction;
				if( friction != 0.0 )
				{
					FrictionForce* frictionForce = new FrictionForce( this );
					frictionForce->netForceAtImpact = particle->netForce;
					frictionForce->particleHandle = particle->GetHandle();
					frictionForce->contactUnitNormal = contactUnitNormal;
					frictionForce->friction = friction;
					forceList->push_back( frictionForce );
				}
			}

			collisionIter++;
		}

		iter++;
	}
}

void ParticleSystem::CalculateCenterOfMass( void )
{
	double totalMass = 0.0;
	Vector totalMoments( 0.0, 0.0, 0.0 );

	ParticleList::iterator iter = particleList->begin();
	while( iter != particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;

		totalMass += particle->mass;

		Vector moment;
		particle->GetPosition( moment );
		moment.Scale( particle->mass );

		totalMoments.Add( moment );

		iter++;
	}

	centerOfMass.SetScaled( totalMoments, 1.0 / totalMass );
}

//-------------------------------------------------------------------------------------------------
//                                          Particle
//-------------------------------------------------------------------------------------------------

ParticleSystem::Particle::Particle( void )
{
	velocity.Set( 0.0, 0.0, 0.0 );
	acceleration.Set( 0.0, 0.0, 0.0 );
	netForce.Set( 0.0, 0.0, 0.0 );
	previousPosition.Set( 0.0, 0.0, 0.0 );
	mass = 1.0;
	timeOfDeath = 0.0;
	friction = 1.0;
}

/*virtual*/ ParticleSystem::Particle::~Particle( void )
{
}

/*virtual*/ void ParticleSystem::Particle::Integrate( const _3DMath::TimeKeeper& timeKeeper, double damping /*= 0.0*/ )
{
	double deltaTime = timeKeeper.GetDeltaTimeSeconds();

	acceleration.SetScaled( netForce, 1.0 / mass );

	Vector position;
	GetPosition( position );

	velocity.Subtract( position, previousPosition );
	velocity.Scale( 1.0 / deltaTime );

	// This is the Verlet method.
	Vector nextPosition;
	nextPosition.AddScale( position, 2.0 - damping, previousPosition, damping - 1.0 );
	nextPosition.AddScale( acceleration, deltaTime * deltaTime );

	SetPosition( nextPosition );

	previousPosition = position;
}

//-------------------------------------------------------------------------------------------------
//                                         GenericParticle
//-------------------------------------------------------------------------------------------------

ParticleSystem::GenericParticle::GenericParticle( const Vector* position /*= nullptr*/ )
{
	if( position )
		this->position = *position;
	else
		this->position.Set( 0.0, 0.0, 0.0 );

	previousPosition = this->position;
}

/*virtual*/ ParticleSystem::GenericParticle::~GenericParticle( void )
{
}

/*virtual*/ void ParticleSystem::GenericParticle::GetPosition( Vector& position ) const
{
	position = this->position;
}

/*virtual*/ void ParticleSystem::GenericParticle::SetPosition( const Vector& position )
{
	this->position = position;
}

//-------------------------------------------------------------------------------------------------
//                                         MeshVertexParticle
//-------------------------------------------------------------------------------------------------

ParticleSystem::MeshVertexParticle::MeshVertexParticle( void )
{
	mesh = nullptr;
	index = 0;
}

/*virtual*/ ParticleSystem::MeshVertexParticle::~MeshVertexParticle( void )
{
}

/*virtual*/ void ParticleSystem::MeshVertexParticle::GetPosition( Vector& position ) const
{
	if( !mesh )
		position.Set( 0.0, 0.0, 0.0 );
	else
		mesh->GetVertexPosition( index, position );
}

/*virtual*/ void ParticleSystem::MeshVertexParticle::SetPosition( const Vector& position )
{
	if( mesh )
		mesh->SetVertexPosition( index, position );
}

//-------------------------------------------------------------------------------------------------
//                                            Force
//-------------------------------------------------------------------------------------------------

ParticleSystem::Force::Force( ParticleSystem* system )
{
	this->system = system;
	enabled = true;
	transient = false;
}

/*virtual*/ ParticleSystem::Force::~Force( void )
{
}

/*virtual*/ void ParticleSystem::Force::Apply( void )
{
	ParticleList::iterator iter = system->particleList->begin();
	while( iter != system->particleList->end() )
	{
		Particle* particle = ( Particle* )*iter;
		Apply( particle );
		iter++;
	}
}

/*virtual*/ void ParticleSystem::Force::Apply( Particle* particle )
{
}

//-------------------------------------------------------------------------------------------------
//                                           GenericForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::GenericForce::GenericForce( ParticleSystem* system ) : Force( system )
{
	force.Set( 0.0, 0.0, 0.0 );
}

/*virtual*/ ParticleSystem::GenericForce::~GenericForce( void )
{
}

/*virtual*/ void ParticleSystem::GenericForce::Apply( Particle* particle )
{
	particle->netForce.Add( force );
}

//-------------------------------------------------------------------------------------------------
//                                            WindForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::WindForce::WindForce( ParticleSystem* system ) : Force( system )
{
	generalUnitDir.Set( 0.0, 0.0, 1.0 );
	coneAngle = M_PI / 8.0;
	minStrength = 0.0;
	maxStrength = 0.0;
}

/*virtual*/ ParticleSystem::WindForce::~WindForce( void )
{
}

/*virtual*/ void ParticleSystem::WindForce::Apply( Particle* particle )
{
	Vector windForce;
	system->random.VectorInCone( generalUnitDir, coneAngle, windForce );
	windForce.Scale( system->random.Float( minStrength, maxStrength ) );
	particle->netForce.Add( windForce );
}

//-------------------------------------------------------------------------------------------------
//                                          ResistanceForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::ResistanceForce::ResistanceForce( ParticleSystem* system ) : Force( system )
{
	resistance = 0.5;
}

/*virtual*/ ParticleSystem::ResistanceForce::~ResistanceForce( void )
{
}

/*virtual*/ void ParticleSystem::ResistanceForce::Apply( Particle* particle )
{
	Vector resistanceForce;
	resistanceForce.SetScaled( particle->velocity, -resistance );
	particle->netForce.Add( resistanceForce );
}

//-------------------------------------------------------------------------------------------------
//                                            GravityForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::GravityForce::GravityForce( ParticleSystem* system ) : Force( system )
{
	accelDueToGravity.Set( 0.0, -1.0, 0.0 );
}

/*virtual*/ ParticleSystem::GravityForce::~GravityForce( void )
{
}

/*virtual*/ void ParticleSystem::GravityForce::Apply( Particle* particle )
{
	Vector gravityForce;
	gravityForce.SetScaled( accelDueToGravity, particle->mass );
	particle->netForce.Add( gravityForce );
}

//-------------------------------------------------------------------------------------------------
//                                            TorqueForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::TorqueForce::TorqueForce( ParticleSystem* system ) : Force( system )
{
	torque.Set( 0.0, 0.0, 0.0 );
}

/*virtual*/ ParticleSystem::TorqueForce::~TorqueForce( void )
{
}

/*virtual*/ void ParticleSystem::TorqueForce::Apply( Particle* particle )
{
	Vector position;
	particle->GetPosition( position );

	Vector vector;
	vector.Subtract( position, system->centerOfMass );

	// TODO: Look this up in a textbook to verify its correctness.
	Vector torqueForce;
	torqueForce.Cross( torque, vector );
	torqueForce.Scale( 1.0 / vector.Dot( vector ) );

	particle->netForce.Add( torqueForce );
}

//-------------------------------------------------------------------------------------------------
//                                            SpringForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::SpringForce::SpringForce( ParticleSystem* system ) : Force( system )
{
	endPointParticleHandles[0] = 0;
	endPointParticleHandles[1] = 0;
	equilibriumLength = 1.0;
	stiffness = 1.0;
}

/*virtual*/ ParticleSystem::SpringForce::~SpringForce( void )
{
}

/*virtual*/ void ParticleSystem::SpringForce::Apply( void )
{
	Particle* particleA = ( Particle* )HandleObject::Dereference( endPointParticleHandles[0] );
	Particle* particleB = ( Particle* )HandleObject::Dereference( endPointParticleHandles[1] );

	if( particleA && particleB )
	{
		Vector positionA, positionB;

		particleA->GetPosition( positionA );
		particleB->GetPosition( positionB );

		Vector vector;
		vector.Subtract( positionB, positionA );

		double length = vector.Length();

		// TODO: Look this up in a textbook to verify its correctness.
		Vector springForce;
		springForce.SetScaled( vector, stiffness * ( length - equilibriumLength ) );

		particleA->netForce.Add( springForce );
		particleB->netForce.Subtract( springForce );
	}
}

/*virtual*/ void ParticleSystem::SpringForce::Render( Renderer& renderer ) const
{
	Particle* particleA = ( Particle* )HandleObject::Dereference( endPointParticleHandles[0] );
	Particle* particleB = ( Particle* )HandleObject::Dereference( endPointParticleHandles[1] );

	if( particleA && particleB )
	{
		Vector positionA, positionB;

		particleA->GetPosition( positionA );
		particleB->GetPosition( positionB );

		renderer.BeginDrawMode( Renderer::DRAW_MODE_LINES );

		renderer.IssueVertex( Vertex( positionA ) );
		renderer.IssueVertex( Vertex( positionB ) );

		renderer.EndDrawMode();
	}
}

void ParticleSystem::SpringForce::ResetEquilibriumLength( void )
{
	Particle* particleA = ( Particle* )HandleObject::Dereference( endPointParticleHandles[0] );
	Particle* particleB = ( Particle* )HandleObject::Dereference( endPointParticleHandles[1] );

	if( particleA && particleB )
	{
		Vector positionA, positionB;

		particleA->GetPosition( positionA );
		particleB->GetPosition( positionB );

		Vector vector;
		vector.Subtract( positionA, positionB );

		equilibriumLength = vector.Length();
	}
}

//-------------------------------------------------------------------------------------------------
//                                            FrictionForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::FrictionForce::FrictionForce( ParticleSystem* system ) : Force( system )
{
	particleHandle = 0;
	contactUnitNormal.Set( 0.0, 0.0, 0.0 );
	netForceAtImpact.Set( 0.0, 0.0, 0.0 );
	friction = 0.0;
	transient = true;
}

/*virtual*/ ParticleSystem::FrictionForce::~FrictionForce( void )
{
}

/*virtual*/ void ParticleSystem::FrictionForce::Apply( void )
{
	Particle* particle = ( Particle* )HandleObject::Dereference( particleHandle );
	if( particle )
	{
		// TODO: Get out the physics book and check this math.

		double normalForce = contactUnitNormal.Dot( netForceAtImpact );
		if( normalForce <= 0.0 )
		{
			Vector position;
			particle->GetPosition( position );

			Vector frictionForce;
			frictionForce.Subtract( particle->previousPosition, position );
			frictionForce.Normalize();
			frictionForce.Scale( -friction * normalForce );

			particle->netForce.Add( frictionForce );
		}
	}
}

//-------------------------------------------------------------------------------------------------
//                                            CollisionObject
//-------------------------------------------------------------------------------------------------

ParticleSystem::CollisionObject::CollisionObject( void )
{
	friction = 0.0;
}

/*virtual*/ ParticleSystem::CollisionObject::~CollisionObject( void )
{
}

//-------------------------------------------------------------------------------------------------
//                                            CollisionPlane
//-------------------------------------------------------------------------------------------------

ParticleSystem::CollisionPlane::CollisionPlane( void )
{
}

/*virtual*/ ParticleSystem::CollisionPlane::~CollisionPlane( void )
{
}

/*virtual*/ bool ParticleSystem::CollisionPlane::ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal )
{
	if( plane.GetSide( lineOfMotion.vertex[1], 0.0 ) != Plane::SIDE_BACK )
		return false;

	contactPosition = lineOfMotion.vertex[1];
	plane.NearestPoint( contactPosition );

	contactUnitNormal = plane.normal;
	return true;
}

//-------------------------------------------------------------------------------------------------
//                                      ConvexTriangleMeshCollisionObject
//-------------------------------------------------------------------------------------------------

ParticleSystem::ConvexTriangleMeshCollisionObject::ConvexTriangleMeshCollisionObject( void )
{
	mesh = nullptr;
	boundingBox = nullptr;
}

/*virtual*/ ParticleSystem::ConvexTriangleMeshCollisionObject::~ConvexTriangleMeshCollisionObject( void )
{
}

/*virtual*/ bool ParticleSystem::ConvexTriangleMeshCollisionObject::ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal )
{
	if( boundingBox && !boundingBox->ContainsPoint( lineOfMotion.vertex[1] ) )
		return false;

	if( !mesh )
		return false;

	int count = 0;

	for( IndexTriangleList::const_iterator iter = mesh->triangleList->cbegin(); iter != mesh->triangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;
		
		Plane plane;
		indexTriangle.GetPlane( plane, mesh->vertexArray );

		if( Plane::SIDE_BACK == plane.GetSide( lineOfMotion.vertex[1], 0.0 ) )
			count++;
	}

	if( count < ( signed )mesh->triangleList->size() )
		return false;

	double smallestDistance = -1.0;

	for( IndexTriangleList::const_iterator iter = mesh->triangleList->cbegin(); iter != mesh->triangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;
		
		Plane plane;
		indexTriangle.GetPlane( plane, mesh->vertexArray );

		Vector nearestPointOnPlane = lineOfMotion.vertex[1];
		plane.NearestPoint( nearestPointOnPlane );
		double distance = lineOfMotion.vertex[1].Distance( nearestPointOnPlane );
		if( smallestDistance < 0.0 || distance < smallestDistance )
		{
			smallestDistance = distance;
			contactPosition = nearestPointOnPlane;
			contactUnitNormal = plane.normal;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
//                                    BoundingBoxTreeCollisionObject
//-------------------------------------------------------------------------------------------------

ParticleSystem::BoundingBoxTreeCollisionObject::BoundingBoxTreeCollisionObject( void )
{
	boxTree = nullptr;
	detectionDistance = 1.0;		// If this is too small, we'll tunnel.
}

/*virtual*/ ParticleSystem::BoundingBoxTreeCollisionObject::~BoundingBoxTreeCollisionObject( void )
{
}

/*virtual*/ bool ParticleSystem::BoundingBoxTreeCollisionObject::ResolveCollision( const LineSegment& lineOfMotion, Vector& contactPosition, Vector& contactUnitNormal )
{
	if( !boxTree )
		return false;

	const Triangle* nearestTriangle = nullptr;
	if( !boxTree->FindNearestTriangle( lineOfMotion.vertex[1], nearestTriangle, detectionDistance ) )
		return false;

	Plane plane;
	nearestTriangle->GetPlane( plane );
	if( plane.GetSide( lineOfMotion.vertex[1], 0.0 ) != Plane::SIDE_BACK )
		return false;

	// The contact position would, intuitively, be the intersection point,
	// but I have found that the nearest point to the plane works better.
	contactPosition = lineOfMotion.vertex[1];
	plane.NearestPoint( contactPosition );
	contactUnitNormal = plane.normal;
	return true;
}

//-------------------------------------------------------------------------------------------------
//                                                Emitter
//-------------------------------------------------------------------------------------------------

ParticleSystem::Emitter::Emitter( void )
{
}

/*virtual*/ ParticleSystem::Emitter::~Emitter( void )
{
}

// ParticleSystem.cpp