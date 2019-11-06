#include "DataTable.h"


DataTable::Row::Row(DataTable *owner,
                    size_t index)
    : owner_(owner),
      index_(index)
{

}

bool DataTable::Row::operator==(DataTable::Row other) const
{
    return owner_ == other.owner_ && index_ == other.index_;
}

bool DataTable::Row::operator==(const vector<any> &other) const
{
    vector<any>& thisRow = owner_->data_[index_];
    return std::equal(thisRow.cbegin(),
                      thisRow.cend(),
                      other.cbegin(),
                      [](const Value &left, const Value &right)
    {
        return Value::equals(left, right);
    });
}

any& DataTable::Row::operator[](string columnName)
{
    return owner_->data_[index_][owner_->columnToIndex_.at(columnName)];
}

DataTable::Value DataTable::Row::operator[](const DataTable::Column &column) const
{
    return owner_->data_[index_][column.index()];
}

any &DataTable::Row::operator[](size_t index)
{
    return owner_->data_[index_][index];
}

DataTable::DataTable(initializer_list<const char *> columns)
{
    addColumns(move(columns));
}

void DataTable::fill(vector<vector<any> > data)
{
    for_each(data.begin(), data.end(), [this](auto row){ addRow(row);});
}

void DataTable::addRow(vector<any> row)
{
    data_.push_back(vector<any>());
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
    return DataTable::Row(this, rowIndex);
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
            row += " '" + Value(value).toString() + "' ";
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


DataTable::Value::Value(any val)
    : value_(convertChar(move(val)))
{}

bool DataTable::Value::equals(const DataTable::Value &left,
                              const DataTable::Value &right)
{
    EQUALS(int, left.value(), right.value())
    EQUALS(string, left.value(), right.value())

    return false;
}

bool DataTable::Value::operator ==(const DataTable::Value &other) const
{
    return equals(*this, other);
}

DataTable::Value DataTable::Value::operator+(const DataTable::Value &other) const
{
    SUM(int, value(), other.value())
    SUM(string, value(), other.value())

    return DataTable::Value(0);
}

const type_info& DataTable::Value::type() const
{
    return value_.type();
}

const any &DataTable::Value::value() const
{
    return value_;
}

string DataTable::Value::toString() const
{
    if(auto val = toNativeType<int>(value_); val)
    {
        return to_string(val.value());
    }
    else if(auto val = toNativeType<string>(value_); val)
    {
        return val.value();
    }
    else if(auto val = toNativeType<rxFunction>(value_); val)
    {
        return "function";
    }

    cout << "not supported " << value_.type().name() << " " << endl;
    return string();
}

any DataTable::Value::convertChar(any value)
{
    if(value.type() == typeid(const char*))
    {
        return any(string(any_cast<const char*>(value)));
    }

    return value;
}

DataTable::Column::Column(DataTable *owner,
                          size_t index)
    : owner_(owner),
      index_(index)
{

}

void DataTable::Column::operator=(any value)
{
    if(auto func = toNativeType<rxFunction>(value); func)
    {

    }
    else
    {
        for_each(owner_->begin(),
                 owner_->end(),
                 [index = index_, &value](vector<any> &row)
        {
            row[index] = value;
        });
    }
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

vector<any> DataTable::Column::operator+(const DataTable::Column &right) const
{
    assert(owner_ == right.owner_);

    vector<any> result;
    result.reserve(owner_->rowCount());
    result.resize(owner_->rowCount());
    for(size_t row = 0; row < owner_->rowCount(); ++row)
    {
        result[row] = (owner_->operator[](row)[*this]
                + owner_->operator[](row)[right]).value();
    }

    return result;
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


template<class T>
optional<T> toNativeType(any value)
{
    if(value.type() == typeid(T))
    {
        return any_cast<T>(value);
    }
    return {};
}

void print(string line)
{
     cout << line << endl;
}
