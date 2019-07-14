---
title: Unit tests
---

All unit tests are implemented as a part of a @bs::TestSuite class. You can create your own test suites, or add tests to the existing ones. 

To register new tests call @BS_ADD_TEST in the test suite's constructor. The test method must not accept any parameters or return any values. To report test failure call @BS_TEST_ASSERT or @BS_TEST_ASSERT_MSG. If neither of those trigger, test is assumed to be successful.

~~~~~~~~~~~~~{.cpp}
class MyTestSuite : TestSuite
{
public:
	MyTestSuite()
	{
		BS_ADD_TEST(MyTestSuite::myTest);
	}
	
private:
	void myTest()
	{
		BS_TEST_ASSERT_MSG(2 + 2 == 4, "Something really bad is going on.");
	}
};
~~~~~~~~~~~~~

To run all tests in a test suite create an instance of the **TestSuite** and run it, like so:
~~~~~~~~~~~~~{.cpp}
SPtr<TestSuite> tests = MyTestSuite::create<MyTestSuite>();
tests->run(ExceptionTestOutput());
~~~~~~~~~~~~~

When running the test we provide @bs::ExceptionTestOutput which tells the test runner to terminate the application when a test fails. You can implement your own @bs::TestOutput class to handle test failure more gracefully.
