#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <any>
#include <algorithm>
#include <optional>
#include "gtest/gtest.h"

using namespace std;

class Value : public any
{
public:
    Value(any val) : any(move(val)) {}
};
//{
//public:
//    Value(any value) : any(value) {}
//    Value() = default;

namespace std {


bool operator == (any left, any right)
{
    if(left.type() != right.type())
    {
        return false;
    }

    if(left.type() == typeid(int))
    {
        return any_cast<int>(left) == any_cast<int>(right);
    }

    if(left.type() == typeid(const char*))
    {
        return string(any_cast<const char*>(left)) == string(any_cast<const char*>(right));
    }

    if(left.type() == typeid(string))
    {
        return any_cast<string>(left) == any_cast<string>(right);
    }

    return false;
}
}

class DataTable
{
public:
    using Row = vector<any>;

public:
    explicit DataTable(initializer_list<const char*> columns)
    {
        addColumns(move(columns));
    }

    void fill(vector<Row> data)
    {
        for_each(data.begin(), data.end(), [this](auto row){ addRow(row);});
    }

    void addRow(Row row)
    {
        data_.push_back(Row());
        for_each(row.begin(), row.end(), [r = &data_[rowSize_]](auto value){r->push_back(value);});
        rowSize_++;
    }

    using iterator = vector<Row>::iterator;
    inline iterator begin()
    {
        return data_.begin();
    }

    inline iterator end()
    {
        return data_.end();
    }

    Row& operator[](size_t rowIndex)
    {
        return data_[rowIndex];
    }

//    Row& operator[](tuple value);

protected:
    void addColumns(initializer_list<const char*> columns)
    {
        for(string column : columns)
        {
            indexToColumn_.emplace(columnSize_, column);
            columnSize_++;
        }
    }

    unordered_map<int, string> indexToColumn_;
    size_t columnSize_ = 0;
    size_t rowSize_ = 0;

    vector<Row> data_;
};

optional<string> anyToString(any value)
{
    if(value.type() == typeid(int))
    {
        return to_string(any_cast<int>(value));
    }

    if(value.type() == typeid(const char*))
    {
        return string(any_cast<const char*>(value));
    }

    if(value.type() == typeid(string))
    {
        return any_cast<string>(value);
    }

    cout << "not supported " << value.type().name() << " " << endl;
    return {};
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (TestUnitFill, Simple)
{
    DataTable dt{"col1", "col2"};
    dt.fill({{1, "aa"}, {2, "bb"}});

    for(auto rowIter = dt.begin(); rowIter != dt.end(); ++rowIter)
    {
        string row;
        for(auto value : *rowIter)
        {
            row += anyToString(value).value_or("") + " ";
        }
        cout << row << endl;
    }

    DataTable::Row r{1, "aa"};
    EXPECT_EQ(dt[0], r);

//    EXPECT_EQ(dt[1]["col1"], 1);
//    EXPECT_EQ(dt[2]["col2"], "bb");
}

