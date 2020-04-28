#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Assign a CPU to the System Object
System::System() {
    Processor aCPU;
    cpu_ =  aCPU;
}

// Done: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Done: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    sort(processes_.begin(), processes_.end());
    return processes_; 
}

// Done: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Done: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Done: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Done: Return the number of processes actively running on the system
int System::RunningProcesses() { 
    // When counting Running Processes, update the vector of Processes
    // for the process list display.
    Process *temporaryProcess;
    /* First, delete any Process objects in the vector.
    for (Process p : processes_) {
        delete &p;
    }
    */
    // get Pids from the LinuxParser and iterate.
    vector<int> procIds = LinuxParser::Pids();
    processes_.clear();
    // Memory management - will the memory used by temporary Process objects
    // be released when the vector is cleared? 

    for (int pid : procIds) {
        temporaryProcess = new Process(pid);
        // Rare case where there is an entry in /proc for a process,
        // but no command can be found in /proc/<pid>/cmdline.
        // Don't show these in our monitor.
        if (temporaryProcess->Command() != "None") {
            processes_.push_back(*temporaryProcess);
        }
        delete temporaryProcess;
    }
    
    return LinuxParser::RunningProcesses(); 
}

// Done: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Done: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }