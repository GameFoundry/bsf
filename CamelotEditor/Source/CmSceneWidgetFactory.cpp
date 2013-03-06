#include "CmSceneWidgetFactory.h"
#include "CmQtSceneWidget.h"

namespace CamelotEditor
{
	QtEditorWidget* SceneWidgetFactory::create(QWidget* parent)
	{
		return new QtSceneWidget(parent);
	}

	const QString& SceneWidgetFactory::getWindowName() const
	{
		static QString name = "Scene";
		return name;
	}

	const QString& SceneWidgetFactory::getMenuCategory() const
	{
		static QString name = "Windows";
		return name;
	}

	const QString& SceneWidgetFactory::getMenuItemName() const
	{
		static QString name = "Scene";
		return name;
	}
}