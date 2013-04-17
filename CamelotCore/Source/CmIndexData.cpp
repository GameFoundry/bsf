#include "CmIndexData.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVector3.h"
#include "CmAxisAlignedBox.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotFramework 
{
	IndexData::IndexData()
	{
		indexCount = 0;
		indexStart = 0;
	}

	IndexData::~IndexData()
	{
	}

	// Local Utility class for vertex cache optimizer
	class Triangle
    {
    public:
		enum EdgeMatchType {
			AB, BC, CA, ANY, NONE
		};

		UINT32 a, b, c;		

		inline Triangle()
		{
		}

		inline Triangle( UINT32 ta, UINT32 tb, UINT32 tc ) 
			: a( ta ), b( tb ), c( tc )
		{
		}

		inline Triangle( UINT32 t[3] )
			: a( t[0] ), b( t[1] ), c( t[2] )
		{
		}

		inline Triangle( const Triangle& t )
			: a( t.a ), b( t.b ), c( t.c )
		{
		}

		inline bool sharesEdge(const Triangle& t) const
		{
			return(	a == t.a && b == t.c ||
					a == t.b && b == t.a ||
					a == t.c && b == t.b ||
					b == t.a && c == t.c ||
					b == t.b && c == t.a ||
					b == t.c && c == t.b ||
					c == t.a && a == t.c ||
					c == t.b && a == t.a ||
					c == t.c && a == t.b );
		}

		inline bool sharesEdge(const UINT32 ea, const UINT32 eb, const Triangle& t) const
		{
			return(	ea == t.a && eb == t.c ||
					ea == t.b && eb == t.a ||
					ea == t.c && eb == t.b );	
		}

		inline bool sharesEdge(const EdgeMatchType edge, const Triangle& t) const
		{
			if (edge == AB)
				return sharesEdge(a, b, t);
			else if (edge == BC)
				return sharesEdge(b, c, t);
			else if (edge == CA)
				return sharesEdge(c, a, t);
			else
				return (edge == ANY) == sharesEdge(t);
		}

		inline EdgeMatchType endoSharedEdge(const Triangle& t) const
		{
			if (sharesEdge(a, b, t)) return AB;
			if (sharesEdge(b, c, t)) return BC;
			if (sharesEdge(c, a, t)) return CA;
			return NONE;
		}

		inline EdgeMatchType exoSharedEdge(const Triangle& t) const
		{
			return t.endoSharedEdge(*this);
		}

		inline void shiftClockwise()
		{
			UINT32 t = a;
			a = c;
			c = b;
			b = t;
		}

		inline void shiftCounterClockwise()
		{
			UINT32 t = a;
			a = b;
			b = c;
			c = t;
		}
	};

	void IndexData::optimiseVertexCacheTriList(void)
	{
		if (indexBuffer->isLocked()) return;

		void *buffer = indexBuffer->lock(GBL_READ_WRITE);

		Triangle* triangles;
		UINT32 *dest;

		UINT32 nIndexes = indexCount;
		UINT32 nTriangles = nIndexes / 3;
		UINT32 i, j;
		UINT16 *source = 0;

		if (indexBuffer->getType() == IndexBuffer::IT_16BIT)
		{
			triangles = (Triangle*) malloc(sizeof(Triangle) * nTriangles);
			source = (UINT16 *)buffer;
			dest = (UINT32 *)triangles;
			for (i = 0; i < nIndexes; ++i) dest[i] = source[i];
		}
		else
			triangles = (Triangle*)buffer;

		// sort triangles based on shared edges
		UINT32 *destlist = (UINT32*)malloc(sizeof(UINT32) * nTriangles);
		unsigned char *visited = (unsigned char*)malloc(sizeof(unsigned char) * nTriangles);

		for (i = 0; i < nTriangles; ++i) visited[i] = 0;

		UINT32 start = 0, ti = 0, destcount = 0;

		bool found = false;
		for (i = 0; i < nTriangles; ++i)
		{
			if (found)
				found = false;
			else
			{
				while (visited[start++]);
				ti = start - 1;
			}

			destlist[destcount++] = ti;
			visited[ti] = 1;

			for (j = start; j < nTriangles; ++j)
			{
				if (visited[j]) continue;
				
				if (triangles[ti].sharesEdge(triangles[j]))
				{
					found = true;
					ti = static_cast<UINT32>(j);
					break;
				}
			}
		}

		if (indexBuffer->getType() == IndexBuffer::IT_16BIT)
		{
			// reorder the indexbuffer
			j = 0;
			for (i = 0; i < nTriangles; ++i)
			{
				Triangle *t = &triangles[destlist[i]];
				source[j++] = (UINT16)t->a;
				source[j++] = (UINT16)t->b;
				source[j++] = (UINT16)t->c;
			}
			free(triangles);
		}
		else
		{
			UINT32 *reflist = (UINT32*)malloc(sizeof(UINT32) * nTriangles);

			// fill the referencebuffer
			for (i = 0; i < nTriangles; ++i)
				reflist[destlist[i]] = static_cast<UINT32>(i);
			
			// reorder the indexbuffer
			for (i = 0; i < nTriangles; ++i)
			{
				j = destlist[i];
				if (i == j) continue; // do not move triangle

				// swap triangles

				Triangle t = triangles[i];
				triangles[i] = triangles[j];
				triangles[j] = t;

				// change reference
				destlist[reflist[i]] = static_cast<UINT32>(j);
				// destlist[i] = i; // not needed, it will not be used
			}

			free(reflist);
		}

		free(destlist);
		free(visited);
					
		indexBuffer->unlock();
	}
}
