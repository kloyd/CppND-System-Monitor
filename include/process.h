#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  void setPid(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

  /*
   * Design:
   * Compute process information when Process object is instantiated.
   * Save results from LinuxParser and return values from private members.
  */
 private:
   int pid_;
   std::string user_;
   std::string command_;
   float cpuUtilization_;
   std::string ram_;
   int uptime_;

};

#endif