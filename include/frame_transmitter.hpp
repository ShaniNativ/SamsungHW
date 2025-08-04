/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#ifndef  FRAME_TRANSMITTER_HPP
#define  FRAME_TRANSMITTER_HPP

#include <string> //std::string, 
#include <queue> //std::priority_queue
#include <chrono> // std::chrono 
#include <utility> //std::pair,
#include <memory> //std::shared_ptr

#include "definitions.hpp" //definitions api
#include "writing_pattern_detector.hpp" //writing_pattern_detector api

namespace samsung
{

/*
* @class: FrameTransmitter
* @desc: Responsible for reading a binary file containing FLASH frames and 
         transmitting them to the WritingPatternDetector at scheduled times.
*/
class FrameTransmitter
{
public:
    /*
    * @desc: Constructs a FrameTransmitter object
    * @params: A shared pointer to an existing WritingPatternDetector that 
    *              will receive the frames.
    */
    FrameTransmitter(std::shared_ptr<WritingPatternDetector> detector);

    /*
    * @desc: Callback method that initiates frame transmission based on their 
    *        intended transmission time
    * @params: bin_filename The path to the binary file to be read
    */
    void Run(const std::string& bin_filename); 

private:
    std::shared_ptr<WritingPatternDetector> m_detector;
    using frame_pair = std::pair<std::chrono::time_point<std::chrono::steady_clock>, 
                                                                    FlashFrame>;
    using frame_pair_queue = std::queue<frame_pair>; 
    
    /*
    * @desc: Parses the binary file and returns a queue of frames
    *        paired with their absolute transmission timestamps.
    *
    * @param: bin_filename The path to the binary file to be read
    * @return: Queue of <timestamp, FlashFrame> pairs, ordered by appearance.
    */
    frame_pair_queue ReadBinFrames(const std::string& bin_filename); 

}; //FrameTransmitter

} //samsung

#endif //FRAME_TRANSMITTER_HPP