#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include "CmEditorWindowFactory.h"

namespace CamelotEditor
{
	class EditorWindowManager : public Module<EditorWindowManager>
	{		
	public:
		void registerWindowFactory(EditorWindowFactory* factory);

		QtEditorWindow* create(const QString& name) const;

	private:
		std::map<QString, EditorWindowFactory*> mFactories;
	};
}