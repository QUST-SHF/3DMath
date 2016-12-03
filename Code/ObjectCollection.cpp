// ObjectCollection.cpp

#include "ObjectCollection.h"

using namespace _3DMath;

Object::Object( void )
{
	id = 0;
}

/*virtual*/ Object::~Object( void )
{
}

/*virtual*/ void Object::Render( Renderer* renderer ) const
{
}

ObjectCollection::ObjectCollection( void )
{
	objectMap = new ObjectMap();
	nextId = 1;
}

/*virtual*/ ObjectCollection::~ObjectCollection( void )
{
	Clear();
	delete objectMap;
}

void ObjectCollection::Clear( void )
{
	while( objectMap->size() > 0 )
	{
		ObjectMap::iterator iter = objectMap->begin();
		Object* object = iter->second;
		delete object;
		objectMap->erase( iter );
	}
}

bool ObjectCollection::AddObject( Object* object )
{
	if( object->id != 0 )
		return false;

	object->id = nextId++;
	objectMap->insert( std::pair< int, Object* >( object->id, object ) );
	return true;
}

Object* ObjectCollection::FindObject( int objectId )
{
	Object* object = nullptr;

	ObjectMap::iterator iter = objectMap->find( objectId );
	if( iter != objectMap->end() )
		object = iter->second;

	return object;
}

void ObjectCollection::Render( Renderer* renderer ) const
{
	ObjectMap::const_iterator iter = objectMap->cbegin();
	while( iter != objectMap->cend() )
	{
		const Object* object = iter->second;
		object->Render( renderer );
		iter++;
	}
}

// ObjectCollection.cpp