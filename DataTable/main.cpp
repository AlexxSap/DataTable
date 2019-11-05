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
        const vector<any> r{1, "aa"};
        EXPECT_EQ(dt[0], r);
    }

    {
        const vector<any> r{2, "bb"};
        EXPECT_EQ(dt[1], r);
    }

    EXPECT_EQ(dt[0]["col1"], DataTable::Value(1));
}

TEST(TestUnitFill, SetDirectData)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, "aa"}, {2, "bb"}});

    dt["col1"] = 3;

    {
        const vector<any> r{3, "aa"};
        EXPECT_EQ(dt[0], r);
    }

    {
        const vector<any> r{3, "bb"};
        EXPECT_EQ(dt[1], r);
    }
}

TEST(TestUnitFill, CopyColumnData)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, 2}, {2, 3}});

    dt["col3"] = dt["col1"];

    {
        const vector<any> r{1, 2, 1};
        EXPECT_EQ(dt[0], r);
    }

    {
        const vector<any> r{2, 3, 2};
        EXPECT_EQ(dt[1], r);
    }
}

TEST(TestUnitFill, SetCalculatedData)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, 2}, {2, 3}});

    dt["col3"] = dt["col1"] + dt["col2"];

    cout << dt.toString() << endl;

    {
        const vector<any> r{1, 2, 3};
        EXPECT_EQ(dt[0], r);
    }

    {
        const vector<any> r{2, 3, 5};
        EXPECT_EQ(dt[1], r);
    }
}


