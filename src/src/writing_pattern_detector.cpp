/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#include <stdexcept>//std::runtime_error
#include <yaml-cpp/yaml.h> //YAML
#include <fstream> //std::ofstream
#include <unistd.h> // get_login
#include <limits.h> // LOGIN_NAME_MAX
#include <iomanip> //std::setw, std::setfill

#include <iostream> //debugging

#include "writing_pattern_detector.hpp" //detector api

/*============================ PUBLIC API =====================================*/

samsung::WritingPatternDetector::WritingPatternDetector(uint32_t delta, uint32_t threshold) : 
            m_delta(delta), m_threshold(threshold), m_sf_detected(false),
            m_last_frame_address(0), m_curr_mw_start_address(0), m_curr_mw_frames_count(0),
            m_last_interval_sec(0.0), tolerance(0.2), m_is_first_frame_write(true), 
            m_last_frame_time(std::chrono::steady_clock::time_point{}),
            m_curr_mw_start_time(std::chrono::steady_clock::time_point{}), 
            m_start_time_point(std::chrono::steady_clock::now())

{
}

void samsung::WritingPatternDetector::ReceiveFlashFrame(const samsung::FlashFrame& 
                                            flash_frame)
{
    if(m_sf_detected)
    {
        return; //??
    }

    auto now = std::chrono::steady_clock::now();

    if(m_is_first_frame_write)
    {
        m_curr_mw_start_address = flash_frame.address;
        m_curr_mw_start_time = now;
        m_curr_mw_frames_count = 1;
        m_last_frame_time = now;
        m_last_frame_address = flash_frame.address;
        m_is_first_frame_write  = false;
    }
    else
    {
        std::chrono::duration<double> interval = now - m_last_frame_time;
        double interval_sec = interval.count();

        bool is_sequential_address = (flash_frame.address == m_last_frame_address + 1) ? 
                                                            true : false;
        bool is_same_mw = true;
        
        if (m_last_interval_sec > 0.0) 
        {
            is_same_mw = (std::abs(interval_sec - m_last_interval_sec) < tolerance);
        }

        if(!is_sequential_address)
        {
            PushCurrentMemoryWrite(now);
            m_current_pattern.clear();
        }
        else if (!is_same_mw && m_curr_mw_frames_count > 0)
        {
            PushCurrentMemoryWrite(now);
        }
        else
        {
            ++m_curr_mw_frames_count;
        }
        
        m_last_interval_sec = interval_sec;
        m_last_frame_time = now;
        m_last_frame_address = flash_frame.address;
    }

    if(CheckSystemFailure()) 
    {
        m_sf_detected = true; 
        if (m_curr_mw_frames_count > 0)
        {
            PushCurrentMemoryWrite(std::chrono::steady_clock::now());
        }
        CreateLog(); 
    } 
    else
    {
        //send address + data to flash memory
    }
}

bool samsung::WritingPatternDetector::IsSystemFailureDetected()
{
    return m_sf_detected;
}

/*============================ PRIVATE API =====================================*/

void samsung::WritingPatternDetector::PushCurrentMemoryWrite
                                (std::chrono::steady_clock::time_point cur_time)
{
    uint32_t mw_start_time = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
                                        m_curr_mw_start_time - m_start_time_point).count());
    uint32_t mw_duration = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
                                        cur_time - m_curr_mw_start_time).count());

    m_current_pattern.push_back({mw_start_time, mw_duration, m_curr_mw_start_address, 
                                                        m_curr_mw_frames_count});
    m_curr_mw_start_address = m_last_frame_address; //reset vars to the next memory write
    m_curr_mw_start_time = cur_time; 
    m_curr_mw_frames_count = 0;

}

bool samsung::WritingPatternDetector::CheckSystemFailure()
{
    uint32_t total_frames = m_curr_mw_frames_count;
    uint32_t total_time = 0;

    if (!m_current_pattern.empty())
    {
        for (const auto& mw : m_current_pattern)
        {
            total_frames += mw.frames;
        }

        uint32_t start_time = m_current_pattern.front().start_time;
        auto now = std::chrono::steady_clock::now();
        uint32_t now_sec = static_cast<uint32_t>(std::chrono::duration_cast
                    <std::chrono::seconds>(now - m_start_time_point).count());
        total_time = now_sec - start_time;
    }
    else if (m_curr_mw_frames_count > 0)
    {
        total_time = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - m_curr_mw_start_time).count());
    }

    return (total_frames >= m_threshold && total_time <= m_delta);
}

void samsung::WritingPatternDetector::CreateLog()
{
    if(m_current_pattern.empty())
    {
        throw std::runtime_error("There is no data to create log");
    }

    std::ofstream log_file("log.txt");

    if (!log_file.is_open()) {
        throw std::runtime_error("Cannot open log file");
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    char date_buffer[20];
    std::strftime(date_buffer, sizeof(date_buffer), "%d/%m/%Y", &tm);

    char username[LOGIN_NAME_MAX];

    if (getlogin_r(username, sizeof(username)) != 0) 
    {
        throw std::runtime_error("Error getting username");
    }

    log_file << "report type: system failure\n";
    log_file << "user: " << username << "\n";
    log_file << "date: " << date_buffer << "\n\n";


    auto addr = m_current_pattern[0].start_address;
    log_file << "SYSTEM_FAILURE_START ADDRESS: 0x"
                << std::hex << std::setw(8) << std::setfill('0') << addr << 
                                " # start address of the writing pattern\n"  << std::dec;
    
    log_file.flush();

    log_file << "THRESHOLD: "  << m_threshold << " # number of writes\n";
    log_file << "DELTA: " << m_delta << " # sec\n\n";
    std::cout << m_threshold << m_delta << std::endl; 
    log_file << "system_failure_writing pattern:\n\n";
    int count = 1;

    for (const auto& mw : m_current_pattern) 
    {
        log_file << "    # memory write " << count++ << "\n";
        log_file << "    memory_write:\n";
        log_file << "        Start_time: " << mw.start_time << " # start time sec\n";
        log_file << "        Duration: " << mw.duration << " # duration sec\n";
        log_file << "        Start_address: 0x"
                 << std::hex << std::setw(8) << std::setfill('0') << mw.start_address << " # start address\n";
        log_file << "        N: " << std::dec << mw.frames << " # N contiguous frames\n\n";
    }

    log_file.close();
}
