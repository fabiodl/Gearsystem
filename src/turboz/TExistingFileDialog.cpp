#include "TExistingFileDialog.h"
#include "fileUtils.h"

TExistingFileDialog::TExistingFileDialog( const char *aWildCard, const char *aTitle,
                                          const char *inputName, ushort aOptions, uchar histId ):
  TWindowInit( &TFileDialog::initFrame ),
  TFileDialog(aWildCard,aTitle,inputName,aOptions,histId)
{
}


Boolean TExistingFileDialog::valid(ushort command){
  if (!TFileDialog::valid(command)) return false;
  char fileName[PATH_MAX];
  getFileName( fileName );
  if (command==cmFileOpen){
    return file_exists(fileName);
  }else{
    return true;
  }
}


void TExistingFileDialog::setDirectory(const char* dir){
  if (dir){
    setData(const_cast<char*>(dir));
    valid(cmFileOpen);
  }
}
