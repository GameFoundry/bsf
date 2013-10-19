#include "BsProfilerOverlay.h"
#include "CmSceneObject.h"
#include "CmRenderWindowManager.h"
#include "BsGUIWidget.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILabel.h"
#include "BsGUISpace.h"
#include "BsEngineGUI.h"
#include "CmProfiler.h"

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
				labelLayout.removeChildAt(labelLayout.getNumChildren() - i - 1); // -1 because last element is flexible space and we want to skip it
				contentLayout.removeChildAt(contentLayout.getNumChildren() - i - 1); // -1 because last element is flexible space and we want to skip it

				ProfilerOverlay::BasicRow& row = rows[curIdx + i];

				for(auto& element : row.elements)
					GUIElement::destroy(element);
			}

			rows.resize(curIdx);
		}

		void addData(UINT32 depth, const String& name, float pctOfParent, UINT32 numCalls, double avgTime, double totalTime, 
			double maxTime, double avgSelfTime, double totalSelfTime, double estOverhead, double estOverheadSelf)
		{
			if(curIdx >= rows.size())
			{
				rows.push_back(ProfilerOverlay::BasicRow());

				ProfilerOverlay::BasicRow& newRow = rows.back();

				newRow.name = HString(L"{0}");
				newRow.pctOfParent = HString(L"{0}");
				newRow.numCalls = HString(L"{0}");
				newRow.avgTime = HString(L"{0}");
				newRow.totalTime = HString(L"{0}");
				newRow.maxTime = HString(L"{0}");
				newRow.avgTimeSelf = HString(L"{0}");
				newRow.totalTimeSelf = HString(L"{0}");
				newRow.estOverhead = HString(L"{0}");
				newRow.estOverheadSelf = HString(L"{0}");

				newRow.labelLayout = &labelLayout.insertLayoutX(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = &contentLayout.insertLayoutX(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(widget, newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(widget, newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* numCalls = GUILabel::create(widget, newRow.numCalls, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* avgTime = GUILabel::create(widget, newRow.avgTime, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalTime = GUILabel::create(widget, newRow.totalTime, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* maxTime = GUILabel::create(widget, newRow.maxTime, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* avgTimeSelf = GUILabel::create(widget, newRow.avgTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalTimeSelf = GUILabel::create(widget, newRow.totalTimeSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* estOverhead = GUILabel::create(widget, newRow.estOverhead, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* estOverheadSelf = GUILabel::create(widget, newRow.estOverheadSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelLayout->addSpace(0);
				newRow.labelLayout->addElement(name);

				newRow.contentLayout->addElement(pctOfParent);
				newRow.contentLayout->addElement(numCalls);
				newRow.contentLayout->addElement(avgTime);
				newRow.contentLayout->addElement(totalTime);
				newRow.contentLayout->addElement(maxTime);
				newRow.contentLayout->addElement(avgTimeSelf);
				newRow.contentLayout->addElement(totalTimeSelf);
				newRow.contentLayout->addElement(estOverhead);
				newRow.contentLayout->addElement(estOverheadSelf);

				newRow.elements.push_back(name);
				newRow.elements.push_back(pctOfParent);
				newRow.elements.push_back(numCalls);
				newRow.elements.push_back(avgTime);
				newRow.elements.push_back(totalTime);
				newRow.elements.push_back(maxTime);
				newRow.elements.push_back(avgTimeSelf);
				newRow.elements.push_back(totalTimeSelf);
				newRow.elements.push_back(estOverhead);
				newRow.elements.push_back(estOverheadSelf);
			}
			
			ProfilerOverlay::BasicRow& row = rows[curIdx];

			row.labelLayout->removeChildAt(0);
			row.labelLayout->insertSpace(0, depth * 20);

			row.name.setParameter(0, toWString(name));
			row.pctOfParent.setParameter(0, toWString(pctOfParent));
			row.numCalls.setParameter(0, toWString(numCalls));
			row.avgTime.setParameter(0, toWString(avgTime));
			row.totalTime.setParameter(0, toWString(totalTime));
			row.maxTime.setParameter(0, toWString(maxTime));
			row.avgTimeSelf.setParameter(0, toWString(avgSelfTime));
			row.totalTimeSelf.setParameter(0, toWString(totalSelfTime));
			row.estOverhead.setParameter(0, toWString(estOverhead));
			row.estOverheadSelf.setParameter(0, toWString(estOverheadSelf));

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

		void addData(UINT32 depth, const String& name, float pctOfParent, UINT32 numCalls, UINT64 avgCycles, UINT64 totalCycles, 
			UINT64 maxCycles, UINT64 avgSelfCycles, UINT64 totalSelfCycles, UINT64 estOverhead, UINT64 estOverheadSelf)
		{
			if(curIdx >= rows.size())
			{
				rows.push_back(ProfilerOverlay::PreciseRow());

				ProfilerOverlay::PreciseRow& newRow = rows.back();

				newRow.name = HString(L"{0}");
				newRow.pctOfParent = HString(L"{0}");
				newRow.numCalls = HString(L"{0}");
				newRow.avgCycles = HString(L"{0}");
				newRow.totalCycles = HString(L"{0}");
				newRow.maxCycles = HString(L"{0}");
				newRow.avgCyclesSelf = HString(L"{0}");
				newRow.totalCyclesSelf = HString(L"{0}");
				newRow.estOverhead = HString(L"{0}");
				newRow.estOverheadSelf = HString(L"{0}");

				newRow.labelLayout = &labelLayout.insertLayoutX(labelLayout.getNumChildren() - 1); // Insert before flexible space
				newRow.contentLayout = &contentLayout.insertLayoutX(contentLayout.getNumChildren() - 1); // Insert before flexible space

				GUILabel* name = GUILabel::create(widget, newRow.name, GUIOptions(GUIOption::fixedWidth(200)));
				GUILabel* pctOfParent = GUILabel::create(widget, newRow.pctOfParent, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* numCalls = GUILabel::create(widget, newRow.numCalls, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* avgCycles = GUILabel::create(widget, newRow.avgCycles,GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalCycles = GUILabel::create(widget, newRow.totalCycles, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* maxCycles = GUILabel::create(widget, newRow.maxCycles, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* avgCyclesSelf = GUILabel::create(widget, newRow.avgCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* totalCyclesSelf = GUILabel::create(widget, newRow.totalCyclesSelf, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* estOverhead = GUILabel::create(widget, newRow.estOverhead, GUIOptions(GUIOption::fixedWidth(100)));
				GUILabel* estOverheadSelf = GUILabel::create(widget, newRow.estOverheadSelf, GUIOptions(GUIOption::fixedWidth(100)));

				newRow.labelLayout->addSpace(0);
				newRow.labelLayout->addElement(name);

				newRow.contentLayout->addElement(pctOfParent);
				newRow.contentLayout->addElement(numCalls);
				newRow.contentLayout->addElement(avgCycles);
				newRow.contentLayout->addElement(totalCycles);
				newRow.contentLayout->addElement(maxCycles);
				newRow.contentLayout->addElement(avgCyclesSelf);
				newRow.contentLayout->addElement(totalCyclesSelf);
				newRow.contentLayout->addElement(estOverhead);
				newRow.contentLayout->addElement(estOverheadSelf);

				newRow.elements.push_back(name);
				newRow.elements.push_back(pctOfParent);
				newRow.elements.push_back(numCalls);
				newRow.elements.push_back(avgCycles);
				newRow.elements.push_back(totalCycles);
				newRow.elements.push_back(maxCycles);
				newRow.elements.push_back(avgCyclesSelf);
				newRow.elements.push_back(totalCyclesSelf);
				newRow.elements.push_back(estOverhead);
				newRow.elements.push_back(estOverheadSelf);
			}

			ProfilerOverlay::PreciseRow& row = rows[curIdx];

			row.labelLayout->removeChildAt(0);
			row.labelLayout->insertSpace(0, depth * 20);

			row.name.setParameter(0, toWString(name));
			row.pctOfParent.setParameter(0, toWString(pctOfParent));
			row.numCalls.setParameter(0, toWString(numCalls));
			row.avgCycles.setParameter(0, toWString(avgCycles));
			row.totalCycles.setParameter(0, toWString(totalCycles));
			row.maxCycles.setParameter(0, toWString(maxCycles));
			row.avgCyclesSelf.setParameter(0, toWString(avgSelfCycles));
			row.totalCyclesSelf.setParameter(0, toWString(totalSelfCycles));
			row.estOverhead.setParameter(0, toWString(estOverhead));
			row.estOverheadSelf.setParameter(0, toWString(estOverheadSelf));

			curIdx++;
		}
	};

	const UINT32 ProfilerOverlay::MAX_DEPTH = 2;

	ProfilerOverlay::ProfilerOverlay(const CM::ViewportPtr& target, const CM::RenderWindowPtr& ownerWindow)
		:mIsShown(false), mBasicAreaLabels(nullptr), mPreciseAreaLabels(nullptr), mBasicAreaContents(nullptr), mPreciseAreaContents(nullptr),
		mBasicLayoutLabels(nullptr), mPreciseLayoutLabels(nullptr), mBasicLayoutContents(nullptr), mPreciseLayoutContents(nullptr),
		mTitleBasicName(nullptr), mTitleBasicPctOfParent(nullptr), mTitleBasicNumCalls(nullptr), mTitleBasicAvgTime(nullptr), 
		mTitleBasicTotalTime(nullptr), mTitleBasicMaxTime(nullptr), mTitleBasicAvgTitleSelf(nullptr), mTitleBasicTotalTimeSelf(nullptr), 
		mTitleBasicEstOverhead(nullptr), mTitleBasicEstOverheadSelf(nullptr), mTitlePreciseName(nullptr), mTitlePrecisePctOfParent(nullptr), 
		mTitlePreciseNumCalls(nullptr), mTitlePreciseAvgCycles(nullptr), mTitlePreciseTotalCycles(nullptr), mTitlePreciseMaxCycles(nullptr), 
		mTitlePreciseAvgCyclesSelf(nullptr), mTitlePreciseTotalCyclesSelf(nullptr), mTitlePreciseEstOverhead(nullptr), mTitlePreciseEstOverheadSelf(nullptr)
	{
		setTarget(target, ownerWindow);

		mWindowMovedOrResized = RenderWindowManager::instance().onMovedOrResized.connect(boost::bind(&ProfilerOverlay::windowMovedOrResized, this, _1));
	}

	ProfilerOverlay::~ProfilerOverlay()
	{
		if(mIsShown)
			hide();

		if(mOwnerWindow != nullptr)
			mWindowMovedOrResized.disconnect();

		if(mWidgetSO)
			mWidgetSO->destroy();
	}

	void ProfilerOverlay::setTarget(const CM::ViewportPtr& target, const CM::RenderWindowPtr& ownerWindow)
	{
		if(mOwnerWindow != nullptr)
			mWindowMovedOrResized.disconnect();

		mTarget = target;
		mOwnerWindow = ownerWindow;

		if(mWidgetSO)
			mWidgetSO->destroy();

		mWidgetSO = SceneObject::create("ProfilerOverlay");
		mWidget = mWidgetSO->addComponent<GUIWidget>(mTarget.get(), mOwnerWindow.get());
		mWidget->setDepth(127);
		mWidget->setSkin(EngineGUI::instance().getSkin());

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
		mTitleBasicPctOfParent = GUILabel::create(*mWidget, HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicNumCalls = GUILabel::create(*mWidget, HString(L"Num. calls"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicAvgTime = GUILabel::create(*mWidget, HString(L"Avg. time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicTotalTime = GUILabel::create(*mWidget, HString(L"Total time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicMaxTime = GUILabel::create(*mWidget, HString(L"Max time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicAvgTitleSelf = GUILabel::create(*mWidget, HString(L"Avg. self time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicTotalTimeSelf = GUILabel::create(*mWidget, HString(L"Total self time"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicEstOverhead = GUILabel::create(*mWidget, HString(L"Est. overhead"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitleBasicEstOverheadSelf = GUILabel::create(*mWidget, HString(L"Est. self overhead"), GUIOptions(GUIOption::fixedWidth(100)));

		mTitlePreciseName = GUILabel::create(*mWidget, HString(L"Name"), GUIOptions(GUIOption::fixedWidth(200)));
		mTitlePrecisePctOfParent = GUILabel::create(*mWidget, HString(L"% parent"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseNumCalls = GUILabel::create(*mWidget, HString(L"Num. calls"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseAvgCycles = GUILabel::create(*mWidget, HString(L"Avg. cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseTotalCycles = GUILabel::create(*mWidget, HString(L"Total cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseMaxCycles = GUILabel::create(*mWidget, HString(L"Max cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseAvgCyclesSelf = GUILabel::create(*mWidget, HString(L"Avg. self cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseTotalCyclesSelf = GUILabel::create(*mWidget, HString(L"Total self cycles"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseEstOverhead = GUILabel::create(*mWidget, HString(L"Est. overhead"), GUIOptions(GUIOption::fixedWidth(100)));
		mTitlePreciseEstOverheadSelf = GUILabel::create(*mWidget, HString(L"Est. self overhead"), GUIOptions(GUIOption::fixedWidth(100)));

		GUILayout& basicTitleLabelLayout = mBasicLayoutLabels->addLayoutX();
		GUILayout& preciseTitleLabelLayout = mPreciseLayoutLabels->addLayoutX();
		GUILayout& basicTitleContentLayout = mBasicLayoutContents->addLayoutX();
		GUILayout& preciseTitleContentLayout = mPreciseLayoutContents->addLayoutX();

		basicTitleLabelLayout.addElement(mTitleBasicName);
		basicTitleContentLayout.addElement(mTitleBasicPctOfParent);
		basicTitleContentLayout.addElement(mTitleBasicNumCalls);
		basicTitleContentLayout.addElement(mTitleBasicAvgTime);
		basicTitleContentLayout.addElement(mTitleBasicTotalTime);
		basicTitleContentLayout.addElement(mTitleBasicMaxTime);
		basicTitleContentLayout.addElement(mTitleBasicAvgTitleSelf);
		basicTitleContentLayout.addElement(mTitleBasicTotalTimeSelf);
		basicTitleContentLayout.addElement(mTitleBasicEstOverhead);
		basicTitleContentLayout.addElement(mTitleBasicEstOverheadSelf);

		preciseTitleLabelLayout.addElement(mTitlePreciseName);
		preciseTitleContentLayout.addElement(mTitlePrecisePctOfParent);
		preciseTitleContentLayout.addElement(mTitlePreciseNumCalls);
		preciseTitleContentLayout.addElement(mTitlePreciseAvgCycles);
		preciseTitleContentLayout.addElement(mTitlePreciseTotalCycles);
		preciseTitleContentLayout.addElement(mTitlePreciseMaxCycles);
		preciseTitleContentLayout.addElement(mTitlePreciseAvgCyclesSelf);
		preciseTitleContentLayout.addElement(mTitlePreciseTotalCyclesSelf);
		preciseTitleContentLayout.addElement(mTitlePreciseEstOverhead);
		preciseTitleContentLayout.addElement(mTitlePreciseEstOverheadSelf);

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
		const ProfilerReport& latestReport = Profiler::instance().getReport();

		updateContents(latestReport);
	}

	void ProfilerOverlay::windowMovedOrResized(RenderWindow& window)
	{
		if(&window != mOwnerWindow.get())
		{
			updateAreaSizes();
		}
	}

	void ProfilerOverlay::updateAreaSizes()
	{
		static const INT32 PADDING = 10;
		static const float LABELS_CONTENT_RATIO = 0.3f;

		UINT32 width = (UINT32)std::max(0, (INT32)mOwnerWindow->getWidth() - PADDING * 2);
		UINT32 height = (UINT32)std::max(0, (INT32)(mOwnerWindow->getHeight() - PADDING * 3)/2);

		UINT32 labelsWidth = Math::CeilToInt(width * LABELS_CONTENT_RATIO);
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

	void ProfilerOverlay::updateContents(const ProfilerReport& report)
	{
		const CPUProfilerBasicSamplingEntry& basicRootEntry = report.cpuReport.getBasicSamplingData();
		const CPUProfilerPreciseSamplingEntry& preciseRootEntry = report.cpuReport.getPreciseSamplingData();

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
		todoBasic.push(TodoBasic(basicRootEntry, 0));

		while(!todoBasic.empty())
		{
			TodoBasic curEntry = todoBasic.top();
			todoBasic.pop();

			const CPUProfilerBasicSamplingEntry::Data& data = curEntry.entry.data;
			basicRowFiller.addData(curEntry.depth, data.name, data.pctOfParent, data.numCalls, data.avgTimeMs, data.totalTimeMs, 
				data.maxTimeMs, data.avgSelfTimeMs, data.totalSelfTimeMs, data.estimatedOverheadMs, data.estimatedSelfOverheadMs);

			if(curEntry.depth <= MAX_DEPTH)
			{
				for(auto& child : curEntry.entry.childEntries)
				{
					todoBasic.push(TodoBasic(child, curEntry.depth + 1));
				}
			}
		}

		PreciseRowFiller preciseRowFiller(mPreciseRows, *mBasicLayoutLabels, *mBasicLayoutContents, *mWidget);

		Stack<TodoPrecise>::type todoPrecise;
		todoPrecise.push(TodoPrecise(preciseRootEntry, 0));

		while(!todoBasic.empty())
		{
			TodoPrecise curEntry = todoPrecise.top();
			todoPrecise.pop();

			const CPUProfilerPreciseSamplingEntry::Data& data = curEntry.entry.data;
			preciseRowFiller.addData(curEntry.depth, data.name, data.pctOfParent, data.numCalls, data.avgCycles, data.totalCycles, 
				data.maxCycles, data.avgSelfCycles, data.totalSelfCycles, data.estimatedOverhead, data.estimatedSelfOverhead);

			if(curEntry.depth <= MAX_DEPTH)
			{
				for(auto& child : curEntry.entry.childEntries)
				{
					todoPrecise.push(TodoPrecise(child, curEntry.depth + 1));
				}
			}
		}
	}
}