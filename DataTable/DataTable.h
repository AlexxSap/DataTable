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

optional<string> anyToString(any value);

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
    private:
        any value_;
    };

    class Column
    {
    public:
        Column(DataTable* owner,
               const size_t index);
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

    class Row : public vector<any>
    {
    public:
        Row() = default;
        explicit Row(DataTable* owner);
        explicit Row(vector<any> other);
        Row(initializer_list<any> values);
        bool operator==(Row other) const;
        Value operator[](string columnName) const;
        Value operator[](const Column& column) const;
        any& operator[](size_t index);
    private:
        DataTable *owner_ = nullptr;
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

    Row& operator[](size_t rowIndex);
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

    vector<Row> data_;
};

#endif // DATATABLE_H
