#include <cstdio>
#include "gtest/gtest.h"
#include "testUtils.hpp"
#include "basicTest.hpp"

GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running main() from " << __FILE__ << std::endl;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
