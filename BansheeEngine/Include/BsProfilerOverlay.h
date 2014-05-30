#pragma once

#include "BsPrerequisites.h"
#include "BsProfilerGPU.h"
#include "BsModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	enum class ProfilerOverlayType
	{
		CPUSamples,
		GPUSamples
	};

	class BS_EXPORT ProfilerOverlay : public Module<ProfilerOverlay>
	{
	public:
		struct BasicRow
		{
			GUILayout* labelLayout;
			GUILayout* contentLayout;

			Vector<GUIElement*> elements;

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

			Vector<GUIElement*> elements;

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

		struct GPUSampleRow
		{
			GUILayout* layout;

			Vector<GUIElement*> elements;

			HString name;
			HString time;
		};

	public:
		ProfilerOverlay(const ViewportPtr& target);
		~ProfilerOverlay();

		void setTarget(const ViewportPtr& target);

		void show(ProfilerOverlayType type);
		void hide();

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();
	private:
		static const UINT32 MAX_DEPTH;

		ProfilerOverlayType mType;
		ViewportPtr mTarget;

		HSceneObject mWidgetSO;
		GameObjectHandle<GUIWidget> mWidget;
		GUIArea* mCPUBasicAreaLabels = nullptr;
		GUIArea* mCPUPreciseAreaLabels = nullptr;
		GUIArea* mCPUBasicAreaContents = nullptr;
		GUIArea* mCPUPreciseAreaContents = nullptr;

		GUILayout* mBasicLayoutLabels = nullptr;
		GUILayout* mPreciseLayoutLabels = nullptr;
		GUILayout* mBasicLayoutContents = nullptr;
		GUILayout* mPreciseLayoutContents = nullptr;

		GUIElement* mTitleBasicName = nullptr;
		GUIElement* mTitleBasicPctOfParent = nullptr;
		GUIElement* mTitleBasicNumCalls = nullptr;
		GUIElement* mTitleBasicNumAllocs = nullptr;
		GUIElement* mTitleBasicNumFrees = nullptr;
		GUIElement* mTitleBasicAvgTime = nullptr;
		GUIElement* mTitleBasicTotalTime = nullptr;
		GUIElement* mTitleBasicAvgTitleSelf = nullptr;
		GUIElement* mTitleBasicTotalTimeSelf = nullptr;

		GUIElement* mTitlePreciseName = nullptr;
		GUIElement* mTitlePrecisePctOfParent = nullptr;
		GUIElement* mTitlePreciseNumCalls = nullptr;
		GUIElement* mTitlePreciseNumAllocs = nullptr;
		GUIElement* mTitlePreciseNumFrees = nullptr;
		GUIElement* mTitlePreciseAvgCycles = nullptr;
		GUIElement* mTitlePreciseTotalCycles = nullptr;
		GUIElement* mTitlePreciseAvgCyclesSelf = nullptr;
		GUIElement* mTitlePreciseTotalCyclesSelf = nullptr;

		GUIArea* mGPUAreaFrameContents = nullptr;
		GUIArea* mGPUAreaFrameSamples = nullptr;
		GUILayout* mGPULayoutFrameContentsLeft = nullptr;
		GUILayout* mGPULayoutFrameContentsRight = nullptr;
		GUILayout* mGPULayoutSamples = nullptr;

		HString mGPUFrameNumStr;
		HString mGPUTimeStr;
		HString mGPUDrawCallsStr;
		HString mGPURenTargetChangesStr;
		HString mGPUPresentsStr;
		HString mGPUClearsStr;
		HString mGPUVerticesStr;
		HString mGPUPrimitivesStr;
		HString mGPUSamplesStr;
		HString mGPUBlendStateChangesStr;
		HString mGPURasterStateChangesStr;
		HString mGPUDepthStencilStateChangesStr;

		HString mGPUObjectsCreatedStr;
		HString mGPUObjectsDestroyedStr;
		HString mGPUResourceWritesStr;
		HString mGPUResourceReadsStr;
		HString mGPUTextureBindsStr;
		HString mGPUSamplerBindsStr;
		HString mGPUVertexBufferBindsStr;
		HString mGPUIndexBufferBindsStr;
		HString mGPUGPUProgramBufferBindsStr;
		HString mGPUGPUProgramBindsStr;

		Vector<BasicRow> mBasicRows;
		Vector<PreciseRow> mPreciseRows;
		Vector<GPUSampleRow> mGPUSampleRows;

		HEvent mTargetResizedConn;
		bool mIsShown;

		void targetResized();
		void updateCPUSampleAreaSizes();
		void updateGPUSampleAreaSizes();

		void updateCPUSampleContents(const ProfilerReport& simReport, const ProfilerReport& coreReport);
		void updateGPUSampleContents(const GPUProfilerReport& gpuReport);
	};
}