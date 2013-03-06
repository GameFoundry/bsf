#pragma once

#include "CmEditorPrefs.h"
#include "CmQtEditorWidget.h"

namespace CamelotEditor
{
	class QtSceneWidget: public QtEditorWidget
	{
		Q_OBJECT
	public:
		QtSceneWidget(QWidget* parent);
		virtual ~QtSceneWidget();
	};
}