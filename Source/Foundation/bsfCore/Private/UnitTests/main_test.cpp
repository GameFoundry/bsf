#include "gtest/gtest.h"
#include "Testing/BsTestSuite.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new bs::AppEnvironment);
    return RUN_ALL_TESTS();
}
