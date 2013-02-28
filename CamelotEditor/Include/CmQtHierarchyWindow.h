#pragma once

#include "CmEditorPrefs.h"
#include "CmQtEditorWindow.h"

namespace CamelotEditor
{
	class QtHierarchyWindow: public QtEditorWindow
	{
		Q_OBJECT
	public:
		QtHierarchyWindow(QWidget* parent);
		virtual ~QtHierarchyWindow();
	};
}