#include <iostream>
#include "DataTable.h"
#include "gtest/gtest.h"

using namespace std;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (TestUnitFill, Simple)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, "aa"}, {2, "bb"}});

    {
        DataTable::Row r{1, "aa"};
        EXPECT_EQ(dt[0], r);
    }

    {
        DataTable::Row r{2, "bb"};
        EXPECT_EQ(dt[1], r);
    }

    EXPECT_EQ(dt[0]["col1"], DataTable::Value(1));
}


