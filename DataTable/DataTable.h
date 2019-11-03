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
        Row& setNames(unordered_map<string, size_t> *columnToIndex);
    private:
        unordered_map<string, size_t> *columnToIndex_ = nullptr;
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
