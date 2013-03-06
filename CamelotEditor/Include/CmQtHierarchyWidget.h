#pragma once

#include "CmEditorPrefs.h"
#include "CmQtEditorWidget.h"

namespace CamelotEditor
{
	class QtHierarchyWidget: public QtEditorWidget
	{
		Q_OBJECT
	public:
		QtHierarchyWidget(QWidget* parent);
		virtual ~QtHierarchyWidget();
	};
}