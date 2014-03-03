#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidgetManager.h"
#include "boost/signal.hpp"

namespace BansheeEditor
{
	class EditorWidgetBase
	{
	public:
		virtual void initialize() { }

		const CM::String& getName() const { return mName; }
		const CM::HString& getDisplayName() const { return mDisplayName; }

		void _setSize(CM::UINT32 width, CM::UINT32 height);
		void _setPosition(CM::INT32 x, CM::INT32 y);
		void _changeParent(EditorWidgetContainer* parent);
		EditorWidgetContainer* _getParent() const { return mParent; }

		void _disable();
		void _enable();

		void close();
	protected:
		friend class EditorWidgetManager;

		EditorWidgetBase(const CM::HString& displayName, const CM::String& name);
		virtual ~EditorWidgetBase();

		CM::String mName;
		CM::HString mDisplayName;
		EditorWidgetContainer* mParent;
		BS::GUIArea* mContent;

		BS::GUIWidget& getParentWidget() const;

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

		static EditorWidgetBase* create()
		{
			return cm_new<Type>(EditorWidget<Type>::ConstructPrivately());
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

		EditorWidget(const CM::HString& displayName)
			:EditorWidgetBase(displayName, Type::getTypeName())
		{
			RegisterOnStart.makeSureIAmInstantiated();
		}

	public:

		virtual ~EditorWidget() { }
	};

	template <typename Type>
	RegisterWidgetOnStart<Type> EditorWidget<Type>::RegisterOnStart;
}