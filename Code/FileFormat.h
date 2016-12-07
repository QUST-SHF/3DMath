// FileFormat.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
    class FileFormat;
    class PlyFormat;
    class TriangleMesh;
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
};

class _3DMATH_API _3DMath::PlyFormat : public _3DMath::FileFormat
{
public:

    PlyFormat( void );
    virtual ~PlyFormat( void );

    virtual bool LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream ) override;
    virtual bool SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream ) override;

private:

    typedef std::vector< std::string > StringArray;
    typedef std::list< StringArray* > LineList;

    void AddVertex( TriangleMesh& triangleMesh, const LineList::iterator& headerIter, const LineList::iterator& bodyIter );
    void AddTriangles( TriangleMesh& triangleMesh, const LineList::iterator& headerIter, const LineList::iterator& bodyIter );

    LineList* TokenizeFile( std::istream& stream );
};

// FileFormat.h