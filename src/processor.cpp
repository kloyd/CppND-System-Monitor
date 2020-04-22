#include "processor.h"
#include "linux_parser.h"
#include <iostream>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float active, total;
    active = LinuxParser::ActiveJiffies();
    total = LinuxParser::Jiffies();
    return active / total;
}