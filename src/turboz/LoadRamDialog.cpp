#include "LoadRamDialog.h"
#include "Placer.h"
#include <cstring>

#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TButton
#include "tv.h"


std::ostream& operator<<(std::ostream& o, const LoadRamDialog::TransferData& td){
  return o<<"from "<<td.filename<<" "<<td.src<<" to "<<td.dst<<" size "<<td.size;
}


std::ostream& operator<<(std::ostream& o, const LoadRamDialog::DialogData& td){
  return o<<"Dialogdata from "<<td.filename<<" "<<td.src<<" to "<<td.dst<<" size "<<td.size;
}
  
LoadRamDialog::DialogData::DialogData(const char* _src,const char* _dst,const char* _size){
  filename[0]=0;
  strcpy(src,_src);
  strcpy(dst,_dst);
  strcpy(size,_size);
}


LoadRamDialog::LoadRamDialog(AddressFinder& _addrFind,const std::string& title):
  TWindowInit( &TFileDialog::initFrame ),
  TExistingFileDialog("*", title.c_str(), "~N~ame", fdOpenButton, 100 ),
  addrFind(_addrFind)
{  
  Placer p(35,6);
  static const int INPUTS=3;
  static const int LABELENGTH=10;

  const char* labels[INPUTS]={"Src","Dst","Size"};

  for (int i=0;i<INPUTS;i++){
    TRect labelRect=p.place(LABELENGTH,1);
    p.newLine();
    p.space(2);
    TInputLine* il=new TInputLine(p.place(ENTRYLENGTH,1),ENTRYLENGTH);  
    insert(il);
    insert(new TLabel(labelRect,labels[i],il));
    p.newLine();      
  }
  DialogData dd("0000","C000","200");
  TDialog::setData(&dd); 
  p.newLine();
  selectNext(false);  
}

Boolean LoadRamDialog::valid( ushort command ){
  TransferData td(getTransferData());
  
  return command==cmCancel ||
    ( TExistingFileDialog::valid(command) &&
      td.src!=AddressFinder::INVALIDADDR &&
      td.dst!=AddressFinder::INVALIDADDR &&
      td.size!=AddressFinder::INVALIDADDR && static_cast<uint16_t>(td.size)+ static_cast<uint16_t>(td.dst)<0xFFFF
      );
}

void LoadRamDialog::setDirectory(const char* dir){
  if (dir){
    DialogData dd;
    TDialog::getData(&dd);
    strcpy(dd.filename,dir);
    setData(&dd);
    valid(cmFileOpen);
  }
}


LoadRamDialog::TransferData LoadRamDialog::getTransferData(){
  DialogData dd;
  TDialog::getData(&dd);
  TransferData td;
  char fileName[PATH_MAX];
  getFileName( fileName );
  td.filename=fileName;
  td.size=AddressFinder::getHex(dd.size);
  td.src=addrFind.getAddressExtended(dd.src);
  td.dst=addrFind.getAddressExtended(dd.dst);
  //std::cout<<"now tradata is "<<td<<std::endl;
  return td;
}
