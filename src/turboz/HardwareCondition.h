#ifndef _HARDWARECONDITION_H_
#define _HARDWARECONDITION_H_

#include "ExecutionWindow.h"

class HardwareCondition:public ExecutionWindow::HaltCondition{
public:
  HardwareCondition(System& sys);
  static void halt(const std::string& reason);
private:
  static std::string haltReason;
};


#endif
