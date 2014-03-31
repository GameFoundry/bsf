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
			CM::HString numAllocs;
			CM::HString numFrees;
			CM::HString avgTime;
			CM::HString totalTime;
			CM::HString avgTimeSelf;
			CM::HString totalTimeSelf;
		};

		struct PreciseRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			CM::Vector<GUIElement*>::type elements;

			CM::HString name;
			CM::HString pctOfParent;
			CM::HString numCalls;
			CM::HString numAllocs;
			CM::HString numFrees;
			CM::HString avgCycles;
			CM::HString totalCycles;
			CM::HString avgCyclesSelf;
			CM::HString totalCyclesSelf;
		};

	public:
		ProfilerOverlay() {} // For Module purposes only
		ProfilerOverlay(const CM::ViewportPtr& target);
		~ProfilerOverlay();

		void setTarget(const CM::ViewportPtr& target);

		void show();
		void hide();

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();
	private:
		static const CM::UINT32 MAX_DEPTH;

		CM::ViewportPtr mTarget;

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
		GUIElement* mTitleBasicNumAllocs;
		GUIElement* mTitleBasicNumFrees;
		GUIElement* mTitleBasicAvgTime;
		GUIElement* mTitleBasicTotalTime;
		GUIElement* mTitleBasicAvgTitleSelf;
		GUIElement* mTitleBasicTotalTimeSelf;

		GUIElement* mTitlePreciseName;
		GUIElement* mTitlePrecisePctOfParent;
		GUIElement* mTitlePreciseNumCalls;
		GUIElement* mTitlePreciseNumAllocs;
		GUIElement* mTitlePreciseNumFrees;
		GUIElement* mTitlePreciseAvgCycles;
		GUIElement* mTitlePreciseTotalCycles;
		GUIElement* mTitlePreciseAvgCyclesSelf;
		GUIElement* mTitlePreciseTotalCyclesSelf;

		CM::Vector<BasicRow>::type mBasicRows;
		CM::Vector<PreciseRow>::type mPreciseRows;

		boost::signals::connection mTargetResizedConn;
		bool mIsShown;

		void targetResized();
		void updateAreaSizes();
		void updateContents(const CM::ProfilerReport& simReport, const CM::ProfilerReport& coreReport);
	};
}