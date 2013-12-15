#include "CmRect.h"
#include "CmInt2.h"
#include "CmMatrix4.h"
#include "CmMath.h"

namespace CamelotFramework
{
	const Rect Rect::EMPTY = Rect();

	Rect::Rect()
		:x(0), y(0), width(0), height(0)
	{ }

	Rect::Rect(int _x, int _y, int _width, int _height)
		:x(_x), y(_y), width(_width), height(_height)
	{ }

	bool Rect::contains(const Int2& point) const
	{
		if(point.x >= x && point.x <= (x + width))
		{
			if(point.y >= y && point.y <= (y + height))
				return true;
		}

		return false;
	}

	bool Rect::overlaps(const Rect& other) const
	{
		INT32 otherRight = other.x + other.width;
		INT32 myRight = x + width;

		INT32 otherBottom = other.y + other.height;
		INT32 myBottom = y + height;

		if(x < otherRight && myRight > other.x &&
			y < otherBottom && myBottom > other.y)
			return true;

		return false;
	}

	void Rect::encapsulate(const Rect& other)
	{
		int myRight = x + width;
		int myBottom = y + height;
		int otherRight = other.x + other.width;
		int otherBottom = other.y + other.height;

		if(other.x < x)
			x = other.x;

		if(other.y < y)
			y = other.y;

		if(otherRight > myRight)
			width = otherRight - x;
		else
			width = myRight - x;

		if(otherBottom > myBottom)
			height = otherBottom - y;
		else
			height = myBottom - y;
	}

	void Rect::clip(const Rect& clipRect)
	{
		int newLeft = std::max(x, clipRect.x);
		int newTop = std::max(y, clipRect.y);

		int newRight = std::min(x + width, clipRect.x + clipRect.width);
		int newBottom = std::min(y + height, clipRect.y + clipRect.height);

		x = newLeft;
		y = newTop;
		width = newRight - newLeft;
		height = newBottom - newTop;
	}

	void Rect::transform(const Matrix4& matrix)
	{
		Vector4 verts[4];
		verts[0] = Vector4((float)x, (float)y, 0.0f, 1.0f);
		verts[1] = Vector4((float)x + width, (float)y, 0.0f, 1.0f);
		verts[2] = Vector4((float)x, (float)y + height, 0.0f, 1.0f);
		verts[3] = Vector4((float)x + width, (float)y + height, 0.0f, 1.0f);

		for(UINT32 i = 0; i < 4; i++)
			verts[i] = matrix.transform(verts[i]);

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();

		for(UINT32 i = 0; i < 4; i++)
		{
			if(verts[i].x < minX)
				minX = verts[i].x;

			if(verts[i].y < minY)
				minY = verts[i].y;

			if(verts[i].x > maxX)
				maxX = verts[i].x;

			if(verts[i].y > maxY)
				maxY = verts[i].y;
		}

		x = Math::FloorToInt(minX);
		y = Math::FloorToInt(minY);
		width = Math::CeilToInt(maxX) - x;
		height = Math::CeilToInt(maxY) - y;
	}
}