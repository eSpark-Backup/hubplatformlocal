#include "gtest/googlemock/include/gmock/gmock.h"
#include "gtest/googletest/include/gtest/gtest.h"

using ::testing::AtLeast;
using ::testing::Return;

int main( int argc, char** argv ) {
    ::testing::InitGoogleTest( &argc, argv );
    int ret = RUN_ALL_TESTS();
    return ret;
}
