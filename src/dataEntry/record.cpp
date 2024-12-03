#include "record.hpp"


Record::Record()
{
    this->series = std::set<int32_t>();
}

Record::Record(const std::string& mode)
{
    if (mode == "empty") {
        this->series = std::set<int32_t>();
    }

    if(mode == "random"){
        this->generateRandomSeries();
    }

 
}

void Record::generateRandomSeries()
{
    std::random_device rd;                     
    std::mt19937 gen(rd());           
    std::uniform_int_distribution<int> distOne(1, MAX_RECORD_COUNT); 
    int count = distOne(gen);        
    std::uniform_int_distribution<int> dist(MIN_NUMBER_VALUE, MAX_NUMBER_VALUE); 

    for(int i = 0; i < count; i++)
    {
        this->series.insert(dist(gen));
    }
}

Record::Record(std::vector<int32_t> series)
{
    for (int element : series) {
        if (this->series.size() >= MAX_RECORD_COUNT) {
            break; 
        }
        this->series.insert(element);
    }
    
}


const std::set<int>& Record::getSeries() const
{
    return this->series;
}

Record& Record::operator=(const Record &record)
{
    this->series = record.series;
    return *this;
}

void Record::insert(int32_t value)
{
    this->series.insert(value);
}

void Record::remove(int32_t value)
{
    this->series.erase(value);
}

const int32_t Record::getSize() const
{
    return static_cast<int32_t>(this->series.size());
}

const int32_t Record::getSizeInBytes() const
{
    return static_cast<int32_t>(this->series.size() * sizeof(int32_t) + sizeof(int32_t));
}

std::ostream &operator<<(std::ostream &os, const Record &record)
{
    for (auto it = record.series.rbegin(); it != record.series.rend(); it++)
    {
        os << *it << " ";
    }
    return os;
}

int32_t Record::getMaxRecordCount()
{
    return MAX_RECORD_COUNT;
}

int32_t Record::getMinNumberValue()
{
    return MIN_NUMBER_VALUE;
}

int32_t Record::getMaxNumberValue()
{
    return MAX_NUMBER_VALUE;
}