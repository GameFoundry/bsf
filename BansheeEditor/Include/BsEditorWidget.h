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

		void _setSize(UINT32 width, UINT32 height);
		void _setPosition(INT32 x, INT32 y);
		void _changeParent(EditorWidgetContainer* parent);
		EditorWidgetContainer* _getParent() const { return mParent; }

		void _disable();
		void _enable();

		void close();

		virtual void _update() { }

		Event<void(UINT32, UINT32)> onResized;
		Event<void(INT32, INT32)> onMoved;
		Event<void(EditorWidgetContainer*)> onParentChanged;
	protected:
		friend class EditorWidgetManager;

		EditorWidgetBase(const HString& displayName, const String& name, EditorWidgetContainer& parentContainer);
		virtual ~EditorWidgetBase();

		String mName;
		HString mDisplayName;
		EditorWidgetContainer* mParent;
		INT32 mX, mY;
		UINT32 mWidth, mHeight;
		GUIArea* mContent;

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