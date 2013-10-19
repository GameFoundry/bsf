#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "boost/signals/connection.hpp"

namespace BansheeEngine
{
	class BS_EXPORT ProfilerOverlay : public CM::Module<ProfilerOverlay>
	{
	public:
		struct BasicRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			CM::Vector<GUIElement*>::type elements;

			CM::HString name;
			CM::HString pctOfParent;
			CM::HString numCalls;
			CM::HString avgTime;
			CM::HString totalTime;
			CM::HString maxTime;
			CM::HString avgTimeSelf;
			CM::HString totalTimeSelf;
			CM::HString estOverhead;
			CM::HString estOverheadSelf;
		};

		struct PreciseRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			CM::Vector<GUIElement*>::type elements;

			CM::HString name;
			CM::HString pctOfParent;
			CM::HString numCalls;
			CM::HString avgCycles;
			CM::HString totalCycles;
			CM::HString maxCycles;
			CM::HString avgCyclesSelf;
			CM::HString totalCyclesSelf;
			CM::HString estOverhead;
			CM::HString estOverheadSelf;
		};

	public:
		ProfilerOverlay(const CM::ViewportPtr& target, const CM::RenderWindowPtr& ownerWindow);
		~ProfilerOverlay();

		void setTarget(const CM::ViewportPtr& target, const CM::RenderWindowPtr& ownerWindow);

		void show();
		void hide();

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();
	private:
		static const CM::UINT32 MAX_DEPTH;

		CM::ViewportPtr mTarget;
		CM::RenderWindowPtr mOwnerWindow;

		CM::HSceneObject mWidgetSO;
		CM::GameObjectHandle<GUIWidget> mWidget;
		GUIArea* mBasicAreaLabels;
		GUIArea* mPreciseAreaLabels;
		GUIArea* mBasicAreaContents;
		GUIArea* mPreciseAreaContents;

		GUILayout* mBasicLayoutLabels;
		GUILayout* mPreciseLayoutLabels;
		GUILayout* mBasicLayoutContents;
		GUILayout* mPreciseLayoutContents;

		GUIElement* mTitleBasicName;
		GUIElement* mTitleBasicPctOfParent;
		GUIElement* mTitleBasicNumCalls;
		GUIElement* mTitleBasicAvgTime;
		GUIElement* mTitleBasicTotalTime;
		GUIElement* mTitleBasicMaxTime;
		GUIElement* mTitleBasicAvgTitleSelf;
		GUIElement* mTitleBasicTotalTimeSelf;
		GUIElement* mTitleBasicEstOverhead;
		GUIElement* mTitleBasicEstOverheadSelf;

		GUIElement* mTitlePreciseName;
		GUIElement* mTitlePrecisePctOfParent;
		GUIElement* mTitlePreciseNumCalls;
		GUIElement* mTitlePreciseAvgTime;
		GUIElement* mTitlePreciseTotalTime;
		GUIElement* mTitlePreciseMaxTime;
		GUIElement* mTitlePreciseAvgTitleSelf;
		GUIElement* mTitlePreciseTotalTimeSelf;
		GUIElement* mTitlePreciseEstOverhead;
		GUIElement* mTitlePreciseEstOverheadSelf;

		CM::Vector<BasicRow>::type mBasicRows;
		CM::Vector<PreciseRow>::type mPreciseRows;

		boost::signals::connection mWindowMovedOrResized;
		bool mIsShown;

		void windowMovedOrResized(CM::RenderWindow& window);
		void updateAreaSizes();
		void updateContents(const CM::ProfilerReport& report);
	};
}