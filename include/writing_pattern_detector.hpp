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
 *          constraints defined in the YAML configuration file 
 *          (THRESHOLD and DELTA).
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
    WritingPatternDetector(); 

    /*
    * @desc: Init and Resets the internal state of the detector. Should be called between 
             test runs to ensure clean state
    * @params: configurator - shared pointer to IConfigurator
    */
    void InitDetector(std::shared_ptr<samsung::IConfigurator> configurator);

    
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
    using deque_pair = std::pair<std::chrono::time_point<std::chrono::steady_clock>, 
                                                                        uint32_t>;
    std::deque<deque_pair> m_recent_frames; 
    std::vector<std::vector<MemoryWrite>> m_writing_patterns;


    /*
    * @desc: Remove old frames (older then DELTA)
    */
    void RemoveFrames();

    /*
    * @desc: Evaluates whether the current state meets system failure conditions
    * @return: True if failure is detected, false otherwise   
    */
    bool CheckSystemFailure(); 

    /*
    * @desc: Checks if the last THRESHOLD addresses in the qeque are consecutive
    * @return: true if addresses are consecutive, false otherwise   
    */
    bool IsConsequent(); 

    /*
    * @desc: Creates a system failure log file
    * @param: frame_address - The address of the frame where failure was detected   
    */
    void CreateLog(uint32_t frame_address);

    /*
    * @desc: Finds the index of the writing pattern to which a frame belongs
    * @param: frame_address - The address of the frame to search for   
    */
    uint32_t FindPatternIndex(uint32_t frame_address);

};

}

#endif //WRITING_PATTERN_DETECTOR_HPP