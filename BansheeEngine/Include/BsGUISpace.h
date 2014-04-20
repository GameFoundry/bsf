#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIFixedSpace : public GUIElementBase
	{
	public:
		GUIFixedSpace(UINT32 size)
			:mSize(size)
		{
			_markAsClean();
		}

		UINT32 getSize() const { return mSize; }
		Type _getType() const { return GUIElementBase::Type::FixedSpace; }

		virtual Vector2I _getOptimalSize() const { return Vector2I(getSize(), getSize()); }
		virtual const RectOffset& _getPadding() const 
		{
			static RectOffset padding;

			return padding;
		}

	protected:
		UINT32 mSize;
	};

	class BS_EXPORT GUIFlexibleSpace : public GUIElementBase
	{
	public:
		GUIFlexibleSpace() 
		{
			_markAsClean();
		}

		Type _getType() const { return GUIElementBase::Type::FlexibleSpace; }

		virtual Vector2I _getOptimalSize() const { return Vector2I(0, 0); }
		virtual const RectOffset& _getPadding() const 
		{
			static RectOffset padding;

			return padding;
		}
	};
}