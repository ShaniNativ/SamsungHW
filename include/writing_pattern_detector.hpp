/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#ifndef  WRITING_PATTERN_DETECTOR_HPP
#define  WRITING_PATTERN_DETECTOR_HPP

#include <time.h> //timer_t
#include <signal.h> //struct sigevent
#include <deque> //std::deque
#include <chrono> // std::chrono 
#include <utility> //std::pair,

#include "definitions.hpp" //definitions api
#include "configurator.hpp" //parsing_yaml api

namespace samsung
{

/*
 * @class: WritingPatternDetector
 * @details This class is responsible for receiving FLASH frames 
 *          during system execution and detecting if a system 
 *          failure occurs based on timing and address continuity 
 *          constraints (THRESHOLD and DELTA).
 *          If a failure is detected, a log file is generated 
 *          and further writes to FLASH are ignored to prevent 
 *          system instability.
*/

class WritingPatternDetector
{
public:

    /*
    * @desc: Constructor 
    * @params: 
    */
    WritingPatternDetector(uint32_t delta, uint32_t threshold); 

    /*
    * @desc: Init and Resets the internal state of the detector. Should be called between 
             test runs to ensure clean state
    * @params: configurator - shared pointer to IConfigurator
    */
    // void InitDetector(std::shared_ptr<samsung::IConfigurator> configurator);

    
    /*
    * @desc: Processes an incoming FLASH frame for system failure detection
    * @params: flash_frame - The received frame containing address and data
    */
    void ReceiveFlashFrame(const samsung::FlashFrame& flash_frame); 

    /*
    * @desc: Returns whether a system failure has been detected
    * @return: True if failure was detected, false otherwise    
    */
    bool IsSystemFailureDetected(); 

private:
    
    uint32_t m_delta;
    uint32_t m_threshold;
    bool m_sf_detected; 
    
    uint32_t m_last_frame_address;

    uint32_t m_curr_mw_start_address;
    uint32_t m_curr_mw_frames_count;
    double m_last_interval_sec;
    const double tolerance; //0.5 ??
    bool m_is_first_frame_write; 
    std::chrono::steady_clock::time_point m_last_frame_time;
    std::chrono::steady_clock::time_point m_curr_mw_start_time;
    std::chrono::steady_clock::time_point m_start_time_point;

    std::vector<MemoryWrite> m_current_pattern;

    void PushCurrentMemoryWrite(std::chrono::steady_clock::time_point cur_time);

    /*
    * @desc: Evaluates whether the current state meets system failure conditions
    * @return: True if failure is detected, false otherwise   
    */
    bool CheckSystemFailure(); 


    /*
    * @desc: Creates a system failure log file
    * @param: frame_address - The address of the frame where failure was detected   
    */
    void CreateLog();
};

}

#endif //WRITING_PATTERN_DETECTOR_HPP