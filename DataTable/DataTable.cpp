#include "DataTable.h"



DataTable::Row::Row(initializer_list<any> values)
    : vector<any>(move(values))
{}

bool DataTable::Row::operator==(DataTable::Row other) const
{
    return std::equal(cbegin(),
                      cend(),
                      other.cbegin(),
                      [](const Value &left, const Value &right)
    {
        return Value::equals(left, right);
    });
}

DataTable::Value DataTable::Row::operator[](string columnName) const
{
    return vector<any>::operator[](columnToIndex_->at(columnName));
}

DataTable::Row &DataTable::Row::setNames(unordered_map<string, size_t> *columnToIndex)
{
    columnToIndex_ = columnToIndex;
    return *this;
}

DataTable::DataTable(initializer_list<const char *> columns)
{
    addColumns(move(columns));
}

void DataTable::fill(vector<DataTable::Row> data)
{
    for_each(data.begin(), data.end(), [this](auto row){ addRow(row);});
}

void DataTable::addRow(DataTable::Row row)
{
    data_.push_back(Row());
    for_each(row.begin(), row.end(), [r = &data_[rowSize_]](auto value){r->push_back(value);});
    rowSize_++;
}

DataTable::iterator DataTable::begin()
{
    return data_.begin();
}

DataTable::iterator DataTable::end()
{
    return data_.end();
}

DataTable::const_iterator DataTable::cbegin() const
{
    return data_.cbegin();
}

DataTable::const_iterator DataTable::cend() const
{
    return data_.cend();
}

DataTable::Row DataTable::operator[](size_t rowIndex)
{
    return data_[rowIndex].setNames(&columnToIndex_);
}

string DataTable::toString() const
{
    string result;
    for(auto rowIter = cbegin(); rowIter != cend(); ++rowIter)
    {
        string row;
        for(auto value : *rowIter)
        {
            row += " '" + anyToString(value).value_or("") + "' ";
        }
        result += "{" + row + "} ";
    }

    return result;
}

void DataTable::addColumns(initializer_list<const char *> columns)
{
    for(string column : columns)
    {
        indexToColumn_.emplace(columnSize_, column);
        columnToIndex_.emplace(column, columnSize_);
        columnSize_++;
    }
}

optional<string> DataTable::anyToString(any value) const
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

bool operator ==(DataTable::Value left, DataTable::Value right)
{
    return DataTable::Value::equals(left, right);
}

DataTable::Value::Value(any val) : any(move(val)) {}

bool DataTable::Value::equals(const DataTable::Value &left, const DataTable::Value &right)
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

bool DataTable::Value::operator ==(const DataTable::Value &other)
{
    return equals(*this, other);
}

//bool operator ==(const any left, const any right)
//{
//    return DataTable::Value::equals(left, right);
//}
