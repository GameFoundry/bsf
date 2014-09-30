#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSlider
	{
	public:
		enum class State
		{
			Inactive,
			Active,
			Hover
		};

		HandleSlider(bool fixedScale, float snapValue);
		virtual ~HandleSlider() { }

		State getState() const { return mState; }
		bool getFixedScale() const { return mFixedScale; }
		float getSnapValue() const { return mSnapValue; }

		virtual void setCustomTransform(const Matrix4& transform) { mCustomTransform = transform; }
		virtual const Matrix4& getCustomTransform() const { return mCustomTransform; }

	protected:
		friend class HandleManager;

		void setInactive() { mState = State::Inactive; }
		void setActive(const Vector2I& pointerPos) { mState = State::Active; mLastPointerPos = pointerPos; }
		void setHover() { mState = State::Hover; }
		void registerDrag(const Vector2I& pointerPos);

		bool mFixedScale;
		float mSnapValue;
		Matrix4 mCustomTransform;

		Vector2I mLastPointerPos;
		Vector2I mCurPointerPos;
		State mState;
	};
}