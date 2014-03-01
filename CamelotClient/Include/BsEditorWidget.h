#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidgetManager.h"
#include "boost/signal.hpp"

namespace BansheeEditor
{
	class EditorWidgetBase
	{
	public:
		virtual ~EditorWidgetBase();

		virtual void initialize() { }

		const CM::HString& getName() const { return mName; }

		void _setSize(CM::UINT32 width, CM::UINT32 height);
		void _setPosition(CM::INT32 x, CM::INT32 y);
		void _changeParent(EditorWidgetContainer* parent);

		void _disable();
		void _enable();

		static void destroy(EditorWidgetBase* widget);
	protected:
		friend class EditorWidgetManager;

		EditorWidgetBase(const CM::HString& name);

		CM::HString mName;
		EditorWidgetContainer* mParent;
		BS::GUIArea* mContent;

		BS::GUIWidget& getParentWidget() const;
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

		EditorWidget(const CM::HString& name)
			:EditorWidgetBase(name)
		{
			RegisterOnStart.makeSureIAmInstantiated();
		}

	public:

		virtual ~EditorWidget() { }
	};

	template <typename Type>
	RegisterWidgetOnStart<Type> EditorWidget<Type>::RegisterOnStart;
}