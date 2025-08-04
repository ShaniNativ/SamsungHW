/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/
#ifndef  PARSING_YAML_HPP
#define  PARSING_YAML_HPP


#include <string> //std::string

#include "definitions.hpp"

namespace samsung
{

class IConfigurator
{
public:
    virtual void ParseConfigurationFile(const std::string& filename) = 0;
    virtual ConfigFile GetConfigFile() = 0;
private:

};

class YamlConfigurator : public IConfigurator
{
public:
    void ParseConfigurationFile(const std::string& filename) override;
    ConfigFile GetConfigFile() override;
private:
    ConfigFile m_config_file; 
};

} //samsung


#endif //PARSING_YAML_HPP