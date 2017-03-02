#ifndef _CISTRING_H_
#define _CISTRING_H_

#include <string>

class ichar_traits:public std::char_traits<char> {
  public:
    static bool eq( char c1, char c2 ) { return toupper( c1 ) == toupper( c2 ); }
    static bool ne( char c1, char c2 ) { return toupper( c1 ) != toupper( c2 ); }
    static bool lt( char c1, char c2 ) { return toupper( c1 ) <  toupper( c2 ); }        
    static int compare(const char_type* s1, const char_type* s2, std::size_t count ){
      for (size_t i=0;i<count;i++){
        if (lt(s1[i],s2[i])) return -1;
        if (lt(s2[i],s1[i])) return +1;
      }
      return 0;
    }
  };



typedef std::basic_string<char,ichar_traits> cistring;


inline  cistring& istring(std::string &s) {
  return reinterpret_cast<cistring&>(s);
}
inline const cistring& istring(const std::string &s) {
  return reinterpret_cast<const cistring&>(s);
}



#endif



