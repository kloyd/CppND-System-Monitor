#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

/*
 * System class
 * Holds information about overall CPU, vector of processes,
 * Operating System, total processes, up time,
 * and kernel information.
 */

class System {
 public:
  System();
  Processor& Cpu();                   // Done: See src/system.cpp
  std::vector<Process>& Processes();  // Done: See src/system.cpp
  float MemoryUtilization();          // Done: See src/system.cpp
  long UpTime();                      // Done: See src/system.cpp
  int TotalProcesses();               // Done: See src/system.cpp
  int RunningProcesses();             // Done: See src/system.cpp
  std::string Kernel();               // Done: See src/system.cpp
  std::string OperatingSystem();      // Done: See src/system.cpp

  // reference to Processor object and vector of processes.
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif