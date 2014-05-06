#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class BS_EXPORT ProfilerOverlay : public Module<ProfilerOverlay>
	{
	public:
		struct BasicRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			Vector<GUIElement*>::type elements;

			HString name;
			HString pctOfParent;
			HString numCalls;
			HString numAllocs;
			HString numFrees;
			HString avgTime;
			HString totalTime;
			HString avgTimeSelf;
			HString totalTimeSelf;
		};

		struct PreciseRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			Vector<GUIElement*>::type elements;

			HString name;
			HString pctOfParent;
			HString numCalls;
			HString numAllocs;
			HString numFrees;
			HString avgCycles;
			HString totalCycles;
			HString avgCyclesSelf;
			HString totalCyclesSelf;
		};

	public:
		ProfilerOverlay(const ViewportPtr& target);
		~ProfilerOverlay();

		void setTarget(const ViewportPtr& target);

		void show();
		void hide();

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();
	private:
		static const UINT32 MAX_DEPTH;

		ViewportPtr mTarget;

		HSceneObject mWidgetSO;
		GameObjectHandle<GUIWidget> mWidget;
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

		Vector<BasicRow>::type mBasicRows;
		Vector<PreciseRow>::type mPreciseRows;

		HEvent mTargetResizedConn;
		bool mIsShown;

		void targetResized();
		void updateAreaSizes();
		void updateContents(const ProfilerReport& simReport, const ProfilerReport& coreReport);
	};
}