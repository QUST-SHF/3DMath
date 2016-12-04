// Renderer.cpp

#include "Renderer.h"
#include "LineSegment.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "LinearTransform.h"
#include "ParticleSystem.h"

using namespace _3DMath;

Renderer::Renderer( void )
{
	drawStyle = DRAW_STYLE_SOLID;
}

/*virtual*/ Renderer::~Renderer( void )
{
}

void Renderer::DrawVector( const Vector& vector, const Vector& position, double arrowRadius, int arrowSegments /*= 8*/ )
{
	Vector unitVector;
	if( !vector.GetNormalized( unitVector ) )
		return;

	LinearTransform rotation;
	rotation.SetRotation( unitVector, 2.0 * M_PI / double( arrowSegments ) );

	Vector arrowPoint;
	arrowPoint.Add( position, vector );

	BeginDrawMode( DRAW_MODE_LINES );
	IssueVertex( Vertex( position ) );
	IssueVertex( Vertex( arrowPoint ) );
	EndDrawMode();

	Vector arrowHeadBasePos;
	arrowHeadBasePos.AddScale( position, unitVector, vector.Length() - arrowRadius );

	Vector orthoVector;
	unitVector.Orthogonal( orthoVector );
	orthoVector.Scale( arrowRadius / orthoVector.Length() );

	Vertex vertex;
	vertex.position.Add( arrowHeadBasePos, orthoVector );

	BeginDrawMode( DRAW_MODE_TRIANGLE_FAN );
	IssueVertex( Vertex( arrowPoint ) );
	for( int i = 0; i <= arrowSegments; i++ )
	{
		IssueVertex( vertex );
		rotation.Transform( vertex.position );
	}
	EndDrawMode();
}

void Renderer::DrawLineSegment( const LineSegment& lineSegment )
{
}

void Renderer::DrawTriangle( const Triangle& triangle )
{
}

void Renderer::CorrectUV( double texCoordAnchor, double& texCoord )
{
	double distance = abs( texCoordAnchor - texCoord );

	if( distance > abs( texCoordAnchor - ( texCoord + 1.0 ) ) )
		texCoord += 1.0;
	else if( distance > abs( texCoordAnchor - ( texCoord - 1.0 ) ) )
		texCoord -= 1.0;
}

void Renderer::DrawTriangleMesh( const TriangleMesh& triangleMesh, int drawFlags /*= 0*/ )
{
	switch( drawStyle )
	{
		case DRAW_STYLE_SOLID:
		{
			BeginDrawMode( DRAW_MODE_TRIANGLES );

			TriangleMesh::IndexTriangleList::iterator iter = triangleMesh.triangleList->begin();
			while( iter != triangleMesh.triangleList->end() )
			{
				const TriangleMesh::IndexTriangle& triangle = *iter;
				
				Vertex vertex[3];
				for( int i = 0; i < 3; i++ )
					triangleMesh.GetVertex( triangle.vertex[i], vertex[i] );

				if( drawFlags & UV_CORRECTION )
				{
					CorrectUV( vertex[0].u, vertex[1].u );
					CorrectUV( vertex[0].u, vertex[2].u );
					CorrectUV( vertex[0].v, vertex[1].v );
					CorrectUV( vertex[0].v, vertex[2].v );
				}

				for( int i = 0; i < 3; i++ )
					IssueVertex( vertex[i] );

				iter++;
			}

			EndDrawMode();

			break;
		}
		case DRAW_STYLE_WIRE_FRAME:
		{
			TriangleMesh::EdgeSet edgeSet;
			triangleMesh.GenerateEdgeSet( edgeSet );

			BeginDrawMode( DRAW_MODE_LINES );
			
			for( TriangleMesh::EdgeSet::iterator iter = edgeSet.begin(); iter != edgeSet.end(); iter++ )
			{
				uint64_t edgePair = *iter;

				int index0, index1;
				TriangleMesh::GetEdgePair( edgePair, index0, index1 );

				const Vertex* vertex0, *vertex1;
				triangleMesh.GetVertex( index0, vertex0 );
				triangleMesh.GetVertex( index1, vertex1 );

				IssueVertex( *vertex0 );
				IssueVertex( *vertex1 );
			}

			EndDrawMode();

			break;
		}
		case DRAW_STYLE_VERTICES:
		{
			BeginDrawMode( DRAW_MODE_POINTS );

			for( int i = 0; i < ( signed )triangleMesh.vertexArray->size(); i++ )
			{
				const Vertex& vertex = ( *triangleMesh.vertexArray )[i];
				IssueVertex( vertex );
			}

			EndDrawMode();

			break;
		}
	}
}

void Renderer::DrawParticleSystem( const ParticleSystem& particleSystem, int drawFlags /*= DRAW_PARTICLES*/ )
{
	if( ( drawFlags & DRAW_FORCES ) != 0 )
		particleSystem.forceCollection.Render( this );

	if( ( drawFlags & DRAW_PARTICLES ) != 0 )
		particleSystem.particleCollection.Render( this );

	if( ( drawFlags & DRAW_EMITTERS ) != 0 )
		particleSystem.emitterCollection.Render( this );

	if( ( drawFlags & DRAW_COLLISION_OBJECTS ) != 0 )
		particleSystem.collisionObjectCollection.Render( this );
}

Vertex::Vertex( void )
{
	position.Set( 0.0, 0.0, 0.0 );
	normal.Set( 0.0, 0.0, 0.0 );
	u = 0.0;
	v = 0.0;
}

Vertex::Vertex( const Vector& position )
{
	this->position = position;
	normal.Set( 0.0, 0.0, 0.0 );
	u = 0.0;
	v = 0.0;
}

Vertex::Vertex( const Vector& position, const Vector& normal, double u, double v )
{
	this->position = position;
	this->normal = normal;
	this->v = v;
	this->u = u;
}

Vertex::~Vertex( void )
{
}

// Renderer.cpp