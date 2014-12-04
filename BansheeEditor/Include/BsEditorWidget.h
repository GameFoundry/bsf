#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidgetManager.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorWidgetBase
	{
	public:
		const String& getName() const { return mName; }
		const HString& getDisplayName() const { return mDisplayName; }

		INT32 getX() const { return mX; }
		INT32 getY() const { return mY; }
		UINT32 getWidth() const { return mWidth; }
		UINT32 getHeight() const { return mHeight; }
		bool hasFocus() const { return mHasFocus; }

		void _setSize(UINT32 width, UINT32 height);
		void _setPosition(INT32 x, INT32 y);
		void _changeParent(EditorWidgetContainer* parent);
		void _setHasFocus(bool focus) { mHasFocus = focus; }
		EditorWidgetContainer* _getParent() const { return mParent; }

		Vector2I screenToWidgetPos(const Vector2I& screenPos) const;
		Vector2I widgetToScreenPos(const Vector2I& widgetPos) const;

		void _disable();
		void _enable();

		void close();

		virtual void update() { }

		Event<void(UINT32, UINT32)> onResized;
		Event<void(INT32, INT32)> onMoved;
		Event<void(EditorWidgetContainer*)> onParentChanged;
	protected:
		friend class EditorWidgetManager;

		EditorWidgetBase(const HString& displayName, const String& name, EditorWidgetContainer& parentContainer);
		virtual ~EditorWidgetBase();

		virtual void doOnMoved(INT32 x, INT32 y);
		virtual void doOnResized(UINT32 width, UINT32 height);
		virtual void doOnParentChanged();

		String mName;
		HString mDisplayName;
		EditorWidgetContainer* mParent;
		INT32 mX, mY;
		UINT32 mWidth, mHeight;
		GUIArea* mContent;
		bool mHasFocus;

		GUIWidget& getParentWidget() const;

		static void destroy(EditorWidgetBase* widget);
	};

	template<typename Type>
	struct RegisterWidgetOnStart
	{
	public:
		RegisterWidgetOnStart()
		{
			EditorWidgetManager::preRegisterWidget(Type::getTypeName(), &create);
		}

		static EditorWidgetBase* create(EditorWidgetContainer& parentContainer)
		{
			return bs_new<Type>(EditorWidget<Type>::ConstructPrivately(), parentContainer);
		}

		void makeSureIAmInstantiated() { }
	};

	template <class Type>
	class EditorWidget : public EditorWidgetBase
	{
		static RegisterWidgetOnStart<Type> RegisterOnStart;

	protected:
		friend struct RegisterWidgetOnStart<Type>;

		struct ConstructPrivately {};

		EditorWidget(const HString& displayName, EditorWidgetContainer& parentContainer)
			:EditorWidgetBase(displayName, Type::getTypeName(), parentContainer)
		{
			RegisterOnStart.makeSureIAmInstantiated();
		}

	public:

		virtual ~EditorWidget() { }
	};

	template <typename Type>
	RegisterWidgetOnStart<Type> EditorWidget<Type>::RegisterOnStart;
}