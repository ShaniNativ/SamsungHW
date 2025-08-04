/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#include <iostream> //std::cerr, 
#include <fstream> //std::ifstream
#include <cstring> //memcpy

#include "frame_transmitter.hpp"

/*============================ PUBLIC API =====================================*/

samsung::FrameTransmitter::FrameTransmitter(std::shared_ptr<WritingPatternDetector> detector) : 
                                                             m_detector(detector)
{

}

void samsung::FrameTransmitter::Run(const std::string& bin_filename)
{
    frame_pair_queue frames_queue =  ReadBinFrames(bin_filename);

    while (!frames_queue.empty())
    {
        auto scheduled_time = frames_queue.front().first;
        auto flash_frame = frames_queue.front().second;
        frames_queue.pop();

        auto now = std::chrono::steady_clock::now();

        if (scheduled_time > now)
        {
            auto sleep_duration = scheduled_time - now;
            struct timespec ts;
            ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(sleep_duration).count();
            ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_duration).count() % 1'000'000'000;
            clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
        }

        m_detector->ReceiveFlashFrame(flash_frame);

        if (m_detector->IsSystemFailureDetected())
        {
            break;
        }

    }

}

/*============================ PRIVATE API =====================================*/

samsung::FrameTransmitter::frame_pair_queue samsung::FrameTransmitter::ReadBinFrames
                                                (const std::string& bin_filename)
{
    std::ifstream bin_file(bin_filename, std::ios::binary);
    samsung::FrameTransmitter::frame_pair_queue frames_queue; 
    if (!bin_file.is_open()) 
    {
        throw std::runtime_error("Failed to open binary file: " + bin_filename);
    }

    samsung::BinMetaData meta_data; 
    bin_file.read(reinterpret_cast<char*>(&meta_data), sizeof(samsung::BinMetaData));

    auto curr_time = std::chrono::steady_clock::now();

    for(uint32_t i = 0; i < meta_data.total_frames; ++i)
    {
        samsung::Frame temp_frame; 
        bin_file.read(reinterpret_cast<char*>(&temp_frame), sizeof(samsung::Frame));
        auto scheduled_time = curr_time  + std::chrono::milliseconds(temp_frame.time_ms);
        samsung::FlashFrame flash_frame;
        flash_frame.address = temp_frame.address; 
        memcpy(flash_frame.data, temp_frame.data, g_data_size); 
        frames_queue.push({scheduled_time, flash_frame});
    }

    bin_file.close(); 

    return frames_queue; 
}
