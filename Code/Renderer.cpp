// Renderer.cpp

#include "Renderer.h"
#include "LineSegment.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "LinearTransform.h"

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

void Renderer::DrawTriangleMesh( const TriangleMesh& triangleMesh )
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
				
				for( int i = 0; i < 3; i++ )
				{
					Vertex vertex;
					triangleMesh.GetVertex( triangle.vertex[i], vertex );
					IssueVertex( vertex );
				}

				iter++;
			}

			EndDrawMode();

			break;
		}
		case DRAW_STYLE_WIRE_FRAME:
		{
			// TODO: Use map to make sure we don't draw an edge more than once?
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