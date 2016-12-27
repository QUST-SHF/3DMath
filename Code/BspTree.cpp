// BspTree.cpp

#include "BspTree.h"
#include "LineSegment.h"
#include "Exception.h"
#include "AffineTransform.h"
#include "IndexTriangle.h"

using namespace _3DMath;

//------------------------------------------------------------------------------------------
//                                        BspTree
//------------------------------------------------------------------------------------------

BspTree::BspTree( void )
{
	rootNode = nullptr;
	vertexArray = nullptr;
}

/*virtual*/ BspTree::~BspTree( void )
{
	Clear();
}

void BspTree::Clear( void )
{
	delete rootNode;
	delete vertexArray;

	rootNode = nullptr;
	vertexArray = nullptr;
}

bool BspTree::Generate( const TriangleMesh& triangleMesh )
{
	Clear();

	vertexArray = new std::vector< Vertex >;
	for( int i = 0; i < ( signed )triangleMesh.vertexArray->size(); i++ )
		vertexArray->push_back( ( *triangleMesh.vertexArray )[i] );

	IndexTriangleList triangleList;
	for( IndexTriangleList::const_iterator iter = triangleMesh.triangleList->cbegin(); iter != triangleMesh.triangleList->cend(); iter++ )
		triangleList.push_back( *iter );

	try
	{
		rootNode = new Node();
		rootNode->Generate( triangleList, *vertexArray );
	}
	catch( Exception* exception )
	{
		exception->Handle();
		Clear();
		return false;
	}

	return true;
}

void BspTree::Render( Renderer& renderer, RenderMode renderMode, const Vector& eye, const AffineTransform* transform /*= nullptr*/, int vertexFlags /*= 0*/ ) const
{
	AffineTransform identityTransform;
	if( !transform )
	{
		identityTransform.Identity();
		transform = &identityTransform;
	}

	LinearTransform normalTransform;
	transform->linearTransform.GetNormalTransform( normalTransform );

	if( rootNode )
	{
		renderer.BeginDrawMode( Renderer::DRAW_MODE_TRIANGLES );
		rootNode->Render( renderer, renderMode, eye, this, *transform, normalTransform, vertexFlags );
		renderer.EndDrawMode();
	}
}

void BspTree::Transform( const AffineTransform& transform )
{
	if( rootNode )
		rootNode->Transform( transform );

	transform.Transform( *vertexArray );
}

/*virtual*/ bool BspTree::FrontSpaceVisible( const Node* node ) const
{
	return true;
}

/*virtual*/ bool BspTree::BackSpaceVisible( const Node* node ) const
{
	return true;
}

//------------------------------------------------------------------------------------------
//                                        Node
//------------------------------------------------------------------------------------------

BspTree::Node::Node( void )
{
	frontNode = nullptr;
	backNode = nullptr;
	triangleList = new IndexTriangleList;
}

/*virtual*/ BspTree::Node::~Node( void )
{
	delete frontNode;
	delete backNode;
	delete triangleList;
}

void BspTree::Node::Transform( const AffineTransform& transform )
{
	partitioningPlane.Transform( transform );

	if( frontNode )
		frontNode->Transform( transform );

	if( backNode )
		backNode->Transform( transform );
}

void BspTree::Node::Render( Renderer& renderer, RenderMode renderMode, const Vector& eye, const BspTree* bspTree, const AffineTransform& transform, const LinearTransform& normalTransform, int vertexFlags ) const
{
	Plane plane = partitioningPlane;
	plane.Transform( transform, &normalTransform );

	Plane::Side side = plane.GetSide( eye );

	int order = 0;

	switch( renderMode )
	{
		case RENDER_BACK_TO_FRONT:
		{
			if( side == Plane::SIDE_BACK )
				order = 0;
			else
				order = 1;
			break;
		}
		case RENDER_FRONT_TO_BACK:
		{
			if( side == Plane::SIDE_FRONT )
				order = 0;
			else
				order = 1;
			break;
		}
	}

	Node* visibleFrontNode = bspTree->FrontSpaceVisible( this ) ? frontNode : nullptr;
	Node* visibleBackSNode = bspTree->BackSpaceVisible( this ) ? backNode : nullptr;

	Node* firstNode = nullptr;
	Node* lastNode = nullptr;

	switch( order )
	{
		case 0:
		{
			firstNode = visibleFrontNode;
			lastNode = visibleBackSNode;
			break;
		}
		case 1:
		{
			firstNode = visibleBackSNode;
			lastNode = visibleFrontNode;
			break;
		}
	}

	if( firstNode )
		firstNode->Render( renderer, renderMode, eye, bspTree, transform, normalTransform, vertexFlags );

	for( IndexTriangleList::const_iterator iter = triangleList->cbegin(); iter != triangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;

		for( int i = 0; i < 3; i++ )
		{
			Vertex vertex = ( *bspTree->vertexArray )[ indexTriangle.vertex[i] ];
			transform.Transform( vertex, &normalTransform );
			renderer.IssueVertex( vertex, vertexFlags );
		}
	}

	if( lastNode )
		lastNode->Render( renderer, renderMode, eye, bspTree, transform, normalTransform, vertexFlags );
}

void BspTree::Node::Generate( IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray )
{
	IndexTriangleList::iterator iter = ChooseBestPartitioningTriangle( givenTriangleList, vertexArray );
	IndexTriangle indexTriangle = *iter;
	indexTriangle.GetPlane( partitioningPlane, &vertexArray );
	triangleList->push_back( indexTriangle );

	IndexTriangleList frontIndexTriangleList, backIndexTriangleList;

	while( givenTriangleList.size() > 0 )
	{
		iter = givenTriangleList.begin();
		indexTriangle = *iter;
		givenTriangleList.erase( iter );

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, &vertexArray );

		int frontCount = 0;
		int backCount = 0;
		int neitherCount = 0;

		for( int i = 0; i < 3; i++ )
		{
			Plane::Side side = partitioningPlane.GetSide( triangle.vertex[i] );
			if( side == Plane::SIDE_FRONT )
				frontCount++;
			else if( side == Plane::SIDE_BACK )
				backCount++;
			else if( side == Plane::SIDE_NEITHER )
				neitherCount++;
		}

		if( neitherCount == 3 )
			triangleList->push_back( indexTriangle );
		else if( backCount == 0 )
			frontIndexTriangleList.push_back( indexTriangle );
		else if( frontCount == 0 )
			backIndexTriangleList.push_back( indexTriangle );
		else
		{
			TriangleList frontList, backList;
			partitioningPlane.SplitTriangle( triangle, frontList, backList );

			AddSubTriangles( frontIndexTriangleList, vertexArray, indexTriangle, frontList );
			AddSubTriangles( backIndexTriangleList, vertexArray, indexTriangle, backList );
		}
	}

	if( frontIndexTriangleList.size() > 0 )
	{
		frontNode = new Node();
		frontNode->Generate( frontIndexTriangleList, vertexArray );
	}

	if( backIndexTriangleList.size() > 0 )
	{
		backNode = new Node();
		backNode->Generate( backIndexTriangleList, vertexArray );
	}
}

void BspTree::Node::AddSubTriangles( IndexTriangleList& triangleList, std::vector< Vertex >& vertexArray, const IndexTriangle& indexTriangle, const TriangleList& subTriangleList )
{
	for( TriangleList::const_iterator iter = subTriangleList.cbegin(); iter != subTriangleList.cend(); iter++ )
	{
		const Triangle& subTriangle = *iter;

		IndexTriangle newIndexTriangle;

		for( int i = 0; i < 3; i++ )
		{
			const Vector* subVertex = &subTriangle.vertex[i];

			int j;
			for( j = 0; j < 3; j++ )
			{
				Vertex* vertex = &vertexArray[ indexTriangle.vertex[j] ];
				if( vertex->position.IsEqualTo( *subVertex ) )
				{
					newIndexTriangle.vertex[i] = indexTriangle.vertex[j];
					break;
				}
			}

			if( j < 3 )
				continue;

			for( j = 0; j < 3; j++ )
			{
				int k = ( j + 1 ) % 3;

				Vertex* vertexA = &vertexArray[ indexTriangle.vertex[j] ];
				Vertex* vertexB = &vertexArray[ indexTriangle.vertex[k] ];

				LineSegment lineSegment;
				lineSegment.vertex[0] = vertexA->position;
				lineSegment.vertex[1] = vertexB->position;

				double lambda;
				if( lineSegment.LerpInverse( lambda, *subVertex ) )
				{
					Vertex newVertex;
					newVertex.position = *subVertex;
					newVertex.texCoords.Lerp( vertexA->texCoords, vertexB->texCoords, lambda );
					newVertex.color.Lerp( vertexA->color, vertexB->color, lambda );
					newVertex.alpha = ( 1.0 - lambda ) * vertexA->alpha + lambda * vertexB->alpha;
					newVertex.normal.Slerp( vertexA->normal, vertexB->normal, lambda );

					vertexArray.push_back( newVertex );
					newIndexTriangle.vertex[i] = signed( vertexArray.size() ) - 1;
					break;
				}
			}

			if( j <= 3 )
				continue;

			throw new Exception( "Failed to add sub-triangle!" );
		}

		triangleList.push_back( newIndexTriangle );
	}
}

IndexTriangleList::iterator BspTree::Node::ChooseBestPartitioningTriangle( IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray )
{
	IndexTriangleList::iterator iter = givenTriangleList.begin();
	return iter;
}

// BspTree.cpp