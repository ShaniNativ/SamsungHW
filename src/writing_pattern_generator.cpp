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

void samsung::WPGenerator::Serialize(const std::string& filename)
{
    std::ofstream bin_file(filename, std::ios::out | std::ios::binary); 

    if (!bin_file.is_open()) 
    {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return; //maybe throw an exceeption???
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
        std::memset(frame.data, 0, g_data_size);
        total_frames.push_back(frame);
    }
    
    return total_frames;
}
