/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/

#include "writing_pattern_generator.hpp"
#include "frame_transmitter.hpp"
#include "writing_pattern_detector.hpp"



int main()
{
    //init configurator
    std::shared_ptr<samsung::IConfigurator> configurator(new samsung::YamlConfigurator()); 
    configurator->ParseConfigurationFile("./config_2.yaml");

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