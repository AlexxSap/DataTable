#include <iostream>
#include "DataTable.h"
#include "gtest/gtest.h"

using namespace std;


//namespace std {
//bool operator == (any left, any right)
//{
//    if(left.type() != right.type())
//    {
//        return false;
//    }

//    if(left.type() == typeid(int))
//    {
//        return any_cast<int>(left) == any_cast<int>(right);
//    }

//    if(left.type() == typeid(const char*))
//    {
//        return string(any_cast<const char*>(left)) == string(any_cast<const char*>(right));
//    }

//    if(left.type() == typeid(string))
//    {
//        return any_cast<string>(left) == any_cast<string>(right);
//    }

//    return false;
//}
//}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (TestUnitFill, Simple)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, "aa"}, {2, "bb"}});

    cout << dt.toString() << endl;

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

