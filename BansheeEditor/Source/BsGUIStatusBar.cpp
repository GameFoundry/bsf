#include "BsGUIStatusBar.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsGUIPanel.h"
#include "BsGUISpace.h"
#include "BsDebug.h"
#include "BsBuiltinEditorResources.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const Color GUIStatusBar::COLOR_INFO = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color GUIStatusBar::COLOR_WARNING = Color(192 / 255.0f, 176 / 255.0f, 0.0f, 1.0f);
	const Color GUIStatusBar::COLOR_ERROR = Color(192 / 255.0f, 36 / 255.0f, 0.0f, 1.0f);

	GUIStatusBar::GUIStatusBar(const PrivatelyConstruct& dummy,
		const String& style, const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions, style)
	{
		mPanel = GUIPanel::create();
		mBgPanel = GUIPanel::create(1);
		_registerChildElement(mPanel);
		_registerChildElement(mBgPanel);

		mBackground = GUITexture::create(GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getGUIBackgroundTypeName()));
		mMessage = GUIButton::create(HString(L""), GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getGUIMessageTypeName()));
		mScene = GUILabel::create(HString(L"Scene: None"), GUIOptions(GUIOption::fixedWidth(150)));
		mProject = GUILabel::create(HString(L"Project: None"), GUIOptions(GUIOption::fixedWidth(150)));

		GUILayoutY* vertLayout = mPanel->addNewElement<GUILayoutY>();
		vertLayout->addNewElement<GUIFixedSpace>(3);
		GUILayoutX* horzLayout = vertLayout->addNewElement<GUILayoutX>();

		horzLayout->addNewElement<GUIFixedSpace>(10);
		horzLayout->addElement(mMessage);
		horzLayout->addNewElement<GUIFixedSpace>(20);
		horzLayout->addNewElement<GUIFixedSpace>(20);

		mBgPanel->addElement(mBackground);

		mLogEntryAddedConn = gDebug().onLogEntryAdded.connect(std::bind(&GUIStatusBar::logEntryAdded, this, _1));
		mMessageBtnPressedConn = mMessage->onClick.connect(std::bind(&GUIStatusBar::messageBtnClicked, this));
	}

	GUIStatusBar::~GUIStatusBar()
	{
		mLogEntryAddedConn.disconnect();
		mMessageBtnPressedConn.disconnect();
	}

	GUIStatusBar* GUIStatusBar::create(const GUIOptions& options, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIStatusBar>(PrivatelyConstruct(), *curStyle, GUIDimensions::create(options));
	}

	GUIStatusBar* GUIStatusBar::create(const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIStatusBar>(PrivatelyConstruct(), *curStyle, GUIDimensions::create());
	}

	void GUIStatusBar::setProject(const WString& name, bool modified)
	{
		WString content = L"Project: " + name;

		if (modified)
			content += L"*";

		mProject->setContent(HString(content));
	}

	void GUIStatusBar::setScene(const WString& name, bool modified)
	{
		WString content = L"Scene: " + name;

		if (modified)
			content += L"*";

		mScene->setContent(HString(content));
	}
	
	void GUIStatusBar::setTint(const Color& color)
	{
		mBackground->setTint(color);
		mMessage->setTint(color);
	}

	void GUIStatusBar::_updateLayoutInternal(const GUILayoutData& data)
	{
		mPanel->_setLayoutData(data);
		mPanel->_updateLayoutInternal(data);

		mBgPanel->_setLayoutData(data);
		mBgPanel->_updateLayoutInternal(data);
	}

	Vector2I GUIStatusBar::_getOptimalSize() const
	{
		return mBgPanel->_getOptimalSize();
	}
	
	void GUIStatusBar::styleUpdated()
	{
		mBackground->setStyle(getSubStyleName(getGUIBackgroundTypeName()));
		mMessage->setStyle(getSubStyleName(getGUIMessageTypeName()));
	}

	void GUIStatusBar::logEntryAdded(const LogEntry& entry)
	{
		HSpriteTexture iconTexture;
		Color textColor = COLOR_INFO;

		UINT32 logChannel = entry.getChannel();
		switch (logChannel)
		{
		case (UINT32)DebugChannel::Debug:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Info);
			break;
		case (UINT32)DebugChannel::Warning:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Warning);
			textColor = COLOR_WARNING;
			break;
		case (UINT32)DebugChannel::Error:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Error);
			textColor = COLOR_ERROR;
			break;
		}

		WString message = toWString(entry.getMessage());
		WString::size_type lfPos = message.find_first_of('\n');
		WString::size_type crPos = message.find_first_of('\r');
		WString::size_type newlinePos;

		if (lfPos >= 0)
		{
			if (crPos >= 0)
				newlinePos = std::min(lfPos, crPos);
			else
				newlinePos = lfPos;
		}
		else if (crPos >= 0)
			newlinePos = crPos;
		else
			newlinePos = -1;

		if (newlinePos == -1)
		{
			GUIContent messageContent(HString(message), iconTexture);
			mMessage->setContent(messageContent);
			mMessage->setTint(textColor);
		}
		else
		{
			WString firstLine = message.substr(0, newlinePos);

			GUIContent messageContent(HString(firstLine), iconTexture);
			mMessage->setContent(messageContent);
			mMessage->setTint(textColor);
		}		
	}

	void GUIStatusBar::messageBtnClicked()
	{
		onMessageClicked();
	}

	const String& GUIStatusBar::getGUITypeName()
	{
		static String TypeName = "GUIStatusBar";
		return TypeName;
	}

	const String& GUIStatusBar::getGUIBackgroundTypeName()
	{
		static String TypeName = "GUIStatusBarBg";
		return TypeName;
	}

	const String& GUIStatusBar::getGUIMessageTypeName()
	{
		static String TypeName = "GUIStatusBarMessage";
		return TypeName;
	}
}