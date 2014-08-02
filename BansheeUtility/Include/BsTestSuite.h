#pragma once

#include "BsPrerequisitesUtil.h"

#define BS_TEST_ASSERT(expr) assertment((expr), "", __FILE__, __LINE__); 
#define BS_TEST_ASSERT_MSG(expr, msg) assertment((expr), msg, __FILE__, __LINE__); 

namespace BansheeEngine
{
	/**
	 * @brief	Primary class for unit testing. Override and register unit tests
	 *			in constructor then run the tests using the desired method of output.
	 */
	class BS_UTILITY_EXPORT TestSuite
	{
	public:
		typedef void(TestSuite::*Func)();

	private:

		/**
		 * @brief	Contains data about a single unit test.
		 */
		struct TestEntry
		{
			TestEntry(Func test, const String& name);

			Func test;
			String name;
		};

	public:
		virtual ~TestSuite() {}

		/**
		 * @brief	Runs all the tests in the suite (and sub-suites). Tests results
		 *			are reported to the provided output class.
		 */
		void run(TestOutput& output);

		/**
		 * @brief	Adds a new child suite to this suite. This method allows
		 *			you to group suites and execute them all at once.
		 */
		void add(const TestSuitePtr& suite);

		/**
		 * @brief	Creates a new suite of a particular type.
		 */
		template <class T>
		static TestSuitePtr create()
		{
			static_assert((std::is_base_of<TestSuite, T>::value), "Invalid test suite type. It needs to derive from BansheeEngine::TestSuite.");

			return std::static_pointer_cast<TestSuite>(bs_shared_ptr<T>());
		}

	protected:
		TestSuite();

		/**
		 * @brief	Called right before any tests are ran.
		 */
		virtual void startUp() {}

		/**
		 * @brief	Called after all tests and child suite's tests are ran.
		 */
		virtual void shutDown() {}

		/**
		 * @brief	Register a new unit test.
		 *
		 * @param	test	Function to call in order to execute the test.
		 * @param	name	Name of the test we can use for referencing it later.
		 */
		void addTest(Func test, const String& name);

		/**
		 * @brief	Reports success or failure depending on the result of an expression.
		 *
		 * @param	success		If true success is reported, otherwise failure.
		 * @param	file		Name of the source code file the assertment originates from.
		 * @param	line		Line number at which the assertment was triggered at.
		 */
		void assertment(bool success, const String& desc, const String& file, long line);

		Vector<TestEntry> mTests;
		Vector<TestSuitePtr> mSuites;

		// Transient
		TestOutput* mOutput;
		String mActiveTestName;
	};
}

#define BS_ADD_TEST(func) addTest(static_cast<Func>(&func), #func);