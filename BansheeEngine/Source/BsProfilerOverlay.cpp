#include "BsProfilerOverlay.h"
#include "BsSceneObject.h"
#include "BsGUIWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUIElement.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsViewport.h"
#include "BsTime.h"
#include "BsBuiltinResources.h"
#include "BsProfilingManager.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{
	class BasicRowFiller
	{
	public:
		UINT32 curIdx;
		GUILayout& labelLayout;
		GUILayout& contentLayout;
		GUIWidget& widget;
		Vector<ProfilerOverlay::BasicRow>& rows;

		BasicRowFiller(Vector<ProfilerOverlay::BasicRow>& _rows, GUILayout& _labelLayout, GUILayout& _contentLayout, GUIWidget& _widget)
			:rows(_rows), curIdx(0), labelLayout(_labelLayout), contentLayout(_contentLayout), widget(_widget)
		{ }

		~BasicRowFiller()
		{
			UINT32 excessEntries = (UINT32)rows.size() - curIdx;
			for(UINT32 i = 0; i < excessEntries; i++)
			{
				ProfilerOverlay::BasicRow& row = rows[curIdx + i];

				GUILayout::destroy(row.labelLayout);
				GUILayout::destroy(row.contentLayout);
			}

			rows.resize(curIdx);
		}

		void addData(UINT32 depth, const String& name, float pctOfParent, UINT32 numCalls, UINT64 numAllocs, 
			UINT64 numFrees, double avgTime, double totalTime, double avgSelfTime, double totalSelfTime)
		{
			if(curIdx >= rows.size())
			{
				rows.push_back(ProfilerOverlay::BasicRow());

				ProfilerOverlay::BasicRow& newRow = rows.back();

				newRow.name = HString(L"{0}");
				newRow.pctOfParent = HString(L"{0} %");
				newRow.numCalls = HString(L"{0}");
				newRow.numAllocs = HString(L"{0}");
				newRow.numFrees = HString(L"{0}");
				newRow.avgTime = HString(L"{0}");
				newRow.totalTime = HString(L"{0}");
				newRow.avgTimeSelf = HString(L"{0}");
				newRow.totalTimeSelf = HString(L"{0}");

				newRow.labelLayout = labelLayout.insertNewElement<GUILayoutX>(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = contentLayout.insertNewElement<GUILayoutX>(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numCalls = GUILabel::create(newRow.numCalls, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numAllocs = GUILabel::create(newRow.numAllocs, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numFrees = GUILabel::create(newRow.numFrees, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* avgTime = GUILabel::create(newRow.avgTime, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* totalTime = GUILabel::create(newRow.totalTime, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* avgTimeSelf = GUILabel::create(newRow.avgTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalTimeSelf = GUILabel::create(newRow.totalTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelSpace = newRow.labelLayout->addNewElement<GUIFixedSpace>(0);
				newRow.labelLayout->addElement(name);

				newRow.contentLayout->addElement(pctOfParent);
				newRow.contentLayout->addElement(numCalls);
				newRow.contentLayout->addElement(numAllocs);
				newRow.contentLayout->addElement(numFrees);
				newRow.contentLayout->addElement(avgTime);
				newRow.contentLayout->addElement(totalTime);
				newRow.contentLayout->addElement(avgTimeSelf);
				newRow.contentLayout->addElement(totalTimeSelf);

				newRow.elements.push_back(name);
				newRow.elements.push_back(pctOfParent);
				newRow.elements.push_back(numCalls);
				newRow.elements.push_back(numAllocs);
				newRow.elements.push_back(numFrees);
				newRow.elements.push_back(avgTime);
				newRow.elements.push_back(totalTime);
				newRow.elements.push_back(avgTimeSelf);
				newRow.elements.push_back(totalTimeSelf);
			}
			
			ProfilerOverlay::BasicRow& row = rows[curIdx];

			GUIFixedSpace::destroy(row.labelSpace);
			row.labelSpace = row.labelLayout->insertNewElement<GUIFixedSpace>(0, depth * 20);

			row.name.setParameter(0, toWString(name));
			row.pctOfParent.setParameter(0, toWString(pctOfParent * 100.0f, 2, 0, ' ', std::ios::fixed));
			row.numCalls.setParameter(0, toWString(numCalls));
			row.numAllocs.setParameter(0, toWString(numAllocs));
			row.numFrees.setParameter(0, toWString(numFrees));
			row.avgTime.setParameter(0, toWString(avgTime, 2, 0, ' ', std::ios::fixed));
			row.totalTime.setParameter(0, toWString(totalTime, 2, 0, ' ', std::ios::fixed));
			row.avgTimeSelf.setParameter(0, toWString(avgSelfTime, 2, 0, ' ', std::ios::fixed));
			row.totalTimeSelf.setParameter(0, toWString(totalSelfTime, 2, 0, ' ', std::ios::fixed));

			curIdx++;
		}
	};

	class PreciseRowFiller
	{
	public:
		UINT32 curIdx;
		GUILayout& labelLayout;
		GUILayout& contentLayout;
		GUIWidget& widget;
		Vector<ProfilerOverlay::PreciseRow>& rows;

		PreciseRowFiller(Vector<ProfilerOverlay::PreciseRow>& _rows, GUILayout& _labelLayout, GUILayout& _contentLayout, GUIWidget& _widget)
			:rows(_rows), curIdx(0), labelLayout(_labelLayout), contentLayout(_contentLayout), widget(_widget)
		{ }

		~PreciseRowFiller()
		{
			UINT32 excessEntries = (UINT32)rows.size() - curIdx;
			for(UINT32 i = 0; i < excessEntries; i++)
			{
				ProfilerOverlay::PreciseRow& row = rows[curIdx + i];

				GUILayout::destroy(row.labelLayout);
				GUILayout::destroy(row.contentLayout);
			}

			rows.resize(curIdx);
		}

		void addData(UINT32 depth, const String& name, float pctOfParent, UINT32 numCalls, UINT64 numAllocs, 
			UINT64 numFrees, UINT64 avgCycles, UINT64 totalCycles, UINT64 avgSelfCycles, UINT64 totalSelfCycles)
		{
			if(curIdx >= rows.size())
			{
				rows.push_back(ProfilerOverlay::PreciseRow());

				ProfilerOverlay::PreciseRow& newRow = rows.back();

				newRow.name = HString(L"{0}");
				newRow.pctOfParent = HString(L"{0}");
				newRow.numCalls = HString(L"{0}");
				newRow.numAllocs = HString(L"{0}");
				newRow.numFrees = HString(L"{0}");
				newRow.avgCycles = HString(L"{0}");
				newRow.totalCycles = HString(L"{0}");
				newRow.avgCyclesSelf = HString(L"{0}");
				newRow.totalCyclesSelf = HString(L"{0}");

				newRow.labelLayout = labelLayout.insertNewElement<GUILayoutX>(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = contentLayout.insertNewElement<GUILayoutX>(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numCalls = GUILabel::create(newRow.numCalls, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numAllocs = GUILabel::create(newRow.numAllocs, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numFrees = GUILabel::create(newRow.numFrees, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* avgCycles = GUILabel::create(newRow.avgCycles,GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* totalCycles = GUILabel::create(newRow.totalCycles, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* avgCyclesSelf = GUILabel::create(newRow.avgCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalCyclesSelf = GUILabel::create(newRow.totalCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelSpace = newRow.labelLayout->addNewElement<GUIFixedSpace>(0);
				newRow.labelLayout->addElement(name);

				newRow.contentLayout->addElement(pctOfParent);
				newRow.contentLayout->addElement(numCalls);
				newRow.contentLayout->addElement(numAllocs);
				newRow.contentLayout->addElement(numFrees);
				newRow.contentLayout->addElement(avgCycles);
				newRow.contentLayout->addElement(totalCycles);
				newRow.contentLayout->addElement(avgCyclesSelf);
				newRow.contentLayout->addElement(totalCyclesSelf);

				newRow.elements.push_back(name);
				newRow.elements.push_back(pctOfParent);
				newRow.elements.push_back(numCalls);
				newRow.elements.push_back(numAllocs);
				newRow.elements.push_back(numFrees);
				newRow.elements.push_back(avgCycles);
				newRow.elements.push_back(totalCycles);
				newRow.elements.push_back(avgCyclesSelf);
				newRow.elements.push_back(totalCyclesSelf);
			}

			ProfilerOverlay::PreciseRow& row = rows[curIdx];

			GUIFixedSpace::destroy(row.labelSpace);
			row.labelSpace = row.labelLayout->insertNewElement<GUIFixedSpace>(0, depth * 20);

			row.name.setParameter(0, toWString(name));
			row.pctOfParent.setParameter(0, toWString(pctOfParent * 100.0f, 2, 0, ' ', std::ios::fixed));
			row.numCalls.setParameter(0, toWString(numCalls));
			row.numAllocs.setParameter(0, toWString(numAllocs));
			row.numFrees.setParameter(0, toWString(numFrees));
			row.avgCycles.setParameter(0, toWString(avgCycles));
			row.totalCycles.setParameter(0, toWString(totalCycles));
			row.avgCyclesSelf.setParameter(0, toWString(avgSelfCycles));
			row.totalCyclesSelf.setParameter(0, toWString(totalSelfCycles));

			curIdx++;
		}
	};

	class GPUSampleRowFiller
	{
	public:
		UINT32 curIdx;
		GUILayout& layout;
		GUIWidget& widget;
		Vector<ProfilerOverlay::GPUSampleRow>& rows;

		GPUSampleRowFiller(Vector<ProfilerOverlay::GPUSampleRow>& _rows, GUILayout& _layout, GUIWidget& _widget)
			:rows(_rows), curIdx(0), layout(_layout), widget(_widget)
		{ }

		~GPUSampleRowFiller()
		{
			UINT32 excessEntries = (UINT32)rows.size() - curIdx;
			for (UINT32 i = 0; i < excessEntries; i++)
			{
				ProfilerOverlay::GPUSampleRow& row = rows[curIdx + i];

				GUILayout::destroy(row.layout);
			}

			rows.resize(curIdx);
		}

		void addData(const String& name, float timeMs)
		{
			if (curIdx >= rows.size())
			{
				rows.push_back(ProfilerOverlay::GPUSampleRow());

				ProfilerOverlay::GPUSampleRow& newRow = rows.back();

				newRow.name = HString(L"{1}");
				newRow.time = HString(L"{0}");

				newRow.layout = layout.insertNewElement<GUILayoutX>(layout.getNumChildren());

				GUILabel* nameLabel = GUILabel::create(newRow.name, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* timeLabel = GUILabel::create(newRow.time, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.layout->addElement(nameLabel);
				newRow.layout->addElement(timeLabel);

				newRow.elements.push_back(nameLabel);
				newRow.elements.push_back(timeLabel);
			}

			ProfilerOverlay::GPUSampleRow& row = rows[curIdx];
			row.name.setParameter(0, toWString(name));
			row.time.setParameter(0, toWString(timeMs));

			curIdx++;
		}
	};

	const UINT32 ProfilerOverlay::MAX_DEPTH = 4;

	ProfilerOverlay::ProfilerOverlay(const HSceneObject& parent, const ViewportPtr& target)
		:Component(parent), mIsShown(false), mType(ProfilerOverlayType::CPUSamples)
	{
		setTarget(target);
	}

	ProfilerOverlay::~ProfilerOverlay()
	{
		if(mTarget != nullptr)
			mTargetResizedConn.disconnect();

		if(mWidgetSO)
			mWidgetSO->destroy();
	}

	void ProfilerOverlay::setTarget(const ViewportPtr& target)
	{
		if(mTarget != nullptr)
			mTargetResizedConn.disconnect();

		mTarget = target;

		mTargetResizedConn = target->getTarget()->onResized.connect(std::bind(&ProfilerOverlay::targetResized, this));

		if(mWidgetSO)
			mWidgetSO->destroy();

		mWidgetSO = SceneObject::create("ProfilerOverlay");
		mWidget = mWidgetSO->addComponent<GUIWidget>(mTarget.get());
		mWidget->setDepth(127);
		mWidget->setSkin(BuiltinResources::instance().getGUISkin());

		// Set up CPU sample areas
		mCPUBasicAreaLabels = GUIArea::create(*mWidget, 0, 0);
		mCPUPreciseAreaLabels = GUIArea::create(*mWidget, 0, 0);
		mCPUBasicAreaContents = GUIArea::create(*mWidget, 0, 0);
		mCPUPreciseAreaContents = GUIArea::create(*mWidget, 0, 0);

		mBasicLayoutLabels = mCPUBasicAreaLabels->getLayout().addNewElement<GUILayoutY>();
		mPreciseLayoutLabels = mCPUPreciseAreaLabels->getLayout().addNewElement<GUILayoutY>();
		mBasicLayoutContents = mCPUBasicAreaContents->getLayout().addNewElement<GUILayoutY>();
		mPreciseLayoutContents = mCPUPreciseAreaContents->getLayout().addNewElement<GUILayoutY>();

		// Set up CPU sample title bars
		mTitleBasicName = GUILabel::create(HString(L"Name"), GUIOptions(GUIOption::fixedWidth(200)));
		mTitleBasicPctOfParent = GUILabel::create(HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumCalls = GUILabel::create(HString(L"# calls"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumAllocs = GUILabel::create(HString(L"# allocs"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumFrees = GUILabel::create(HString(L"# frees"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicAvgTime = GUILabel::create(HString(L"Avg. time"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitleBasicTotalTime = GUILabel::create(HString(L"Total time"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitleBasicAvgTitleSelf = GUILabel::create(HString(L"Avg. self time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicTotalTimeSelf = GUILabel::create(HString(L"Total self time"), GUIOptions(GUIOption::fixedWidth(100)));

		mTitlePreciseName = GUILabel::create(HString(L"Name"), GUIOptions(GUIOption::fixedWidth(200)));
		mTitlePrecisePctOfParent = GUILabel::create(HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumCalls = GUILabel::create(HString(L"# calls"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumAllocs = GUILabel::create(HString(L"# allocs"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumFrees = GUILabel::create(HString(L"# frees"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseAvgCycles = GUILabel::create(HString(L"Avg. cycles"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitlePreciseTotalCycles = GUILabel::create(HString(L"Total cycles"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitlePreciseAvgCyclesSelf = GUILabel::create(HString(L"Avg. self cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseTotalCyclesSelf = GUILabel::create(HString(L"Total self cycles"), GUIOptions(GUIOption::fixedWidth(100)));

		GUILayout* basicTitleLabelLayout = mBasicLayoutLabels->addNewElement<GUILayoutX>();
		GUILayout* preciseTitleLabelLayout = mPreciseLayoutLabels->addNewElement<GUILayoutX>();
		GUILayout* basicTitleContentLayout = mBasicLayoutContents->addNewElement<GUILayoutX>();
		GUILayout* preciseTitleContentLayout = mPreciseLayoutContents->addNewElement<GUILayoutX>();

		basicTitleLabelLayout->addElement(mTitleBasicName);
		basicTitleContentLayout->addElement(mTitleBasicPctOfParent);
		basicTitleContentLayout->addElement(mTitleBasicNumCalls);
		basicTitleContentLayout->addElement(mTitleBasicNumAllocs);
		basicTitleContentLayout->addElement(mTitleBasicNumFrees);
		basicTitleContentLayout->addElement(mTitleBasicAvgTime);
		basicTitleContentLayout->addElement(mTitleBasicTotalTime);
		basicTitleContentLayout->addElement(mTitleBasicAvgTitleSelf);
		basicTitleContentLayout->addElement(mTitleBasicTotalTimeSelf);

		preciseTitleLabelLayout->addElement(mTitlePreciseName);
		preciseTitleContentLayout->addElement(mTitlePrecisePctOfParent);
		preciseTitleContentLayout->addElement(mTitlePreciseNumCalls);
		preciseTitleContentLayout->addElement(mTitlePreciseNumAllocs);
		preciseTitleContentLayout->addElement(mTitlePreciseNumFrees);
		preciseTitleContentLayout->addElement(mTitlePreciseAvgCycles);
		preciseTitleContentLayout->addElement(mTitlePreciseTotalCycles);
		preciseTitleContentLayout->addElement(mTitlePreciseAvgCyclesSelf);
		preciseTitleContentLayout->addElement(mTitlePreciseTotalCyclesSelf);

		mBasicLayoutLabels->addNewElement<GUIFlexibleSpace>();
		mPreciseLayoutLabels->addNewElement<GUIFlexibleSpace>();
		mBasicLayoutContents->addNewElement<GUIFlexibleSpace>();
		mPreciseLayoutContents->addNewElement<GUIFlexibleSpace>();

		// Set up GPU sample areas
		mGPUAreaFrameContents = GUIArea::create(*mWidget, 0, 0);
		mGPUAreaFrameSamples = GUIArea::create(*mWidget, 0, 0);
		mGPULayoutFrameContentsLeft = mGPUAreaFrameContents->getLayout().addNewElement<GUILayoutY>();
		mGPULayoutFrameContentsRight = mGPUAreaFrameContents->getLayout().addNewElement<GUILayoutY>();

		GUILayout* gpuSamplesMain = mGPUAreaFrameSamples->getLayout().addNewElement<GUILayoutY>();

		GUILayout* gpuSampleTitle = gpuSamplesMain->addNewElement<GUILayoutY>();
		mGPULayoutSamples = gpuSamplesMain->addNewElement<GUILayoutY>();
		gpuSamplesMain->addNewElement<GUIFlexibleSpace>();

		HString gpuSamplesStr(L"__ProfOvGPUSamples", L"Samples");
		gpuSampleTitle->addElement(GUILabel::create(gpuSamplesStr));
		gpuSampleTitle->addNewElement<GUIFixedSpace>(20);

		GUILayout* gpuSampleTitleRow = gpuSampleTitle->addNewElement<GUILayoutX>();

		HString gpuSamplesNameStr(L"__ProfOvGPUSampName", L"Name");
		HString gpuSamplesTimeStr(L"__ProfOvGPUSampTime", L"Time");
		gpuSampleTitleRow->addElement(GUILabel::create(gpuSamplesNameStr, GUIOptions(GUIOption::fixedWidth(100))));
		gpuSampleTitleRow->addElement(GUILabel::create(gpuSamplesTimeStr, GUIOptions(GUIOption::fixedWidth(100))));

		mGPUFrameNumStr = HString(L"__ProfOvFrame", L"Frame #{0}");
		mGPUTimeStr = HString(L"__ProfOvTime", L"Time: {0}ms");
		mGPUDrawCallsStr = HString(L"__ProfOvDrawCalls", L"Draw calls: {0}");
		mGPURenTargetChangesStr = HString(L"__ProfOvRTChanges", L"Render target changes: {0}");
		mGPUPresentsStr = HString(L"__ProfOvPresents", L"Presents: {0}");
		mGPUClearsStr = HString(L"__ProfOvClears", L"Clears: {0}");
		mGPUVerticesStr = HString(L"__ProfOvVertices", L"Num. vertices: {0}");
		mGPUPrimitivesStr = HString(L"__ProfOvPrimitives", L"Num. primitives: {0}");
		mGPUSamplesStr = HString(L"__ProfOvSamples", L"Samples drawn: {0}");
		mGPUBlendStateChangesStr = HString(L"__ProfOvBSChanges", L"Blend state changes: {0}");
		mGPURasterStateChangesStr = HString(L"__ProfOvRSChanges", L"Rasterizer state changes: {0}");
		mGPUDepthStencilStateChangesStr = HString(L"__ProfOvDSSChanges", L"Depth/stencil state changes: {0}");

		mGPUObjectsCreatedStr = HString(L"__ProfOvObjsCreated", L"Objects created: {0}");
		mGPUObjectsDestroyedStr = HString(L"__ProfOvObjsDestroyed", L"Objects destroyed: {0}");
		mGPUResourceWritesStr = HString(L"__ProfOvResWrites", L"Resource writes: {0}");
		mGPUResourceReadsStr = HString(L"__ProfOvResReads", L"Resource reads: {0}");
		mGPUTextureBindsStr = HString(L"__ProfOvTexBinds", L"Texture binds: {0}");
		mGPUSamplerBindsStr = HString(L"__ProfOvSampBinds", L"Sampler binds: {0}");
		mGPUVertexBufferBindsStr = HString(L"__ProfOvVBBinds", L"VB binds: {0}");
		mGPUIndexBufferBindsStr = HString(L"__ProfOvIBBinds", L"IB binds: {0}");
		mGPUGPUProgramBufferBindsStr = HString(L"__ProfOvProgBuffBinds", L"GPU program buffer binds: {0}");
		mGPUGPUProgramBindsStr = HString(L"__ProfOvProgBinds", L"GPU program binds: {0}");

		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUFrameNumStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUTimeStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUDrawCallsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPURenTargetChangesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUPresentsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUClearsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUVerticesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUPrimitivesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUSamplesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUBlendStateChangesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPURasterStateChangesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addElement(GUILabel::create(mGPUDepthStencilStateChangesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsLeft->addNewElement<GUIFlexibleSpace>();

		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUObjectsCreatedStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUObjectsDestroyedStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUResourceWritesStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUResourceReadsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUTextureBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUSamplerBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUVertexBufferBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUIndexBufferBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUGPUProgramBufferBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addElement(GUILabel::create(mGPUGPUProgramBindsStr, GUIOptions(GUIOption::fixedWidth(200))));
		mGPULayoutFrameContentsRight->addNewElement<GUIFlexibleSpace>();

		updateCPUSampleAreaSizes();
		updateGPUSampleAreaSizes();

		if (!mIsShown)
			hide();
		else
		{
			if (mType == ProfilerOverlayType::CPUSamples)
				show(ProfilerOverlayType::CPUSamples);
			else
				show(ProfilerOverlayType::GPUSamples);
		}
	}

	void ProfilerOverlay::show(ProfilerOverlayType type)
	{
		if (type == ProfilerOverlayType::CPUSamples)
		{
			mCPUBasicAreaLabels->enable();
			mCPUPreciseAreaLabels->enable();
			mCPUBasicAreaContents->enable();
			mCPUPreciseAreaContents->enable();
			mGPUAreaFrameContents->disable();
			mGPUAreaFrameSamples->disable();
		}
		else
		{
			mGPUAreaFrameContents->enable();
			mGPUAreaFrameSamples->enable();
			mCPUBasicAreaLabels->disable();
			mCPUPreciseAreaLabels->disable();
			mCPUBasicAreaContents->disable();
			mCPUPreciseAreaContents->disable();
		}

		mType = type;
		mIsShown = true;
	}

	void ProfilerOverlay::hide()
	{
		mCPUBasicAreaLabels->disable();
		mCPUPreciseAreaLabels->disable();
		mCPUBasicAreaContents->disable();
		mCPUPreciseAreaContents->disable();
		mGPUAreaFrameContents->disable();
		mGPUAreaFrameSamples->disable();
		mIsShown = false;
	}

	void ProfilerOverlay::update()
	{
		const ProfilerReport& latestSimReport = ProfilingManager::instance().getReport(ProfiledThread::Sim);
		const ProfilerReport& latestCoreReport = ProfilingManager::instance().getReport(ProfiledThread::Core);

		updateCPUSampleContents(latestSimReport, latestCoreReport);

		while (ProfilerGPU::instance().getNumAvailableReports() > 1)
			ProfilerGPU::instance().getNextReport(); // Drop any extra reports, we only want the latest

		if (ProfilerGPU::instance().getNumAvailableReports() > 0)
		{
			updateGPUSampleContents(ProfilerGPU::instance().getNextReport());
		}
	}

	void ProfilerOverlay::targetResized()
	{
		updateCPUSampleAreaSizes();
		updateGPUSampleAreaSizes();
	}

	void ProfilerOverlay::updateCPUSampleAreaSizes()
	{
		static const INT32 PADDING = 10;
		static const float LABELS_CONTENT_RATIO = 0.3f;

		UINT32 width = (UINT32)std::max(0, (INT32)mTarget->getWidth() - PADDING * 2);
		UINT32 height = (UINT32)std::max(0, (INT32)(mTarget->getHeight() - PADDING * 3)/2);

		UINT32 labelsWidth = Math::ceilToInt(width * LABELS_CONTENT_RATIO);
		UINT32 contentWidth = width - labelsWidth;

		mCPUBasicAreaLabels->setPosition(PADDING, PADDING);
		mCPUBasicAreaLabels->setSize(labelsWidth, height);

		mCPUPreciseAreaLabels->setPosition(PADDING, height + PADDING * 2);
		mCPUPreciseAreaLabels->setSize(labelsWidth, height);

		mCPUBasicAreaContents->setPosition(PADDING + labelsWidth, PADDING);
		mCPUBasicAreaContents->setSize(contentWidth, height);

		mCPUPreciseAreaContents->setPosition(PADDING + labelsWidth, height + PADDING * 2);
		mCPUPreciseAreaContents->setSize(contentWidth, height);
	}

	void ProfilerOverlay::updateGPUSampleAreaSizes()
	{
		static const INT32 PADDING = 10;
		static const float SAMPLES_FRAME_RATIO = 0.5f;

		UINT32 width = (UINT32)std::max(0, (INT32)mTarget->getWidth() - PADDING * 2);
		UINT32 height = (UINT32)std::max(0, (INT32)(mTarget->getHeight() - PADDING * 3));

		UINT32 frameHeight = Math::ceilToInt(height * SAMPLES_FRAME_RATIO);
		UINT32 samplesHeight = height - frameHeight;

		mGPUAreaFrameContents->setPosition(PADDING, PADDING);
		mGPUAreaFrameContents->setSize(width, frameHeight);

		mGPUAreaFrameSamples->setPosition(PADDING, PADDING + frameHeight + PADDING);
		mGPUAreaFrameSamples->setSize(width, samplesHeight);
	}

	void ProfilerOverlay::updateCPUSampleContents(const ProfilerReport& simReport, const ProfilerReport& coreReport)
	{
		static const UINT32 NUM_ROOT_ENTRIES = 2;

		const CPUProfilerBasicSamplingEntry& simBasicRootEntry = simReport.cpuReport.getBasicSamplingData();
		const CPUProfilerPreciseSamplingEntry& simPreciseRootEntry = simReport.cpuReport.getPreciseSamplingData();

		const CPUProfilerBasicSamplingEntry& coreBasicRootEntry = coreReport.cpuReport.getBasicSamplingData();
		const CPUProfilerPreciseSamplingEntry& corePreciseRootEntry = coreReport.cpuReport.getPreciseSamplingData();

		struct TodoBasic
		{
			TodoBasic(const CPUProfilerBasicSamplingEntry& _entry, UINT32 _depth)
				:entry(_entry), depth(_depth)
			{ }

			const CPUProfilerBasicSamplingEntry& entry;
			UINT32 depth;
		};

		struct TodoPrecise
		{
			TodoPrecise(const CPUProfilerPreciseSamplingEntry& _entry, UINT32 _depth)
				:entry(_entry), depth(_depth)
			{ }

			const CPUProfilerPreciseSamplingEntry& entry;
			UINT32 depth;
		};

		BasicRowFiller basicRowFiller(mBasicRows, *mBasicLayoutLabels, *mBasicLayoutContents, *mWidget);
		Stack<TodoBasic> todoBasic;

		const CPUProfilerBasicSamplingEntry* basicRootEntries[NUM_ROOT_ENTRIES];
		basicRootEntries[0] = &simBasicRootEntry;
		basicRootEntries[1] = &coreBasicRootEntry;

		for(UINT32 i = 0; i < NUM_ROOT_ENTRIES; i++)
		{
			todoBasic.push(TodoBasic(*basicRootEntries[i], 0));

			while(!todoBasic.empty())
			{
				TodoBasic curEntry = todoBasic.top();
				todoBasic.pop();

				const CPUProfilerBasicSamplingEntry::Data& data = curEntry.entry.data;
				basicRowFiller.addData(curEntry.depth, data.name, data.pctOfParent, data.numCalls, data.memAllocs, data.memFrees, 
					data.avgTimeMs, data.totalTimeMs, data.avgSelfTimeMs, data.totalSelfTimeMs);

				if(curEntry.depth <= MAX_DEPTH)
				{
					for(auto iter = curEntry.entry.childEntries.rbegin(); iter != curEntry.entry.childEntries.rend(); ++iter)
					{
						todoBasic.push(TodoBasic(*iter, curEntry.depth + 1));
					}
				}
			}
		}

		PreciseRowFiller preciseRowFiller(mPreciseRows, *mBasicLayoutLabels, *mBasicLayoutContents, *mWidget);
		Stack<TodoPrecise> todoPrecise;

		const CPUProfilerPreciseSamplingEntry* preciseRootEntries[NUM_ROOT_ENTRIES];
		preciseRootEntries[0] = &simPreciseRootEntry;
		preciseRootEntries[1] = &corePreciseRootEntry;

		for(UINT32 i = 0; i < NUM_ROOT_ENTRIES; i++)
		{
			todoPrecise.push(TodoPrecise(*preciseRootEntries[i], 0));

			while(!todoBasic.empty())
			{
				TodoPrecise curEntry = todoPrecise.top();
				todoPrecise.pop();

				const CPUProfilerPreciseSamplingEntry::Data& data = curEntry.entry.data;
				preciseRowFiller.addData(curEntry.depth, data.name, data.pctOfParent, data.numCalls, data.memAllocs, data.memFrees, 
					data.avgCycles, data.totalCycles, data.avgSelfCycles, data.totalSelfCycles);

				if(curEntry.depth <= MAX_DEPTH)
				{
					for(auto iter = curEntry.entry.childEntries.rbegin(); iter != curEntry.entry.childEntries.rend(); ++iter)
					{
						todoPrecise.push(TodoPrecise(*iter, curEntry.depth + 1));
					}
				}
			}
		}
	}

	void ProfilerOverlay::updateGPUSampleContents(const GPUProfilerReport& gpuReport)
	{
		mGPUFrameNumStr.setParameter(0, toWString((UINT64)gTime().getFrameNumber()));
		mGPUTimeStr.setParameter(0, toWString(gpuReport.frameSample.timeMs));
		mGPUDrawCallsStr.setParameter(0, toWString(gpuReport.frameSample.numDrawCalls));
		mGPURenTargetChangesStr.setParameter(0, toWString(gpuReport.frameSample.numRenderTargetChanges));
		mGPUPresentsStr.setParameter(0, toWString(gpuReport.frameSample.numPresents));
		mGPUClearsStr.setParameter(0, toWString(gpuReport.frameSample.numClears));
		mGPUVerticesStr.setParameter(0, toWString(gpuReport.frameSample.numVertices));
		mGPUPrimitivesStr.setParameter(0, toWString(gpuReport.frameSample.numPrimitives));
		mGPUSamplesStr.setParameter(0, toWString(gpuReport.frameSample.numDrawnSamples));
		mGPUBlendStateChangesStr.setParameter(0, toWString(gpuReport.frameSample.numBlendStateChanges));
		mGPURasterStateChangesStr.setParameter(0, toWString(gpuReport.frameSample.numRasterizerStateChanges));
		mGPUDepthStencilStateChangesStr.setParameter(0, toWString(gpuReport.frameSample.numDepthStencilStateChanges));

		mGPUObjectsCreatedStr.setParameter(0, toWString(gpuReport.frameSample.numObjectsCreated));
		mGPUObjectsDestroyedStr.setParameter(0, toWString(gpuReport.frameSample.numObjectsDestroyed));
		mGPUResourceWritesStr.setParameter(0, toWString(gpuReport.frameSample.numResourceWrites));
		mGPUResourceReadsStr.setParameter(0, toWString(gpuReport.frameSample.numResourceReads));
		mGPUTextureBindsStr.setParameter(0, toWString(gpuReport.frameSample.numTextureBinds));
		mGPUSamplerBindsStr.setParameter(0, toWString(gpuReport.frameSample.numSamplerBinds));
		mGPUVertexBufferBindsStr.setParameter(0, toWString(gpuReport.frameSample.numVertexBufferBinds));
		mGPUIndexBufferBindsStr.setParameter(0, toWString(gpuReport.frameSample.numIndexBufferBinds));
		mGPUGPUProgramBufferBindsStr.setParameter(0, toWString(gpuReport.frameSample.numGpuParamBufferBinds));
		mGPUGPUProgramBindsStr.setParameter(0, toWString(gpuReport.frameSample.numGpuProgramBinds));

		GPUSampleRowFiller sampleRowFiller(mGPUSampleRows, *mGPULayoutSamples, *mWidget);
		for (auto& sample : gpuReport.samples)
		{
			sampleRowFiller.addData(sample.name, sample.timeMs);
		}
	}
}