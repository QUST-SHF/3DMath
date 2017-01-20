// Graph.h

#pragma once

#include "Defines.h"
#include "HandleObject.h"

namespace _3DMath
{
	class GraphNode;
	class GraphTraversor;

	void DeleteGraph( GraphNode* graphNode );
}

class _3DMATH_API _3DMath::GraphNode : public _3DMath::HandleObject
{
public:

	GraphNode( void );
	virtual ~GraphNode( void );

	GraphNode* GetAdjacency( const std::string& name );

	typedef std::map< std::string, int > AdjacencyMap;
	AdjacencyMap adjacencyMap;
};

class _3DMATH_API _3DMath::GraphTraversor
{
public:

	enum Mode
	{
		DEPTH_FIRST,
		BREADTH_FIRST,
	};

	GraphTraversor( GraphNode* graphNode, Mode mode = BREADTH_FIRST );
	virtual ~GraphTraversor( void );

	virtual bool Traverse( GraphNode** graphNode );
	virtual void EnqueueUnvisitedAdjacencies( GraphNode* graphNode );

	Mode mode;
	ObjectHandleList handleQueue;
	
	typedef std::set< int > VisitationSet;
	VisitationSet visitationSet;
};

// Graph.h