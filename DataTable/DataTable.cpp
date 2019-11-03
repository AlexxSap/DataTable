#include "DataTable.h"


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

DataTable::Row::Row(DataTable *owner)
    : owner_(owner)
{

}

DataTable::Row::Row(vector<any> other)
    : vector<any>(move(other))
{

}

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
    return vector<any>::operator[](owner_->columnToIndex_.at(columnName));
}

DataTable::Value DataTable::Row::operator[](const DataTable::Column &column) const
{
    return vector<any>::operator[](column.index());
}

any &DataTable::Row::operator[](size_t index)
{
    return vector<any>::operator[](index);
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
    data_.push_back(Row(this));
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

DataTable::Row& DataTable::operator[](size_t rowIndex)
{
    return data_[rowIndex];
}

DataTable::Column DataTable::operator[](string columnName)
{
    if(auto iter = columnToIndex_.find(columnName); iter != columnToIndex_.end())
    {
        return DataTable::Column(this, iter->second);
    }

    addColumn(columnName);
    return DataTable::Column(this, columnSize_ - 1);
}

string DataTable::toString() const
{
    string result;
    for(size_t col = 0; col < columnSize_; ++col)
    {
        result += " '" + indexToColumn_.at(col) + "' ";
    }
    result += "\n";

    for(auto rowIter = cbegin(); rowIter != cend(); ++rowIter)
    {
        string row;
        for(auto value : *rowIter)
        {
            row += " '" + anyToString(value).value_or("") + "' ";
        }
        result += "{" + row + "} \n";
    }

    return result;
}

size_t DataTable::rowCount() const
{
    return rowSize_;
}

void DataTable::addColumns(initializer_list<const char *> columns)
{
    for(string column : columns)
    {
        addColumn(column);
    }
}

void DataTable::addColumn(string column)
{
    indexToColumn_.emplace(columnSize_, column);
    columnToIndex_.emplace(column, columnSize_);

    for(auto &row : data_)
    {
        row.emplace_back(any(0));
    }
    columnSize_++;
}

bool operator ==(DataTable::Value left, DataTable::Value right)
{
    return DataTable::Value::equals(left, right);
}

DataTable::Value::Value(any val)
    : value_(move(val))
{}

bool DataTable::Value::equals(const DataTable::Value &left,
                              const DataTable::Value &right)
{
    if(left.type() != right.type())
    {
        return false;
    }

    if(left.type() == typeid(int))
    {
        return any_cast<int>(left.value()) == any_cast<int>(right.value());
    }

    if(left.type() == typeid(const char*))
    {
        return string(any_cast<const char*>(left.value())) == string(any_cast<const char*>(right.value()));
    }

    if(left.type() == typeid(string))
    {
        return any_cast<string>(left.value()) == any_cast<string>(right.value());
    }

    return false;
}

bool DataTable::Value::operator ==(const DataTable::Value &other)
{
    return equals(*this, other);
}

const type_info& DataTable::Value::type() const
{
    return value_.type();
}

const any &DataTable::Value::value() const
{
    return value_;
}

DataTable::Column::Column(DataTable *owner,
                          const size_t index)
    : owner_(owner),
      index_(index)
{

}

void DataTable::Column::operator=(any value)
{
    for_each(owner_->begin(),
             owner_->end(),
             [index = index_, &value](Row &row)
    {
        row[index] = value;
    });
}

void DataTable::Column::operator=(vector<any> columnData)
{
    for(size_t row = 0; row < owner_->rowCount(); ++row)
    {
        owner_->operator[](row)[index_] = columnData[row];
    }
}

void DataTable::Column::operator=(const DataTable::Column &other)
{
    this->operator=(other.data());
}

size_t DataTable::Column::index() const
{
    return index_;
}

vector<any> DataTable::Column::data() const
{
    vector<any> d;
    d.reserve(owner_->rowCount());
    for(size_t row = 0; row < owner_->rowCount(); ++row)
    {
        d.emplace_back(owner_->operator[](row)[index_]);
    }

    return d;
}

vector<any> operator+(const DataTable::Column &left,
                      const DataTable::Column &right)
{
    assert(left.owner_ == right.owner_);

    vector<any> result;
    DataTable* owner = left.owner_;
    result.reserve(owner->rowCount());
    result.resize(owner->rowCount());
    for(size_t row = 0; row < owner->rowCount(); ++row)
    {
        result[row] = (owner->operator[](row)[left]
                + owner->operator[](row)[right]).value();
    }

    return result;
}

DataTable::Value operator+(const DataTable::Value &left,
                           const DataTable::Value &right)
{
    assert(left.type() == right.type());

    if(left.type() == typeid(int))
    {
        return DataTable::Value(any_cast<int>(left.value())
                                + any_cast<int>(right.value()));

    }
    else if(left.type() == typeid(const char*))
    {
        return DataTable::Value((string(any_cast<const char*>(left.value()))
                                 + string(any_cast<const char*>(right.value()))).c_str());
    }
    else if(left.type() == typeid(string))
    {
         return DataTable::Value(any_cast<string>(left.value()) + any_cast<string>(right.value()));
    }

    return DataTable::Value(0);
}
