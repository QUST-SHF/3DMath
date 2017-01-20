// Graph.cpp

#include "Graph.h"

using namespace _3DMath;

void DeleteGraph( GraphNode* graphNode )
{
	GraphTraversor traversor( graphNode );
	while( traversor.Traverse( &graphNode ) )
		delete graphNode;
}

//------------------------------------------------------------------------------------
//                                       GraphNode
//------------------------------------------------------------------------------------

GraphNode::GraphNode( void )
{
}

/*virtual*/ GraphNode::~GraphNode( void )
{
}

GraphNode* GraphNode::GetAdjacency( const std::string& name )
{
	GraphNode* adjacentNode = nullptr;

	AdjacencyMap::iterator iter = adjacencyMap.find( name );
	if( iter != adjacencyMap.end() )
	{
		int graphNodeHandle = iter->second;
		adjacentNode = ( GraphNode* )HandleObject::Dereference( graphNodeHandle );
	}

	return adjacentNode;
}

//------------------------------------------------------------------------------------
//                                    GraphTraversor
//------------------------------------------------------------------------------------

GraphTraversor::GraphTraversor( GraphNode* graphNode, Mode mode /*= BREADTH_FIRST*/ )
{
	this->mode = mode;

	handleQueue.push_back( graphNode->GetHandle() );
}

/*virtual*/ GraphTraversor::~GraphTraversor( void )
{
}

/*virtual*/ bool GraphTraversor::Traverse( GraphNode** graphNode )
{
	*graphNode = nullptr;
	if( handleQueue.size() == 0 )
		return false;

	ObjectHandleList::iterator iter = handleQueue.begin();
	int graphNodeHandle = *iter;
	handleQueue.erase( iter );

	visitationSet.insert( graphNodeHandle );

	*graphNode = ( GraphNode* )HandleObject::Dereference( graphNodeHandle );

	EnqueueUnvisitedAdjacencies( *graphNode );

	return true;
}

/*virtual*/ void GraphTraversor::EnqueueUnvisitedAdjacencies( GraphNode* graphNode )
{
	for( GraphNode::AdjacencyMap::iterator iter = graphNode->adjacencyMap.begin(); iter != graphNode->adjacencyMap.end(); iter++ )
	{
		int graphNodeHandle = iter->second;
		VisitationSet::iterator visitIter = visitationSet.find( graphNodeHandle );
		if( visitIter == visitationSet.end() )
		{
			switch( mode )
			{
				case BREADTH_FIRST:
				{
					handleQueue.push_back( graphNodeHandle );
					break;
				}
				case DEPTH_FIRST:
				{
					handleQueue.push_front( graphNodeHandle );
					break;
				}
			}
		}
	}
}

// Graph.cpp