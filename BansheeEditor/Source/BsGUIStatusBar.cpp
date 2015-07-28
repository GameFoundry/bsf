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

		GUILayoutX* horzLayout = mPanel->addNewElement<GUILayoutX>();
		horzLayout->addNewElement<GUIFixedSpace>(10);
		horzLayout->addElement(mMessage);
		horzLayout->addNewElement<GUIFixedSpace>(20);

		mBgPanel->addElement(mBackground);

		mLogEntryAddedConn = gDebug().getLog().onEntryAdded.connect(std::bind(&GUIStatusBar::logEntryAdded, this, _1));
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

		UINT32 logChannel = entry.getChannel();
		switch (logChannel)
		{
		case (UINT32)DebugChannel::Info:
		case (UINT32)DebugChannel::Debug:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Info);
			break;
		case (UINT32)DebugChannel::Warning:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Warning);
			break;
		case (UINT32)DebugChannel::Error:
			iconTexture = BuiltinEditorResources::instance().getLogMessageIcon(LogMessageIcon::Error);
			break;
		}

		GUIContent messageContent(HString(toWString(entry.getMessage())), iconTexture);
		mMessage->setContent(messageContent);
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