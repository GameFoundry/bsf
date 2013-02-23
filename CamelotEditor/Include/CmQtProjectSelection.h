#pragma once

#include <QtWidgets/QWidget>

namespace CamelotEditor
{
	class QtProjectSelection : public QWidget
	{
		Q_OBJECT

	public:
		QtProjectSelection(QWidget *parent = 0);
		~QtProjectSelection();

	private:
		void setupUi();
		void retranslateUi();
	};
}