/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/

#include "writing_pattern_generator.hpp"
#include "frame_transmitter.hpp"
#include "writing_pattern_detector.hpp"



int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        throw std::runtime_error("Missing input: config.yaml ");
    }
    //init configurator
    std::shared_ptr<samsung::IConfigurator> configurator(new samsung::YamlConfigurator()); 
    configurator->ParseConfigurationFile(argv[1]);

    samsung::WPGenerator generator; 

    //init detector
    std::shared_ptr<samsung::WritingPatternDetector> detector = 
                    std::make_shared<samsung::WritingPatternDetector>();
    detector->InitDetector(configurator);

    //init transmitter
    samsung::FrameTransmitter transmitter(detector); 
    //register to generator
    ilrd::Callback<samsung::FrameTransmitter, const std::string&> transmitter_callback(transmitter, &samsung::FrameTransmitter::Run);
    generator.RegisterForNewBinFile(&transmitter_callback);


    generator.GenerateBinaryFromFile(configurator);

    return 0; 
}