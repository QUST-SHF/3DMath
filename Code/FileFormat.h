// FileFormat.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
    class FileFormat;
    class PlyFormat;
	class ObjFormat;
    class TriangleMesh;
	class Vector;
}

class _3DMATH_API _3DMath::FileFormat
{
public:

    FileFormat( void );
    virtual ~FileFormat( void );

	virtual bool LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream ) = 0;
	virtual bool SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream ) = 0;

    virtual bool LoadTriangleMesh( TriangleMesh& triangleMesh, const std::string& file );
    virtual bool SaveTriangleMesh( const TriangleMesh& triangleMesh, const std::string& file );

	static FileFormat* CreateForFile( const std::string& file );

protected:

	typedef std::vector< std::string > StringArray;
    typedef std::vector< StringArray* > LineArray;

	LineArray* TokenizeFile( std::istream& stream );
};

class _3DMATH_API _3DMath::PlyFormat : public _3DMath::FileFormat
{
public:

    PlyFormat( void );
    virtual ~PlyFormat( void );

    virtual bool LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream ) override;
    virtual bool SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream ) override;

private:

    void AddVertex( TriangleMesh& triangleMesh, const LineArray::iterator& headerIter, const LineArray::iterator& bodyIter );
    void AddTriangles( TriangleMesh& triangleMesh, const LineArray::iterator& headerIter, const LineArray::iterator& bodyIter );
};

class _3DMATH_API _3DMath::ObjFormat : public _3DMath::FileFormat
{
public:

	ObjFormat( void );
    virtual ~ObjFormat( void );

    virtual bool LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream ) override;
    virtual bool SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream ) override;

private:

	int FindFirstLine( const LineArray* lineArray, const std::string& lineType );
	bool ParseVertex( const std::string& vertexString, int& positionIndex, int& textureIndex, int& normalIndex );
	void PopulateVector( const StringArray* stringArray, int startIndex, Vector& vector );
};

// FileFormat.h