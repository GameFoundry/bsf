#include "CmEditorWindowManager.h"
#include "CmException.h"

namespace CamelotEditor
{
	void EditorWindowManager::registerWindowFactory(EditorWindowFactory* factory)
	{
		assert(factory != nullptr);

		mFactories[factory->getWindowName()] = factory;
	}

	QtEditorWindow* EditorWindowManager::create(const QString& name) const
	{
		auto iterFind = mFactories.find(name);

		if(iterFind == mFactories.end())
			CM_EXCEPT(InvalidParametersException, "Window with the name: \"" + name.toStdString() + "\" doesn't exist.");



		//QtEditorWindow* window = iterFind->second.create(parent);
	}
}