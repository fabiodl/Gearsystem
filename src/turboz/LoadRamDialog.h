#ifndef _LOADRAMDIALOG_H_
#define _LOADRAMDIALOG_H_

#include "TExistingFileDialog.h"
#include "AddressFinder.h"


class LoadRamDialog:public TExistingFileDialog{
public:
  LoadRamDialog(AddressFinder& addrFind,const std::string& title);
  virtual Boolean valid( ushort command );
  struct TransferData{
    std::string filename;
    uint32_t src;
    uint32_t dst;
    uint32_t size;        
  };

  TransferData getTransferData();
  void setDirectory(const char* dir);
private:
  static const int ENTRYLENGTH=10;
  struct DialogData{
    char filename[PATH_MAX];
    char src[ENTRYLENGTH];
    char dst[ENTRYLENGTH];
    char size[ENTRYLENGTH];
    DialogData(const char*,const char*,const char*);
    DialogData(){}
  };
  friend std::ostream& operator<<(std::ostream& o, const DialogData& td);
  AddressFinder& addrFind;
 
};

#endif
