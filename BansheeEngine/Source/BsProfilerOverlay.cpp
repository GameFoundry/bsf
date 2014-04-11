#include "BsProfilerOverlay.h"
#include "CmSceneObject.h"
#include "BsGUIWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "CmProfiler.h"
#include "CmViewport.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	class BasicRowFiller
	{
	public:
		UINT32 curIdx;
		GUILayout& labelLayout;
		GUILayout& contentLayout;
		GUIWidget& widget;
		Vector<ProfilerOverlay::BasicRow>::type& rows;

		BasicRowFiller(Vector<ProfilerOverlay::BasicRow>::type& _rows, GUILayout& _labelLayout, GUILayout& _contentLayout, GUIWidget& _widget)
			:rows(_rows), curIdx(0), labelLayout(_labelLayout), contentLayout(_contentLayout), widget(_widget)
		{ }

		~BasicRowFiller()
		{
			UINT32 excessEntries = (UINT32)rows.size() - curIdx;
			for(UINT32 i = 0; i < excessEntries; i++)
			{
				labelLayout.removeChildAt(labelLayout.getNumChildren() - 2); // -2 because last element is flexible space and we want to skip it
				contentLayout.removeChildAt(contentLayout.getNumChildren() - 2); // -2 because last element is flexible space and we want to skip it

				ProfilerOverlay::BasicRow& row = rows[curIdx + i];

				for(auto& element : row.elements)
					GUIElement::destroy(element);
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

				newRow.labelLayout = &labelLayout.insertLayoutX(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = &contentLayout.insertLayoutX(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(widget, newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(widget, newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numCalls = GUILabel::create(widget, newRow.numCalls, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numAllocs = GUILabel::create(widget, newRow.numAllocs, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numFrees = GUILabel::create(widget, newRow.numFrees, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* avgTime = GUILabel::create(widget, newRow.avgTime, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* totalTime = GUILabel::create(widget, newRow.totalTime, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* avgTimeSelf = GUILabel::create(widget, newRow.avgTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalTimeSelf = GUILabel::create(widget, newRow.totalTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelLayout->addSpace(0);
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

			row.labelLayout->removeChildAt(0);
			row.labelLayout->insertSpace(0, depth * 20);

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
		Vector<ProfilerOverlay::PreciseRow>::type& rows;

		PreciseRowFiller(Vector<ProfilerOverlay::PreciseRow>::type& _rows, GUILayout& _labelLayout, GUILayout& _contentLayout, GUIWidget& _widget)
			:rows(_rows), curIdx(0), labelLayout(_labelLayout), contentLayout(_contentLayout), widget(_widget)
		{ }

		~PreciseRowFiller()
		{
			UINT32 excessEntries = (UINT32)rows.size() - curIdx;
			for(UINT32 i = 0; i < excessEntries; i++)
			{
				labelLayout.removeChildAt(labelLayout.getNumChildren() - i - 1); // -1 because last element is flexible space and we want to skip it
				contentLayout.removeChildAt(contentLayout.getNumChildren() - i - 1); // -1 because last element is flexible space and we want to skip it

				ProfilerOverlay::PreciseRow& row = rows[curIdx + i];

				for(auto& element : row.elements)
					GUIElement::destroy(element);
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

				newRow.labelLayout = &labelLayout.insertLayoutX(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = &contentLayout.insertLayoutX(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(widget, newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(widget, newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numCalls = GUILabel::create(widget, newRow.numCalls, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numAllocs = GUILabel::create(widget, newRow.numAllocs, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* numFrees = GUILabel::create(widget, newRow.numFrees, GUIOptions(GUIOption::fixedWidth(50)));
				GUILabel* avgCycles = GUILabel::create(widget, newRow.avgCycles,GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* totalCycles = GUILabel::create(widget, newRow.totalCycles, GUIOptions(GUIOption::fixedWidth(60)));
				GUILabel* avgCyclesSelf = GUILabel::create(widget, newRow.avgCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalCyclesSelf = GUILabel::create(widget, newRow.totalCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelLayout->addSpace(0);
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

			row.labelLayout->removeChildAt(0);
			row.labelLayout->insertSpace(0, depth * 20);

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

	const UINT32 ProfilerOverlay::MAX_DEPTH = 4;

	ProfilerOverlay::ProfilerOverlay(const CM::ViewportPtr& target)
		:mIsShown(false), mBasicAreaLabels(nullptr), mPreciseAreaLabels(nullptr), mBasicAreaContents(nullptr), mPreciseAreaContents(nullptr),
		mBasicLayoutLabels(nullptr), mPreciseLayoutLabels(nullptr), mBasicLayoutContents(nullptr), mPreciseLayoutContents(nullptr),
		mTitleBasicName(nullptr), mTitleBasicPctOfParent(nullptr), mTitleBasicNumCalls(nullptr), mTitleBasicNumAllocs(nullptr), mTitleBasicNumFrees(nullptr),
		mTitleBasicAvgTime(nullptr), mTitleBasicTotalTime(nullptr), mTitleBasicAvgTitleSelf(nullptr), mTitleBasicTotalTimeSelf(nullptr), 
		mTitlePreciseName(nullptr), mTitlePrecisePctOfParent(nullptr), mTitlePreciseNumCalls(nullptr), mTitlePreciseNumAllocs(nullptr), 
		mTitlePreciseNumFrees(nullptr), mTitlePreciseAvgCycles(nullptr), mTitlePreciseTotalCycles(nullptr), mTitlePreciseAvgCyclesSelf(nullptr), 
		mTitlePreciseTotalCyclesSelf(nullptr)
	{
		setTarget(target);
	}

	ProfilerOverlay::~ProfilerOverlay()
	{
		if(mIsShown)
			hide();

		if(mTarget != nullptr)
			mTargetResizedConn.disconnect();

		if(mWidgetSO)
			mWidgetSO->destroy();
	}

	void ProfilerOverlay::setTarget(const CM::ViewportPtr& target)
	{
		if(mTarget != nullptr)
			mTargetResizedConn.disconnect();

		mTarget = target;

		mTargetResizedConn = target->onResized.connect(std::bind(&ProfilerOverlay::targetResized, this));

		if(mWidgetSO)
			mWidgetSO->destroy();

		mWidgetSO = SceneObject::create("ProfilerOverlay");
		mWidget = mWidgetSO->addComponent<GUIWidget>(mTarget.get());
		mWidget->setDepth(127);
		mWidget->setSkin(BuiltinResources::instance().getSkin());

		mBasicAreaLabels = GUIArea::create(*mWidget, 0, 0);
		mPreciseAreaLabels = GUIArea::create(*mWidget, 0, 0);
		mBasicAreaContents = GUIArea::create(*mWidget, 0, 0);
		mPreciseAreaContents = GUIArea::create(*mWidget, 0, 0);

		mBasicLayoutLabels = &mBasicAreaLabels->getLayout().addLayoutY();
		mPreciseLayoutLabels = &mPreciseAreaLabels->getLayout().addLayoutY();
		mBasicLayoutContents = &mBasicAreaContents->getLayout().addLayoutY();
		mPreciseLayoutContents = &mPreciseAreaContents->getLayout().addLayoutY();

		// Set up title bars
		mTitleBasicName = GUILabel::create(*mWidget, HString(L"Name"), GUIOptions(GUIOption::fixedWidth(200)));
		mTitleBasicPctOfParent = GUILabel::create(*mWidget, HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumCalls = GUILabel::create(*mWidget, HString(L"# calls"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumAllocs = GUILabel::create(*mWidget, HString(L"# allocs"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicNumFrees = GUILabel::create(*mWidget, HString(L"# frees"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitleBasicAvgTime = GUILabel::create(*mWidget, HString(L"Avg. time"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitleBasicTotalTime = GUILabel::create(*mWidget, HString(L"Total time"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitleBasicAvgTitleSelf = GUILabel::create(*mWidget, HString(L"Avg. self time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicTotalTimeSelf = GUILabel::create(*mWidget, HString(L"Total self time"), GUIOptions(GUIOption::fixedWidth(100)));

		mTitlePreciseName = GUILabel::create(*mWidget, HString(L"Name"), GUIOptions(GUIOption::fixedWidth(200)));
		mTitlePrecisePctOfParent = GUILabel::create(*mWidget, HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumCalls = GUILabel::create(*mWidget, HString(L"# calls"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumAllocs = GUILabel::create(*mWidget, HString(L"# allocs"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseNumFrees = GUILabel::create(*mWidget, HString(L"# frees"), GUIOptions(GUIOption::fixedWidth(50)));
		mTitlePreciseAvgCycles = GUILabel::create(*mWidget, HString(L"Avg. cycles"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitlePreciseTotalCycles = GUILabel::create(*mWidget, HString(L"Total cycles"), GUIOptions(GUIOption::fixedWidth(60)));
		mTitlePreciseAvgCyclesSelf = GUILabel::create(*mWidget, HString(L"Avg. self cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseTotalCyclesSelf = GUILabel::create(*mWidget, HString(L"Total self cycles"), GUIOptions(GUIOption::fixedWidth(100)));

		GUILayout& basicTitleLabelLayout = mBasicLayoutLabels->addLayoutX();
		GUILayout& preciseTitleLabelLayout = mPreciseLayoutLabels->addLayoutX();
		GUILayout& basicTitleContentLayout = mBasicLayoutContents->addLayoutX();
		GUILayout& preciseTitleContentLayout = mPreciseLayoutContents->addLayoutX();

		basicTitleLabelLayout.addElement(mTitleBasicName);
		basicTitleContentLayout.addElement(mTitleBasicPctOfParent);
		basicTitleContentLayout.addElement(mTitleBasicNumCalls);
		basicTitleContentLayout.addElement(mTitleBasicNumAllocs);
		basicTitleContentLayout.addElement(mTitleBasicNumFrees);
		basicTitleContentLayout.addElement(mTitleBasicAvgTime);
		basicTitleContentLayout.addElement(mTitleBasicTotalTime);
		basicTitleContentLayout.addElement(mTitleBasicAvgTitleSelf);
		basicTitleContentLayout.addElement(mTitleBasicTotalTimeSelf);

		preciseTitleLabelLayout.addElement(mTitlePreciseName);
		preciseTitleContentLayout.addElement(mTitlePrecisePctOfParent);
		preciseTitleContentLayout.addElement(mTitlePreciseNumCalls);
		preciseTitleContentLayout.addElement(mTitlePreciseNumAllocs);
		preciseTitleContentLayout.addElement(mTitlePreciseNumFrees);
		preciseTitleContentLayout.addElement(mTitlePreciseAvgCycles);
		preciseTitleContentLayout.addElement(mTitlePreciseTotalCycles);
		preciseTitleContentLayout.addElement(mTitlePreciseAvgCyclesSelf);
		preciseTitleContentLayout.addElement(mTitlePreciseTotalCyclesSelf);

		mBasicLayoutLabels->addFlexibleSpace();
		mPreciseLayoutLabels->addFlexibleSpace();
		mBasicLayoutContents->addFlexibleSpace();
		mPreciseLayoutContents->addFlexibleSpace();

		updateAreaSizes();
	}

	void ProfilerOverlay::show()
	{
		if(mIsShown)
			return;

		mBasicAreaLabels->enable();
		mPreciseAreaLabels->enable();
		mBasicAreaContents->enable();
		mPreciseAreaContents->enable();
		mIsShown = true;
	}

	void ProfilerOverlay::hide()
	{
		if(!mIsShown)
			return;

		mBasicAreaLabels->disable();
		mPreciseAreaLabels->disable();
		mBasicAreaContents->disable();
		mPreciseAreaContents->disable();
		mIsShown = false;
	}

	void ProfilerOverlay::update()
	{
		const ProfilerReport& latestSimReport = Profiler::instance().getReport(ProfiledThread::Sim);
		const ProfilerReport& latestCoreReport = Profiler::instance().getReport(ProfiledThread::Core);

		updateContents(latestSimReport, latestCoreReport);
	}

	void ProfilerOverlay::targetResized()
	{
		updateAreaSizes();
	}

	void ProfilerOverlay::updateAreaSizes()
	{
		static const INT32 PADDING = 10;
		static const float LABELS_CONTENT_RATIO = 0.3f;

		UINT32 width = (UINT32)std::max(0, (INT32)mTarget->getWidth() - PADDING * 2);
		UINT32 height = (UINT32)std::max(0, (INT32)(mTarget->getHeight() - PADDING * 3)/2);

		UINT32 labelsWidth = Math::ceilToInt(width * LABELS_CONTENT_RATIO);
		UINT32 contentWidth = width - labelsWidth;

		mBasicAreaLabels->setPosition(PADDING, PADDING);
		mBasicAreaLabels->setSize(labelsWidth, height);

		mPreciseAreaLabels->setPosition(PADDING, height + PADDING * 2);
		mPreciseAreaLabels->setSize(labelsWidth, height);

		mBasicAreaContents->setPosition(PADDING + labelsWidth, PADDING);
		mBasicAreaContents->setSize(contentWidth, height);

		mPreciseAreaContents->setPosition(PADDING + labelsWidth, height + PADDING * 2);
		mPreciseAreaContents->setSize(contentWidth, height);
	}

	void ProfilerOverlay::updateContents(const CM::ProfilerReport& simReport, const CM::ProfilerReport& coreReport)
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
		Stack<TodoBasic>::type todoBasic;

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
		Stack<TodoPrecise>::type todoPrecise;

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
}