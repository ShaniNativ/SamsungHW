/*******
Written by: Shani Davidian
Reviewed by: 
Date: 31.7.25
Exercise: Samsumg - Work Assigment 
*******/

#include <yaml-cpp/yaml.h> //YAML

#include "configurator.hpp"

void samsung::YamlConfigurator::ParseConfigurationFile(const std::string& filename)
{
    try 
    {
        YAML::Node root = YAML::LoadFile(filename);

        if (!root["THRESHOLD"] || !root["THRESHOLD"].IsScalar())
        {
            throw std::runtime_error("Missing or invalid THRESHOLD in config");
        }

        if (!root["DELTA"] || !root["DELTA"].IsScalar())
        {
            throw std::runtime_error("Missing or invalid DELTA in config");
        }

        m_config_file.threshold = root["THRESHOLD"].as<int>();
        m_config_file.delta = root["DELTA"].as<int>();

        if (root["writing_patterns"] && root["writing_patterns"].IsSequence()) 
        {
            for (const auto& writing_patterns_node : root["writing_patterns"]) 
            {
                std::vector<MemoryWrite> block;

                if (writing_patterns_node.IsSequence()) 
                {
                    for (const auto& memory_write_node : writing_patterns_node) 
                    {
                        if (memory_write_node["memory_write"]) 
                        {
                            MemoryWrite memory_write;
                            memory_write.start_time = memory_write_node["memory_write"]
                                                                ["Start_time"].as<int>();
                            memory_write.duration = memory_write_node["memory_write"]
                                                                ["Duration"].as<int>();
                            std::string addr_str = memory_write_node["memory_write"]
                                                                ["Start_address"].
                                                                as<std::string>();
                            memory_write.start_address = std::stoul(addr_str, nullptr, 0); //str to unsigne dlong
                            memory_write.frames = memory_write_node["memory_write"]
                                                                ["N"].as<int>();
                            block.push_back(memory_write);
                        }
                    }
                }
                m_config_file.writing_patterns.push_back(block);
            }
        }
    } 
    catch (const YAML::Exception& e) 
    {
        throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
    }

}

samsung::ConfigFile samsung::YamlConfigurator::GetConfigFile()
{
    return m_config_file;
}
