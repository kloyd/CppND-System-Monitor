#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
 * Process class
 * Represents a single system process.
 * Attributes: pid, username, command, cpu%, memory, uptime.
 * overload operator< to determine how process list is sorted.
*/
class Process {
 public:
  void setPid(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

  /*
   * Design:
   * determine process information when Process object is created.
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