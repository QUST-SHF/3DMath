// Renderer.cpp

#include "Renderer.h"
#include "LineSegment.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "IndexTriangle.h"
#include "LinearTransform.h"
#include "ParticleSystem.h"
#include "BoundingBoxTree.h"
#include "AffineTransform.h"
#include "ListFunctions.h"

using namespace _3DMath;

Renderer::Renderer( void )
{
	drawStyle = DRAW_STYLE_SOLID;
	cachedEdgeSet = new TriangleMesh::EdgeSet;
}

/*virtual*/ Renderer::~Renderer( void )
{
	delete cachedEdgeSet;
}

void Renderer::DrawVector( const Vector& vector, const Vector& position, const Vector& color, double arrowRadius, int arrowSegments /*= 8*/ )
{
	Vector unitVector;
	if( !vector.GetNormalized( unitVector ) )
		return;

	Vector arrowPoint;
	arrowPoint.Add( position, vector );

	BeginDrawMode( DRAW_MODE_LINES );
	IssueVertex( Vertex( position, color ) );
	IssueVertex( Vertex( arrowPoint, color ) );
	EndDrawMode();

	if( arrowSegments > 2 )
	{
		LinearTransform rotation;
		rotation.SetRotation( unitVector, 2.0 * M_PI / double( arrowSegments ) );

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
}

void Renderer::DrawLineSegment( const LineSegment& lineSegment )
{
}

void Renderer::DrawTriangle( const Triangle& triangle )
{
}

void Renderer::DrawPolygon( const Polygon& polygon )
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

void Renderer::DrawTriangleMesh( const TriangleMesh& triangleMesh, int drawFlags /*= 0*/, const AffineTransform* transform /*= nullptr*/ )
{
	switch( drawStyle )
	{
		case DRAW_STYLE_SOLID:
		{
			LinearTransform normalTransform;
			if( transform )
				transform->linearTransform.GetNormalTransform( normalTransform );

			BeginDrawMode( DRAW_MODE_TRIANGLES );

			IndexTriangleList::iterator iter = triangleMesh.triangleList->begin();
			while( iter != triangleMesh.triangleList->end() )
			{
				const IndexTriangle& triangle = *iter;
				
				Vertex vertex[3];
				for( int i = 0; i < 3; i++ )
					triangleMesh.GetVertex( triangle.vertex[i], vertex[i] );

				if( drawFlags & UV_CORRECTION )
				{
					CorrectUV( vertex[0].texCoords.x, vertex[1].texCoords.x );
					CorrectUV( vertex[0].texCoords.x, vertex[2].texCoords.x );
					CorrectUV( vertex[0].texCoords.y, vertex[1].texCoords.y );
					CorrectUV( vertex[0].texCoords.y, vertex[2].texCoords.y );
				}

				for( int i = 0; i < 3; i++ )
				{
					if( transform )
						transform->Transform( vertex[i], &normalTransform );

					IssueVertex( vertex[i] );
				}

				iter++;
			}

			EndDrawMode();

			break;
		}
		case DRAW_STYLE_WIRE_FRAME:
		{
			if( cachedEdgeSet->size() == 0 )
				triangleMesh.GenerateEdgeSet( *cachedEdgeSet );

			BeginDrawMode( DRAW_MODE_LINES );
			
			for( TriangleMesh::EdgeSet::iterator iter = cachedEdgeSet->begin(); iter != cachedEdgeSet->end(); iter++ )
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

	if( drawFlags & DRAW_NORMALS )
	{
		IndexTriangleList::iterator iter = triangleMesh.triangleList->begin();
		while( iter != triangleMesh.triangleList->end() )
		{
			const IndexTriangle& indexTriangle = *iter;

			Triangle triangle;
			indexTriangle.GetTriangle( triangle, triangleMesh.vertexArray );

			Plane plane;
			indexTriangle.GetPlane( plane, triangleMesh.vertexArray );

			Vector center;
			triangle.GetCenter( center );
			
			plane.normal.Scale( 0.2 );
			DrawVector( plane.normal, center, Vector( 1.0, 0.0, 0.0 ), 0.0, 0 );

			iter++;
		}
	}
}

void Renderer::DrawParticleSystem( const ParticleSystem& particleSystem, int drawFlags /*= DRAW_PARTICLES*/ )
{
	if( ( drawFlags & DRAW_FORCES ) != 0 )
		RenderList< ParticleSystem::Force >( *particleSystem.forceList, *this );

	if( ( drawFlags & DRAW_PARTICLES ) != 0 )
		RenderList< ParticleSystem::Particle >( *particleSystem.particleList, *this );

	if( ( drawFlags & DRAW_EMITTERS ) != 0 )
		RenderList< ParticleSystem::Emitter >( *particleSystem.emitterList, *this );

	if( ( drawFlags & DRAW_COLLISION_OBJECTS ) != 0 )
		RenderList< ParticleSystem::CollisionObject >( *particleSystem.collisionObjectList, *this );
}

void Renderer::DrawBoundingBoxTree( const BoundingBoxTree& boxTree, int drawFlags/*= DRAW_BOXES*/ )
{
	typedef std::list< const BoundingBoxTree::Node* > NodeList;

	NodeList nodeQueue;
	nodeQueue.push_back( boxTree.rootNode );

	while( nodeQueue.size() > 0 )
	{
		const BoundingBoxTree::Node* node = nodeQueue.front();
		nodeQueue.pop_front();

		const BoundingBoxTree::BranchNode* branchNode = dynamic_cast< const BoundingBoxTree::BranchNode* >( node );
		const BoundingBoxTree::LeafNode* leafNode = dynamic_cast< const BoundingBoxTree::LeafNode* >( node );

		if( branchNode )
		{
			nodeQueue.push_back( branchNode->backNode );
			nodeQueue.push_back( branchNode->frontNode );
		}
		else if( leafNode )
		{
			if( drawFlags & DRAW_BOXES )
				leafNode->boundingBox.Render( *this );

			if( drawFlags & DRAW_TRIANGLES )
			{
				Vector color;
				random.VectorInInterval( 0.5, 1.0, color );
				Color( color );

				BeginDrawMode( DRAW_MODE_TRIANGLES );

				for( TriangleList::const_iterator iter = leafNode->triangleList->cbegin(); iter != leafNode->triangleList->cend(); iter++ )
				{
					const Triangle& triangle = *iter;

					for( int i = 0; i < 3; i++ )
						IssueVertex( Vertex( triangle.vertex[i] ) );
				}

				EndDrawMode();
			}
		}
	}
}

// Renderer.cpp