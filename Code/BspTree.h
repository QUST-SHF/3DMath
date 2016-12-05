// BspTree.h

#pragma once

#include "Defines.h"
#include "Plane.h"
#include "Triangle.h"

namespace _3DMath
{
	class BspTree;
	class Triangle;
	class TriangleMesh;
}

class _3DMATH_API _3DMath::BspTree
{
public:

	BspTree( void );
	virtual ~BspTree( void );

	void Generate( const TriangleList& triangleList );

	class _3DMATH_API Node
	{
	public:

		Node( void );
		~Node( void );

		Plane* plane;
		Node* front;
		Node* back;
		TriangleList* triangleList;
	};

	Node* root;
};

// BspTree.h
