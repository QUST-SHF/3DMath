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
#include "Polygon.h"
#include "Surface.h"
#include "Plane.h"
#include "Sphere.h"
#include "Spline.h"

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

void Renderer::DrawVector( const Vector& vector, const Vector& position, const Vector& color, double alpha /*= 1.0*/, double arrowRadius /*= 1.0*/, int arrowSegments /*= 8*/ )
{
	Vector unitVector;
	if( !vector.GetNormalized( unitVector ) )
		return;

	Vertex vertex;
	vertex.color = color;
	vertex.alpha = alpha;

	Vector arrowPoint;
	arrowPoint.Add( position, vector );

	BeginDrawMode( DRAW_MODE_LINES );
	vertex.position = position;
	IssueVertex( vertex );
	vertex.position = arrowPoint;
	IssueVertex( vertex );
	EndDrawMode();

	if( arrowSegments > 2 )
	{
		_3DMath::AffineTransform transform;
		transform.linearTransform.BuildFrameUsingVector( unitVector );
		transform.translation.AddScale( position, unitVector, vector.Length() - arrowRadius );

		Vector centers[2];
		centers[0].Set( 0.0, 0.0, arrowRadius );
		centers[1].Set( 0.0, 0.0, 0.0 );

		DrawDoubleFan( transform, arrowRadius, centers, arrowSegments, color, alpha );
	}
}

void Renderer::DrawDoubleFan( const _3DMath::AffineTransform& transform, double radius, const Vector* centers, int segments, const Vector& color, double alpha )
{
	Vertex vertex;
	vertex.color = color;
	vertex.alpha = alpha;

	double angleDelta = 2.0 * M_PI / double( segments );

	for( int i = 0; i < 2; i++ )
	{
		BeginDrawMode( DRAW_MODE_TRIANGLE_FAN );
	
		vertex.normal = transform.linearTransform.zAxis;
		if( i == 1 )
			vertex.normal.Negate();

		vertex.position = centers[i];
		transform.Transform( vertex.position );
		IssueVertex( vertex );

		for( int j = 0; j <= segments; j++ )
		{
			int k = ( i == 0 ) ? j : ( segments - j );
			double angle = double(k) * angleDelta;

			vertex.position.Set( radius * cos( angle ), radius * sin( angle ), 0.0 );
			transform.Transform( vertex.position );
			IssueVertex( vertex );
		}

		EndDrawMode();
	}
}

void Renderer::DrawSurface( const Surface& surface, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ )
{
	surface.Render( *this, color, alpha, transform );
}

void Renderer::DrawSphere( const Sphere& sphere, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ )
{
	LinearTransform normalTransform;
	if( transform )
		transform->linearTransform.GetNormalTransform( normalTransform );

	Vertex vertex;
	vertex.color = color;
	vertex.alpha = alpha;

	class VertexCalculator
	{
	public:
		void Calculate( int lat, int lon, Vertex& vertex )
		{
			double phi = double( lat ) / double( lattitudes ) * M_PI;
			double cosPhi = cos( phi );
			double sinPhi = sin( phi );
			double theta = double( lon ) / double( longitudes ) * 2.0 * M_PI;
			double cosTheta = cos( theta );
			double sinTheta = sin( theta );

			vertex.position.Set( sinPhi * cosTheta, cosPhi, sinPhi * sinTheta );
			vertex.normal = vertex.position;
			vertex.position.Scale( sphere->radius );
			vertex.position.Add( sphere->center );

			if( transform )
			{
				transform->Transform( vertex.position );
				normalTransform->Transform( vertex.normal );
			}
		}

		const Sphere* sphere;
		const AffineTransform* transform;
		const LinearTransform* normalTransform;
		int lattitudes;
		int longitudes;
	};

	VertexCalculator calculator;
	calculator.sphere = &sphere;
	calculator.transform = transform;
	calculator.normalTransform = &normalTransform;
	calculator.lattitudes = 20;
	calculator.longitudes = 40;

	BeginDrawMode( DRAW_MODE_TRIANGLE_FAN );

	calculator.Calculate( 0, 0, vertex );
	IssueVertex( vertex );

	for( int lon = calculator.longitudes; lon >= 0; lon-- )
	{
		calculator.Calculate( 1, lon, vertex );
		IssueVertex( vertex );
	}

	EndDrawMode();
	BeginDrawMode( DRAW_MODE_TRIANGLE_FAN );

	calculator.Calculate( calculator.lattitudes, 0, vertex );
	IssueVertex( vertex );

	for( int lon = 0; lon <= calculator.longitudes; lon++ )
	{
		calculator.Calculate( calculator.lattitudes - 1, lon, vertex );
		IssueVertex( vertex );
	}

	EndDrawMode();

	for( int lat = 1; lat <= calculator.lattitudes - 2; lat++ )
	{
		BeginDrawMode( DRAW_MODE_QUAD_STRIP );

		for( int lon = 0; lon <= calculator.longitudes; lon++ )
		{
			calculator.Calculate( lat + 1, lon, vertex );
			IssueVertex( vertex );

			calculator.Calculate( lat, lon, vertex );
			IssueVertex( vertex );
		}

		EndDrawMode();
	}
}

void Renderer::DrawPlane( const Plane& plane, double radius, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ )
{
	Vector planeCenter;
	plane.GetCenter( planeCenter );
	transform->Transform( planeCenter );

	LinearTransform normalTransform;
	transform->linearTransform.GetNormalTransform( normalTransform );
	Vector planeNormal = plane.normal;
	normalTransform.Transform( planeNormal );

	AffineTransform fanTransform;
	fanTransform.linearTransform.BuildFrameUsingVector( planeNormal );
	fanTransform.translation = planeCenter;

	Vector centers[2];
	centers[0].Set( 0.0, 0.0, 0.0 );
	centers[1].Set( 0.0, 0.0, 0.0 );

	DrawDoubleFan( fanTransform, radius, centers, 20, color, alpha );
}

void Renderer::DrawLineSegment( const LineSegment& lineSegment )
{
}

void Renderer::DrawTriangle( const Triangle& triangle )
{
}

void Renderer::DrawPolygon( const Polygon& polygon, const AffineTransform* transform /*= nullptr*/, bool drawTessellation /*= true*/ )
{
	switch( drawStyle )
	{
		case DRAW_STYLE_SOLID:
		{
			Plane plane;
			polygon.GetPlane( plane );

			if( drawTessellation )
			{
				BeginDrawMode( DRAW_MODE_TRIANGLES );	

				IndexTriangleList::const_iterator iter = polygon.indexTriangleList->cbegin();
				while( iter != polygon.indexTriangleList->cend() )
				{
					const IndexTriangle& indexTriangle = *iter;

					for( int i = 0; i < 3; i++ )
					{
						Vector point = ( *polygon.vertexArray )[ indexTriangle.vertex[i] ];
						if( transform )
							transform->Transform( point );

						Vertex vertex;
						vertex.position = point;
						vertex.normal = plane.normal;
						IssueVertex( vertex, VTX_FLAG_POSITION | VTX_FLAG_NORMAL );
					}

					iter++;
				}

				EndDrawMode();
			}
			else
			{
				BeginDrawMode( DRAW_MODE_POLYGON );

				for( int i = 0; i < ( signed )polygon.vertexArray->size(); i++ )
				{
					Vector point = ( *polygon.vertexArray )[i];
					if( transform )
						transform->Transform( point );

					Vertex vertex;
					vertex.position = point;
					vertex.normal = plane.normal;
					IssueVertex( vertex, VTX_FLAG_POSITION | VTX_FLAG_NORMAL );
				}

				EndDrawMode();
			}

			break;
		}
		case DRAW_STYLE_WIRE_FRAME:
		{
			BeginDrawMode( DRAW_MODE_LINE_LOOP );

			for( int i = 0; i < ( signed )polygon.vertexArray->size(); i++ )
			{
				Vector point = ( *polygon.vertexArray )[i];
				if( transform )
					transform->Transform( point );

				Vertex vertex;
				vertex.position = point;
				IssueVertex( vertex, VTX_FLAG_POSITION );
			}

			EndDrawMode();

			break;
		}
	}
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

void Renderer::DrawSpline( const Spline& spline, const _3DMath::AffineTransform& transform, double maxSegmentLength )
{
	VectorArray pointArray;
	spline.CalcSplinePolyline( maxSegmentLength, pointArray );

	transform.Transform( pointArray );

	BeginDrawMode( DRAW_MODE_LINE_STRIP );

	for( int i = 0; i < ( signed )pointArray.size(); i++ )
		IssueVertex( Vertex( pointArray[i] ), VTX_FLAG_POSITION );

	EndDrawMode();
}

// Renderer.cpp