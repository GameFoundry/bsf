//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsProfilerGPU.h"
#include "BsModule.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of profiler overlay.
	 */
	enum class ProfilerOverlayType
	{
		CPUSamples,
		GPUSamples
	};

	/**
	 * @brief	Handles rendering of Profiler information as an overlay in a viewport.
	 */
	class BS_EXPORT ProfilerOverlay : public Component
	{
	public:
		/**
		 * @brief	Holds data about GUI elements in a single row of a "CPU basic" sample.
		 */
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

		/**
		 * @brief	Holds data about GUI elements in a single row of a "CPU precise" sample.
		 */
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
		
		/**
		 * @brief	Holds data about GUI elements in a single row of a GPU sample.
		 */
		struct GPUSampleRow
		{
			GUILayout* layout;

			Vector<GUIElement*> elements;

			HString name;
			HString time;
		};

	public:
		/**
		 * @brief	Constructs a new overlay attached to the specified parent and displayed on the provided viewport.
		 */
		ProfilerOverlay(const HSceneObject& parent, const ViewportPtr& target);
		~ProfilerOverlay();

		/**
		 * @brief	Changes the viewport to display the overlay on.
		 */
		void setTarget(const ViewportPtr& target);

		/**
		 * @brief	Shows the overlay of the specified type.
		 */
		void show(ProfilerOverlayType type);

		/**
		 * @brief	Hides the overlay.
		 */
		void hide();

		/**
		 * @copydoc	Component::update
		 */
		void update();
	private:
		/**
		 * @brief	Called whenever the viewport resizes in order to rearrange the GUI elements.
		 */
		void targetResized();

		/**
		 * @brief	Updates sizes of GUI areas used for displaying CPU sample data. To be called
		 *			after viewport change or resize.
		 */
		void updateCPUSampleAreaSizes();

		/**
		 * @brief	Updates sizes of GUI areas used for displaying GPU sample data. To be called
		 *			after viewport change or resize.
		 */
		void updateGPUSampleAreaSizes();

		/**
		 * @brief	Updates CPU GUI elements from the data in the provided profiler reports. To be called
		 *			whenever a new report is received.
		 */
		void updateCPUSampleContents(const ProfilerReport& simReport, const ProfilerReport& coreReport);

		/**
		 * @brief	Updates GPU GUI elemnts from the data in the provided profiler report. To be called whenever
		 *			a new report is received.
		 */
		void updateGPUSampleContents(const GPUProfilerReport& gpuReport);

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
	};
}