#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Assign a CPU to the System Object
System::System() {
  Processor aCPU;
  cpu_ = aCPU;
}

// Done: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Done: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // place holder variable for each process in the int vector of pids.
  Process temporaryProcess;
  // clear out the old list.
  processes_.clear();
  // get Pids from the LinuxParser and iterate.
  vector<int> procIds = LinuxParser::Pids();
  for (int pid : procIds) {
    temporaryProcess.setPid(pid);
    // Rare case where there is an entry in /proc for a process,
    // but no command can be found in /proc/<pid>/cmdline.
    // Don't show these in our monitor.
    if (temporaryProcess.Command() != "None") {
      processes_.push_back(temporaryProcess);
    }
  }

  // Showing processes with Largest CPU utilization at the top of the list.
  // so sort in reverse order.
  // std::reverse(processes_.begin(), processes_.end());
  sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

// Done: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Done: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Done: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Done: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Done: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Done: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }