#include "processor.h"
#include <iostream>
#include "linux_parser.h"

// Done: Return the aggregate CPU utilization
float Processor::Utilization() { return LinuxParser::CpuUtilization(); }