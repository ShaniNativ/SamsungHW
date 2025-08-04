/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/

#include "writing_pattern_generator.hpp"
#include "configurator.hpp"

//write deserialization

//gdpp23 src/writing_pattern_generator.cpp src/configurator.cpp test/test_writing_pattern_generator.cpp -I./include -lyaml-cpp -o test_wp.out 
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        throw std::runtime_error("Missing input: config.yaml ");
    }
    std::shared_ptr<samsung::IConfigurator> configurator(new samsung::YamlConfigurator()); 
    configurator->ParseConfigurationFile(argv[1]);

    samsung::WPGenerator generator; 
    generator.GenerateBinaryFromFile(configurator); 
    // yaml_gen.GenerateBinaryFromFile("./config_2.yaml"); //need to handle binary file names 

    return 0; 
}