// ParticleSystem.cpp

#include "ParticleSystem.h"
#include "TriangleMesh.h"

using namespace _3DMath;

//-------------------------------------------------------------------------------------------------
//                                        Particlesystem
//-------------------------------------------------------------------------------------------------

ParticleSystem::ParticleSystem( void )
{
	centerOfMass.Set( 0.0, 0.0, 0.0 );
	previousTime = 0.0;
}

/*virtual*/ ParticleSystem::~ParticleSystem( void )
{
}

void ParticleSystem::Clear( void )
{
	centerOfMass.Set( 0.0, 0.0, 0.0 );
	previousTime = 0.0;

	particleCollection.Clear();
	forceCollection.Clear();
	collisionObjectCollection.Clear();
	emitterCollection.Clear();
}

void ParticleSystem::Simulate( double currentTime )
{
	CullDeadParticles( currentTime );
	ResetParticlePhysics();
	CalculateCenterOfMass();
	AccumulateForces();
	IntegrateParticles( currentTime );
	ResolveCollisions();
}

void ParticleSystem::CullDeadParticles( double currentTime )
{
	ObjectMap::iterator iter = particleCollection.objectMap->begin();
	while( iter != particleCollection.objectMap->end() )
	{
		ObjectMap::iterator nextIter = iter;
		nextIter++;

		Particle* particle = ( Particle* )iter->second;
		if( particle->timeOfDeath != 0.0 && particle->timeOfDeath <= currentTime )
		{
			particleCollection.objectMap->erase( iter );
			delete particle;
		}

		iter = nextIter;
	}
}

void ParticleSystem::ResetParticlePhysics( void )
{
	ObjectMap::iterator iter = particleCollection.objectMap->begin();
	while( iter != particleCollection.objectMap->end() )
	{
		Particle* particle = ( Particle* )iter->second;
		particle->netForce.Set( 0.0, 0.0, 0.0 );
		iter++;
	}
}

void ParticleSystem::AccumulateForces( void )
{
	ObjectMap::iterator iter = forceCollection.objectMap->begin();
	while( iter != forceCollection.objectMap->end() )
	{
		ObjectMap::iterator nextIter = iter;
		nextIter++;
		
		Force* force = ( Force* )iter->second;
		force->Apply();

		if( force->transient )
		{
			delete force;
			forceCollection.objectMap->erase( iter );
		}

		iter = nextIter;
	}
}

void ParticleSystem::IntegrateParticles( double currentTime )
{
	if( previousTime == 0.0 )
		previousTime = currentTime;

	double deltaTime = currentTime - previousTime;

	ObjectMap::iterator iter = particleCollection.objectMap->begin();
	while( iter != particleCollection.objectMap->end() )
	{
		Particle* particle = ( Particle* )iter->second;
		particle->Integrate( deltaTime );
		iter++;
	}

	previousTime = currentTime;
}

void ParticleSystem::ResolveCollisions( void )
{
	ObjectMap::iterator iter = particleCollection.objectMap->begin();
	while( iter != particleCollection.objectMap->end() )
	{
		Particle* particle = ( Particle* )iter->second;

		ImpactInfo impactInfo;
		impactInfo.netForceAtImpact = particle->netForce;
		impactInfo.friction = 0.0;
		impactInfo.contactPosition.Set( 0.0, 0.0, 0.0 );
		impactInfo.contactUnitNormal.Set( 0.0, 0.0, 0.0 );
		impactInfo.lineOfMotion.vertex[0] = particle->previousPosition;
		particle->GetPosition( impactInfo.lineOfMotion.vertex[1] );

		ObjectMap::iterator collisionIter = collisionObjectCollection.objectMap->begin();
		while( collisionIter != collisionObjectCollection.objectMap->end() )
		{
			CollisionObject* collisionObject = ( CollisionObject* )collisionIter->second;

			if( collisionObject->ResolveCollision( impactInfo ) )
			{
				particle->SetPosition( impactInfo.contactPosition );

				if( impactInfo.friction != 0.0 )
				{
					FrictionForce* frictionForce = new FrictionForce( this );
					frictionForce->impactInfo = impactInfo;
					forceCollection.AddObject( frictionForce );
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

	ObjectMap::iterator iter = particleCollection.objectMap->begin();
	while( iter != particleCollection.objectMap->end() )
	{
		Particle* particle = ( Particle* )iter->second;

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
}

/*virtual*/ ParticleSystem::Particle::~Particle( void )
{
}

/*virtual*/ void ParticleSystem::Particle::Integrate( double deltaTime )
{
	// TODO: To increase accuracy, we may want to integrate the delta-time over
	//       smaller time intervals and also consider other integration methods.

	acceleration.SetScaled( netForce, 1.0 / mass );
	velocity.AddScale( acceleration, deltaTime );

	Vector currentPosition;
	GetPosition( currentPosition );

	previousPosition = currentPosition;

	currentPosition.AddScale( velocity, deltaTime );
	SetPosition( currentPosition );
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
	ObjectMap::iterator iter = system->particleCollection.objectMap->begin();
	while( iter != system->particleCollection.objectMap->end() )
	{
		Particle* particle = ( Particle* )iter->second;
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

	Vector torqueForce;
	torqueForce.Cross( torque, vector );
	torqueForce.Scale( 1.0 / torqueForce.Dot( torqueForce ) );

	particle->netForce.Add( torqueForce );
}

//-------------------------------------------------------------------------------------------------
//                                            SpringForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::SpringForce::SpringForce( ParticleSystem* system ) : Force( system )
{
	endPointParticleIds[0] = 0;
	endPointParticleIds[1] = 0;
	equilibriumLength = 1.0;
	stiffness = 1.0;
}

/*virtual*/ ParticleSystem::SpringForce::~SpringForce( void )
{
}

/*virtual*/ void ParticleSystem::SpringForce::Apply( void )
{
	Particle* particleA = ( Particle* )system->particleCollection.FindObject( endPointParticleIds[0] );
	Particle* particleB = ( Particle* )system->particleCollection.FindObject( endPointParticleIds[1] );

	if( particleA && particleB )
	{
		Vector positionA, positionB;

		particleA->GetPosition( positionA );
		particleB->GetPosition( positionB );

		Vector vector;
		vector.Subtract( positionB, positionA );

		double length = vector.Length();

		Vector springForce;
		springForce.SetScaled( vector, stiffness * ( length - equilibriumLength ) / length );

		particleA->netForce.Add( springForce );
		particleB->netForce.Subtract( springForce );
	}
}

/*virtual*/ void ParticleSystem::SpringForce::Render( Renderer* renderer ) const
{
	Particle* particleA = ( Particle* )system->particleCollection.FindObject( endPointParticleIds[0] );
	Particle* particleB = ( Particle* )system->particleCollection.FindObject( endPointParticleIds[1] );

	if( particleA && particleB )
	{
		Vector positionA, positionB;

		particleA->GetPosition( positionA );
		particleB->GetPosition( positionB );

		renderer->BeginDrawMode( Renderer::DRAW_MODE_LINES );

		renderer->IssueVertex( Vertex( positionA ) );
		renderer->IssueVertex( Vertex( positionB ) );

		renderer->EndDrawMode();
	}
}

//-------------------------------------------------------------------------------------------------
//                                            FrictionForce
//-------------------------------------------------------------------------------------------------

ParticleSystem::FrictionForce::FrictionForce( ParticleSystem* system ) : Force( system )
{
	particleId = 0;
	impactInfo.contactPosition.Set( 0.0, 0.0, 0.0 );
	impactInfo.contactUnitNormal.Set( 0.0, 0.0, 0.0 );
	impactInfo.netForceAtImpact.Set( 0.0, 0.0, 0.0 );
	impactInfo.friction = 0.0;
	transient = true;
}

/*virtual*/ ParticleSystem::FrictionForce::~FrictionForce( void )
{
}

/*virtual*/ void ParticleSystem::FrictionForce::Apply( void )
{
	Particle* particle = ( Particle* )system->particleCollection.FindObject( particleId );
	if( particle )
	{
		double normalForce = impactInfo.contactUnitNormal.Dot( impactInfo.netForceAtImpact );
		if( normalForce <= 0.0 )
		{
			Vector impactUnitDir;
			impactUnitDir.Subtract( impactInfo.lineOfMotion.vertex[1], impactInfo.lineOfMotion.vertex[0] );
			impactUnitDir.Normalize();

			Vector frictionForce;
			frictionForce.SetScaled( impactUnitDir, impactInfo.friction * normalForce );

			particle->netForce.Add( frictionForce );
		}
	}
}

//-------------------------------------------------------------------------------------------------
//                                            CollisionObject
//-------------------------------------------------------------------------------------------------

ParticleSystem::CollisionObject::CollisionObject( void )
{
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

/*virtual*/ bool ParticleSystem::CollisionPlane::ResolveCollision( ImpactInfo& impactInfo )
{
	return false;
}

//-------------------------------------------------------------------------------------------------
//                                      TriangleMeshCollisionObject
//-------------------------------------------------------------------------------------------------

ParticleSystem::TriangleMeshCollisionObject::TriangleMeshCollisionObject( void )
{
	mesh = nullptr;
}

/*virtual*/ ParticleSystem::TriangleMeshCollisionObject::~TriangleMeshCollisionObject( void )
{
}

/*virtual*/ bool ParticleSystem::TriangleMeshCollisionObject::ResolveCollision( ImpactInfo& impactInfo )
{
	return false;
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