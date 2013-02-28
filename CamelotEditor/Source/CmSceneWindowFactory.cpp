#include "CmSceneWindowFactory.h"
#include "CmQtSceneWindow.h"

namespace CamelotEditor
{
	QtEditorWindow* SceneWindowFactory::create(QWidget* parent) const
	{
		return new QtSceneWindow(parent);
	}

	const QString& SceneWindowFactory::getWindowName() const
	{
		static QString name = "Scene";
		return name;
	}

	const QString& SceneWindowFactory::getMenuCategory() const
	{
		static QString name = "Windows";
		return name;
	}

	const QString& SceneWindowFactory::getMenuItemName() const
	{
		static QString name = "Scene";
		return name;
	}
}