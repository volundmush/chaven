#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#if defined(_WIN32)
#include <locale>
#endif

#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

string strUpper(string &str) {
  string val(str);
  transform(val.begin(), val.end(), val.begin(), ::toupper);
  return val;
}

string strLower(string &str) {
  string val(str);
  transform(val.begin(), val.end(), val.begin(), ::tolower);
  return val;
}

string str_rem_color(const string target) {
  string tar(target);
  int x = 0;

  for (size_t loc = tar.find('`'); loc != std::string::npos;
  loc = tar.find('`', loc)) {
    tar.replace(loc, 2, "");
    x++;
  }

  return tar;
}
