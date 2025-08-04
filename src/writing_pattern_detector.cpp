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

samsung::WritingPatternDetector::WritingPatternDetector() : 
                m_delta(0), m_threshold(0), m_sf_detected(false)
{
}

void samsung::WritingPatternDetector::InitDetector(std::shared_ptr<samsung::IConfigurator> configurator)
{
    samsung::ConfigFile config = configurator->GetConfigFile();
    m_delta = config.delta;
    m_threshold = config.threshold;
    m_sf_detected = false; 
    m_writing_patterns.clear();
    m_writing_patterns = config.writing_patterns;
    m_recent_frames.clear(); 
}

void samsung::WritingPatternDetector::ReceiveFlashFrame(const samsung::FlashFrame& 
                                            flash_frame)
{

    if(m_sf_detected)
    {
        return; //??
    }

    m_recent_frames.push_back(std::pair{std::chrono::steady_clock::now(), 
                                                            flash_frame.address});
    RemoveFrames(); 
    

    if(CheckSystemFailure()) 
    {
        m_sf_detected = true; 
        CreateLog(flash_frame.address); 
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


void samsung::WritingPatternDetector::RemoveFrames()
{
    auto delta_duration = std::chrono::seconds(m_delta);
    auto cur_time = std::chrono::steady_clock::now(); 

    while(!m_recent_frames.empty() && 
                    ((cur_time - m_recent_frames.front().first) > delta_duration)) 
    {
        m_recent_frames.pop_front(); 
    }
}

bool samsung::WritingPatternDetector::CheckSystemFailure()
{
    if(m_recent_frames.size() >= m_threshold  && IsConsequent())
    {
        return true; 
    }

    return false;
}

bool samsung::WritingPatternDetector::IsConsequent()
{
    if (m_recent_frames.size() < m_threshold)
    {
        return false;
    }
    size_t start = m_recent_frames.size() - m_threshold;

    for(size_t i = start; i < m_recent_frames.size() - 1; ++i)
    {
        if(m_recent_frames[i + 1].second != m_recent_frames[i].second + 1)
        {
            return false; 
        }
    }

    return true;
}

uint32_t samsung::WritingPatternDetector::FindPatternIndex(uint32_t frame_address)
{
    for (size_t pattern_idx = 0; pattern_idx < m_writing_patterns.size(); 
                                                                    ++pattern_idx) 
    {
        const auto& pattern = m_writing_patterns[pattern_idx];

        for (const auto& mem_write : pattern) 
        {
            uint32_t start = mem_write.start_address;
            uint32_t end = start + mem_write.frames;

            if (frame_address >= start && frame_address < end) 
            {
                return static_cast<uint32_t>(pattern_idx);
            }
        }
    }

    throw std::runtime_error("Invalid Frame Address - does not match any known writing pattern");
}

void samsung::WritingPatternDetector::CreateLog(uint32_t frame_address)
{

    uint32_t pattern_index = FindPatternIndex(frame_address);

    if (pattern_index >= m_writing_patterns.size()) 
    {
        throw std::out_of_range("Invalid pattern index in CreateLog");
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

    if (!m_writing_patterns.empty()) 
    {
        auto addr = m_writing_patterns[pattern_index][0].start_address;
        log_file << "SYSTEM_FAILURE_START ADDRESS: 0x"
                 << std::hex << std::setw(8) << std::setfill('0') << addr << 
                                    " # start address of the writing pattern\n"  << std::dec;
    }
    log_file.flush();

    log_file << "THRESHOLD: "  << m_threshold << " # number of writes\n";
    log_file << "DELTA: " << m_delta << " # sec\n\n";
    std::cout << m_threshold << m_delta << std::endl; 
    log_file << "system_failure_writing pattern:\n\n";
    int count = 1;

    for (const auto& mw : m_writing_patterns[pattern_index]) 
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
