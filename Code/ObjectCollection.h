// ObjectCollection.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class Object;
	class ObjectCollection;
	class Renderer;

	typedef std::map< int, Object* > ObjectMap;
}

// TODO: May want to revisit all this.  I also need the idea of a handle system.  Can this be consolidated with that?

class _3DMATH_API _3DMath::Object
{
public:

	Object( void );
	virtual ~Object( void );

	virtual void Render( Renderer* renderer ) const;

	int id;
};

class _3DMATH_API _3DMath::ObjectCollection
{
public:

	ObjectCollection( void );
	virtual ~ObjectCollection( void );

	bool AddObject( Object* object );
	Object* FindObject( int objectId );
	void Clear( void );

	void Render( Renderer* renderer ) const;

	ObjectMap* objectMap;
	int nextId;
};

// ObjectCollection.h
