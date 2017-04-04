#ifndef _TEXISTINGFILEDIALOG_H_
#define _TEXISTINGFILEDIALOG_H_

#define Uses_TFileDialog
#include <tv.h>

class TExistingFileDialog:public TFileDialog{
public:
  TExistingFileDialog( const char *aWildCard, const char *aTitle,
		 const char *inputName, ushort aOptions, uchar histId );
  Boolean valid(ushort command);
  void setDirectory(const char* dir);
};


#endif
