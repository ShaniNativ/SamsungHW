/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#ifndef  WRITING_PATTERN_GENERATOR_HPP
#define  WRITING_PATTERN_GENERATOR_HPP

#include <string> //std::string
#include <vector> //std::vector
#include <memory> //std::shred_ptr

#include "definitions.hpp" //definitions api
#include "dispatcher.hpp"//dispatcher api
#include "configurator.hpp" //parsing_yaml api


namespace samsung
{
/*
 * @class: YamlWPGenerator
 * @desc: A concrete implementation of IWPGenerator for YAML input.
 *          This class parses a YAML configuration file containing
 *          writing patterns and generates a corresponding binary 
 *          file (`FRAMES.bin`) that represents a sequence of frames 
 *          to be written to FLASH memory.
 *          After the binary file is generated, a callback mechanism 
 *          is triggered via an internal dispatcher to notify 
 *          registered observers (FrameTransmitter) that a new 
 *          file is available for processing.
*/
class WPGenerator
{
public:
    /*
    * @desc: Parses a YAML configuration file and generates a binary file
    * @param: config_file that contains writing patterns and global metadata. 
    */
    void GenerateBinaryFromFile(std::shared_ptr<samsung::IConfigurator> configurator);

    /*
    * @desc: Registers an object to be notified a when a new binary file is generated.
    * @params: Pointer to an object implementing Acallback<const std::string&>
    */
    void RegisterForNewBinFile(ilrd::Acallback<const std::string&>* obj); 

private:
    
    samsung::ConfigFile m_config_params; //refernce of not?
    ilrd::Dispatcher<const std::string&> m_dispatcher_new_bin;

    // void GenerateData(char* data, size_t size); //needed??

    /*
    * @desc: Serializes the parsed writing pattern configuration into a 
    *        binary file format.
    * @param: filename - utput binary file name.   
    */
    void Serialize(const std::string& filename);

    /*
    * @desc: Generates a sequence of frames for a given memory write.
    * @param: memory_write - A MemoryWrite object
    * @return A vector of generated Frame objects    
    */
    std::vector<Frame> GenerateFramesInMW(const MemoryWrite& memory_write); 

}; //YamlWPGenerator

}//samsung

#endif //WRITING_PATTERN_GENERATOR_HPP
