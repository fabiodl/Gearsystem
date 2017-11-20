#include "HardwareCondition.h"


std::string HardwareCondition::haltReason;
void HardwareCondition::halt(const std::string& reason){
  haltReason=reason;
}

HardwareCondition::HardwareCondition(System& sys){
  init=[]{haltReason="";};
  
  check.preWork=[&sys]{
    /*
      you can use sys here for debug
     */
    if (haltReason.length()>0){
      std::cout<<"Before executing instruction:"<<haltReason<<std::endl;
      return true;
    }else{
      return false;
    }
  };
  
  check.postWork=[]{
    if (haltReason.length()>0){
      std::cout<<"Before executing instruction:"<<haltReason<<std::endl;
      return true;
    }else{
      return false;
    }
  };

}

