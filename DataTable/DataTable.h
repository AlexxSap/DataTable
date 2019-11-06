#pragma once

#include <cassert>
#include <unordered_map>
#include <vector>
#include <any>
#include <algorithm>
#include <optional>
#include <iostream>
#include <functional>

using namespace std;

template<class T>
optional<T> toNativeType(any value);

void print(string line);

#define CHECK(Type, value1, value2) \
    if(auto val1 = toNativeType<Type>(value1); val1) \
    if(auto val2 = toNativeType<Type>(value2); val2)

#define SUM(Type, value1, value2) \
    CHECK(Type, value1, value2) \
    return DataTable::Value(val1.value() + val2.value());

#define EQUALS(Type, value1, value2) \
    CHECK(Type, value1, value2) \
    return val1.value() == val2.value();

#define rxFunction function<vector<any>(DataTable&)>



class DataTable
{
public:
    class Value
    {
    public:
        Value(any val);
        static bool equals(const Value &left, const Value &right);
        bool operator == (const Value& other) const;
        Value operator+(const DataTable::Value& other) const;
        const type_info& type() const;
        const any& value() const;
        string toString() const;
    private:
        any convertChar(any value);
        any value_;
    };

    class Column
    {
    public:
        Column(DataTable* owner,
               size_t index);
        void operator= (any value);
        void operator= (vector<any> columnData);
        void operator= (const Column& other);
        vector<any> operator+(const DataTable::Column& right) const;
        size_t index() const;
        vector<any> data() const;

    private:
        DataTable *owner_ = nullptr;
        size_t index_;
    };

    class Row
    {
    public:
        explicit Row(DataTable* owner,
                     size_t index);
        bool operator==(Row other) const;
        bool operator==(const vector<any>& other) const;
        any& operator[](string columnName);
        Value operator[](const Column& column) const;
        any& operator[](size_t index);
    private:
        DataTable *owner_ = nullptr;
        size_t index_;
    };

public:
    explicit DataTable(initializer_list<const char*> columns);

    void fill(vector<vector<any>> data);
    void addRow(vector<any> row);

    using iterator = vector<vector<any>>::iterator;
    iterator begin();
    iterator end();

    using const_iterator = vector<vector<any>>::const_iterator;
    inline const_iterator cbegin() const;
    inline const_iterator cend() const;

    Row operator[](size_t rowIndex);
    Column operator[](string columnName);

    string toString() const;
    size_t rowCount() const;

protected:
    void addColumns(initializer_list<const char*> columns);
    void addColumn(string column);

    unordered_map<size_t, string> indexToColumn_;
    unordered_map<string, size_t> columnToIndex_;
    size_t columnSize_ = 0;
    size_t rowSize_ = 0;

    vector<vector<any>> data_;
};

