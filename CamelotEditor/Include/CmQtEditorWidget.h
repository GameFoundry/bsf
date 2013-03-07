#pragma once

#include "CmEditorPrerequisites.h"
#include <QtWidgets/QWidget>
#include <boost/signal.hpp>

namespace CamelotEditor
{
	class QtEditorWidget : public QWidget
	{
		Q_OBJECT

	public:
		QtEditorWidget(QWidget* parent, const QString& name, const QString& title);
		virtual ~QtEditorWidget() { }

		const QString& getName() const { return mName; }
		const QString& getTitle() const { return mTitle; }

		QtEditorWindow* getParentWindow() const { return mParentWindow; }

		void closeWidget();

		boost::signal<void(QtEditorWidget*)> onClosed;
	private:
		friend class QtEditorWindow;
		
		QString mName;
		QString mTitle;

		QtEditorWindow* mParentWindow;

		void setParentWindow(QtEditorWindow* parentWindow) { mParentWindow = parentWindow; }
	};
}