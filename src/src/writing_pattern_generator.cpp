/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/

#include <iostream> //std::cerr, 
#include <fstream> //std::ofstream, 
#include <cstring> //std::memset, 

#include <writing_pattern_generator.hpp>

/*============================ PUBLIC API =====================================*/

samsung::WPGenerator::WPGenerator() : m_data_file("data.txt", std::ios::in | std::ios::out |  std::ios::trunc)
{
    if (!m_data_file) 
    {
        throw std::runtime_error("Error: Could not open file for writing");
    }

    // Create a buffer filled with zeros
    char buffer[g_data_size] = {0};

    // Write the zeroed buffer to the file
    m_data_file.write(buffer, g_data_size);
    m_data_file.seekp(0); 

    std::string str = "dummy data for frames";
    m_data_file.write(str.c_str(), str.size());

}

samsung::WPGenerator::~WPGenerator()
{
    m_data_file.close();
}

void samsung::WPGenerator::GenerateBinaryFromFile(std::shared_ptr<samsung::IConfigurator> configurator)
{
    m_config_params = configurator->GetConfigFile();
    std::string binary_filename = "FRAMES.bin";
    Serialize(binary_filename);
    m_dispatcher_new_bin.Notify(binary_filename); 
}

void samsung::WPGenerator::RegisterForNewBinFile(ilrd::Acallback<const std::string&>* obj)
{
    m_dispatcher_new_bin.Register(obj); 
}

/*============================= PRIVATE API ===================================*/

void samsung::WPGenerator::CopyData(char* data)
{
    if (!m_data_file.is_open()) 
    {
        throw std::runtime_error("Error: Data file is not open");

    }

    m_data_file.seekp(0);

    if (!m_data_file.read(data, g_data_size)) 
    {
        throw std::runtime_error("Failed to read file content");
    }
}

void samsung::WPGenerator::Serialize(const std::string &filename)
{
    std::ofstream bin_file(filename, std::ios::out | std::ios::binary); 

    if (!bin_file.is_open()) 
    {
        throw std::runtime_error("Error: Could not open binary file for writing");
    }

    std::vector<Frame> all_frames;
    for(const auto& writing_pattern: m_config_params.writing_patterns)
    {
        for (const auto& memory_write : writing_pattern) 
        {
            std::vector<Frame> all_frames_in_mw = GenerateFramesInMW(memory_write);
            all_frames.insert(all_frames.end(), all_frames_in_mw.begin(), 
                                                                all_frames_in_mw.end());    
        }
    }

    samsung::BinMetaData meta
    {
        .threshold = m_config_params.threshold,
        .delta = m_config_params.delta,
        .total_frames = static_cast<uint32_t>(all_frames.size())
    };
    
    bin_file.write(reinterpret_cast<const char*>(&meta), sizeof(meta));

    for (const auto& frame : all_frames) 
    {
        bin_file.write(reinterpret_cast<const char*>(&frame), 
                                                    sizeof(frame));
    }

    bin_file.close(); 
}

std::vector<samsung::Frame> samsung::WPGenerator::GenerateFramesInMW
                                            (const MemoryWrite& memory_write)
{
    std::vector<samsung::Frame> total_frames; 

    if(memory_write.frames <= 0)
    {
        return total_frames; 
    }

     //original duration is in seconds
    float time_interval_ms = static_cast<float>(memory_write.duration*1000)/memory_write.frames; 
    uint32_t adrress_step = 1; 

    for (uint32_t i = 0; i < memory_write.frames; ++i) 
    {
        Frame frame;
        frame.address = memory_write.start_address + (i * adrress_step);
        frame.time_ms = memory_write.start_time*1000 + (i * time_interval_ms);
        //generate data
        CopyData(frame.data);
        total_frames.push_back(frame);
    }
    
    return total_frames;
}
