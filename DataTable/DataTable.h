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

void print(string line);

template<class T>
optional<T> toNativeType(any value);

template<class T>
optional<pair<T, T>> convert(any left, any right);

/// TODO как-то заменить макросы операциями
#define SUM(Type, left, right) \
    if(auto values = convert<Type>(left, right); values) return DataTable::Value(values.value().first + values.value().second);

#define SUB(Type, left, right) \
    if(auto values = convert<Type>(left, right); values) return DataTable::Value(values.value().first - values.value().second);

#define EQUALS(Type, left, right) \
    if(auto values = convert<Type>(left, right); values) return values.value().first == values.value().second;


class DataTable
{
public:
    using fn = function<vector<any>(DataTable&)>;

    class Value
    {
    public:
        Value(any val);
        static bool equals(const Value &left, const Value &right);
        bool operator == (const Value& other) const;
        Value operator+(const DataTable::Value& other) const;
        Value operator-(const DataTable::Value& other) const;
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
        vector<any> operator-(const DataTable::Column& right) const;
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
        Value operator[](const Column& column) const;
        any& operator[](string columnName);
        any& operator[](size_t index);

        any value(string columnName)const ;
        any value(size_t index) const;
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
    any value(size_t rowIndex, string columnName) const;

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

