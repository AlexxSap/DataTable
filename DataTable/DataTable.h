#ifndef DATATABLE_H
#define DATATABLE_H

#include <cassert>
#include <unordered_map>
#include <vector>
#include <any>
#include <algorithm>
#include <optional>
#include <iostream>

using namespace std;

class DataTable
{
public:
    class Value : public any
    {
    public:
        Value(any val);
        static bool equals(const Value &left, const Value &right);
        bool operator == (const Value& other);
    };

    class Row : public vector<any>
    {
    public:
        Row() = default;
        Row(initializer_list<any> values);
        bool operator==(Row other) const;
        Value operator[](string columnName) const;
        any& operator[](size_t index);
        Row& setNames(unordered_map<string, size_t> *columnToIndex);
    private:
        unordered_map<string, size_t> *columnToIndex_ = nullptr;
    };

    class Column : public vector<any>
    {
    public:
        Column(DataTable* owner,
               const size_t index);
        void operator= (any value);

    private:
        DataTable *owner_ = nullptr;
        size_t index_ = 0;

    };

public:
    explicit DataTable(initializer_list<const char*> columns);

    void fill(vector<Row> data);
    void addRow(Row row);

    using iterator = vector<Row>::iterator;
    iterator begin();
    iterator end();

    using const_iterator = vector<Row>::const_iterator;
    inline const_iterator cbegin() const;
    inline const_iterator cend() const;

    Row operator[](size_t rowIndex);
    Column operator[](string columnName);

    string toString() const;

//    Row& operator[](tuple value);

protected:
    void addColumns(initializer_list<const char*> columns);
    optional<string> anyToString(any value) const;

    unordered_map<size_t, string> indexToColumn_;
    unordered_map<string, size_t> columnToIndex_;
    size_t columnSize_ = 0;
    size_t rowSize_ = 0;

    vector<Row> data_;
};

bool operator == (DataTable::Value left, DataTable::Value right);

#endif // DATATABLE_H
