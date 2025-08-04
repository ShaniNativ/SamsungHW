/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#ifndef  DEFINITIONS_HPP
#define  DEFINITIONS_HPP

#include <cstddef> //size_t
#include <cstdint> //uint32_t
#include <vector> //std::vector

namespace samsung
{
static const size_t g_data_size = 4096; 

struct Frame
{
    uint32_t address; 
    uint32_t time_ms; //to keep presicion time in frame will be in millisecond
    char data[g_data_size]; //what this will be??
};

struct BinMetaData
{
    uint32_t threshold; 
    uint32_t delta;
    uint32_t total_frames; 
};

struct MemoryWrite
{
    uint32_t start_time; 
    uint32_t duration; 
    uint32_t start_address; 
    uint32_t frames; 
};

struct ConfigFile
{
    uint32_t threshold; 
    uint32_t delta; 
    std::vector<std::vector<MemoryWrite>> writing_patterns; 
};

struct FlashFrame
{
    uint32_t address; 
    char data[g_data_size]; //what this will be??
};

}


#endif //DEFINITIONS_HPP