#pragma once

#include "CmEditorPrefs.h"
#include "CmQtEditorWindow.h"

namespace CamelotEditor
{
	class QtSceneWindow: public QtEditorWindow
	{
		Q_OBJECT
	public:
		QtSceneWindow(QWidget* parent);
		virtual ~QtSceneWindow();
	};
}