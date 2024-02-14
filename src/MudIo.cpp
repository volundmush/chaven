#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include "MudIo.hpp"

#ifdef HAVE_MMAP
#include <sys/mman.h>
#endif

#ifdef ultrix
extern "C"
{
  caddr_t mmap( caddr_t, size_t, int, int, int, off_t );
  int munmap( caddr_t, size_t );
}
#endif

#define CACHE_SIZE 1

#define STR_OPEN 1
#define STR_EOF  2
#define STR_NOTMAPPED 4  // This means mmap() failed and class had to allocate.

#ifndef MAP_FILE
#define MAP_FILE 0  // non BSD systems.
#endif

#ifndef STDOUT_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

int StaticInput::getInt() {
  const char *start;
  //    skipWhite();

  if (isEof()) {
    perror("Input::getInt() - EOF hit before data");
    return 0;
  }

  start = m_pcStart;
  if (!isdigit(*m_pcStart)) {
    if ((*m_pcStart == '-') || (*m_pcStart == '+'))
    m_pcStart++;
    else {
      perror("Input::getInt() - non-numeric character");
      return 0;
    }
  }

  while (isdigit(getch()))
  ;

  if (!isEof())
  putBack();

  skipWhite();
  return atoi(start);
}

float StaticInput::getFloat() {
  const char *start;
  //    skipWhite();

  if (isEof()) {
    perror("Input::getFloat() - EOF hit before data");
    return 0;
  }

  start = m_pcStart;

  if (!isdigit(*m_pcStart) && (*m_pcStart != '-') && (*m_pcStart != '+')) {
    perror("Input::getFloat() - non-numeric character");
    return 0;
  }

  while (isdigit(getch()) || *(m_pcStart - 1) == '.')
  ;

  if (!isEof())
  putBack();

  skipWhite();
  return atof(start);
}

unsigned long StaticInput::getLong() {
  const char *start;
  // skipWhite();

  if (isEof()) {
    perror("Input::getLong() - EOF hit before data");
    return 0;
  }

  start = m_pcStart;

  if (!isdigit(*m_pcStart) && *m_pcStart != '-' && *m_pcStart != '+') {
    perror("Input::getLong() - non-numeric character");
    return 0;
  }

  while (isdigit(getch()))
  ;

  if (!isEof())
  putBack();

  skipWhite();
  return atol(start);
}

void temp_replace(string &str) {
  int x = 0;

  for (size_t loc = str.find("`%"); loc != std::string::npos;
  loc = str.find("`%", loc + 2)) {
    str.replace(loc, 2, "`W");
    x++;
  }
}

char *StaticInput::getWord(char *target) {
  char *start; //= target;

  skipWhite();

  if (isEof()) {
    perror("[Input::getWord] EOF hit before data");
    return 0;
  }

  char end = getch();

  if (end == '#') {
    target[0] = end;
    target[1] = '\0';
    return target;
  }

  if (end == '\'' || end == '"') {
    start = target;
  }
  else {
    target[0] = end;
    start = target + 1;
    end = ' ';
  }

  while (!isEof() && (end == ' ' ? !isspace(*m_pcStart) : *m_pcStart != end)) {
    *start++ = *m_pcStart++;
  }

  *start = '\0';
  getch();
  skipWhite();

  string str(target);
  temp_replace(str);
  strcpy(target, str.c_str());
  return target;
  /*
char * start = target;
//    skipWhite();

if ( isEof() )
{
perror("Input::getWord - EOF hit before data");
return 0;
}

if ( (*m_pcStart == '{') || (*m_pcStart == '}') || (*m_pcStart =='#') || (*m_pcStart == ';'))
{
*target++ 	= *m_pcStart++;
*target		= '\0';
return start;
}

while ( !isEof() && !isspace(*m_pcStart) && *m_pcStart != '{' && *m_pcStart != '}' && *m_pcStart != '#' && *m_pcStart !=';' )
{
*target++ = *m_pcStart++;
}

*target = '\0';
skipWhite();
return start;
*/
}

char *StaticInput::getString(char *target) {
  char *start = target;
  skipWhite();

  if (isEof()) {
    perror("Input::getString - EOF hit before data");
    return 0;
  }

  while (!isEof() && *m_pcStart != TERM_CHAR) {
    if (*m_pcStart != '\r') {
      *target = *m_pcStart;
      if (*target == '\n')
      *(++target) = '\r';
      target++;
    }
    m_pcStart++;
  }

  if (!isEof())
  m_pcStart++;

  *target = '\0';
  skipWhite();

  string str(start);
  temp_replace(str);
  strcpy(start, str.c_str());
  return start;
}

char *StaticInput::getLine(char *target) {
  char *start = target;
  //    skipWhite();

  if (isEof()) {
    perror("Input::getLine - EOF hit before data");
    return 0;
  }

  while (!isEof() && *m_pcStart != '\n')
  *target++ = *m_pcStart++;

  *target = '\0';
  skipWhite();
  return start;
}

void StaticInput::getBitfield(unsigned long *field) {
  int fields = getInt();
  for (int i = 0; i < fields; i++)
  field[i] = getInt();
}

void *StaticInput::read(void *target, int siz) {
  void *start = target;

  if ((m_pcEnd - m_pcStart) < siz) {
    char lbuf[1000];
    sprintf(lbuf, "Input::read - requested %ld more bytes than available.\n\rRequested: %d\n\rHighwater: %p\n\rPtr: %p\n\r", (long)(siz - (m_pcEnd - m_pcStart)), siz, m_pcEnd, m_pcStart);
    perror(lbuf);
    // errorf("Input::read - requested %d more bytes than available.\n\r"
    //                "Requested: %d\n\rHighwater: %x\n\rPtr: %x\n\r", //                (siz - (m_pcEnd - m_pcStart)), siz, m_pcEnd, m_pcStart);
    return 0;
  }
  memcpy(target, m_pcStart, siz);
  m_pcStart += siz;
  return start;
}

void InputFile::open() {
  int fd;

  if ((fd = ::open(m_strFilename.c_str(), O_RDONLY | O_BINARY)) < 0)
  return;

  // Get stats on the file (size is what we need)
  if (fstat(fd, &stats) < 0)
  return;

  // Tell kernel to map this device to memory. If your mmap()
  // is failing then change the define below and it will skip to
  // allocator.
#if (HAVE_MMAP)
  if ((tcache = (char *)mmap(0, stats.st_size + 1, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0)) == (caddr_t)-1)
#endif /* HAVE_MMAP */
  {
    // If mmap fails or is not available then we use our own allocation
    // We lose performance like this but the class internals
    // will still use the whole buffer just like if we mmap()
    int num;
#if !defined(WIN32) && !defined(__CYGWIN32__)
    int tot = 0;
    m_szTCache = new char[stats.st_size + 1];
    while (tot < stats.st_size) {
      if ((num = ::read(fd, (m_szTCache + tot), stats.st_size - tot)) == -1) {
        perror("MPPIStream::open:read");
        exit(0);
      } /* if */
      else if ((tot += num) == stats.st_size)
      break;

      continue;
    } /* while */
    *(m_szTCache + stats.st_size) = 0;
#else
    /*
* WIN32 returns 0 at end of file, and truncates CRLF to LF, returning
* ONLY the number of LFs which means that the normal looping read
* will likely never fill properly.
* The above looping reader is not necessary for WIN32 anyway since
* it can handle large files with one read call (I hope :).
* The routine for WIN32 is a little bit longer in that it makes an effort
* to use the minimal amount of memory for the cache[] given the CRLF
* translation.
*/
    char *temp;
    temp = new char[stats.st_size + 1];
    num = ::read(fd, temp, stats.st_size);
    if (num == -1) {
      perror("MPPIStream::open:read");
      mudpp_exit(0);
    }
    m_szTCache = new char[num + 1];
    memcpy(m_szTCache, temp, num);
    *(m_szTCache + num) = 0;
    delete[] temp;
#endif /* WIN32 */
    m_iFlags |= STR_NOTMAPPED;
  } /* if ...mmap */

  ::close(fd); // We dont need it, its all in RAM now.
  m_szData = m_szTCache;
  m_iFlags |= STR_OPEN;
  m_pcStart = m_szData;
  m_pcEnd = m_szData + stats.st_size;
  skipWhite();
  return;
}

void InputFile::close() {
  if (!m_szData)
  return;

  if (m_iFlags & STR_NOTMAPPED)
  delete[] m_szTCache;

#if (HAVE_MMAP)
  else if ((munmap(m_szTCache, stats.st_size)) < 0) {
    cout << "Unmapped file " << getName() << " of size " << size() << " or "
    << stats.st_size << endl;
    perror("MPPIStream::close:munmap");
    exit(0);
  }
#endif /* HAVE_MMAP */

  m_szData = NULL;
  m_pcStart = NULL;
  m_szTCache = NULL;
}

void Output::write(const void *src, size_t len) {
  if (!len)
  return;

  if ((m_pcStart + len) < m_pcEnd) {
    memcpy(m_pcStart, src, len);
    m_pcStart += len;
  }
  else {
    flush();
    if ((m_pcStart + len) < m_pcEnd) {
      memcpy(m_pcStart, src, len);
      m_pcStart += len;
    }
    else {
      largewrite(src, len);
    }
  }
  return;
}

void Output::putBitfield(const unsigned long *field, int num) {
  *this << num;

  for (int i = 0; i < num; i++) {
    *this << ' ';
    *this << field[i];
  }
}

Output &Output::operator<<(const char *x) {
  if (!x)
  perror("Null pointer passed to Output::operator << char *");
  write(x, strlen(x));
  return *this;
}

Output &Output::operator<<(int x) {
  char p[20];
  sprintf(p, "%d", x);
  write(p, strlen(p));
  return *this;
}

Output &Output::operator<<(long x) {
  char p[20];
  sprintf(p, "%ld", x);
  write(p, strlen(p));
  return *this;
}

Output &Output::operator<<(unsigned long x) {
  char p[20];
  sprintf(p, "%lu", x);
  write(p, strlen(p));
  return *this;
}

Output &Output::operator<<(short int x) {
  char p[20];
  sprintf(p, "%d", x);
  write(p, strlen(p));
  return *this;
}

Output &Output::operator<<(unsigned short int x) {
  char p[20];
  sprintf(p, "%d", x);
  write(p, strlen(p));
  return *this;
}

Output &Output::operator<<(char x) {
  if (m_pcStart >= m_pcEnd)
  flush();

  *m_pcStart++ = x;
  return *this;
}

void OutputFile::open(IOMode mode) {
  if (mode == ioWrite)
  m_iFD = ::open(m_strFilename.c_str(), O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_FILE_MODE);
  else
  m_iFD = ::open(m_strFilename.c_str(), O_BINARY | O_WRONLY | O_CREAT | O_APPEND, DEFAULT_FILE_MODE);

  if (m_iFD == -1) {
    m_bActive = false;
    perror("Cannot open file for writing");
    exit(1);
  }
  m_bActive = true;
}

void OutputFile::flush() {
  if (m_pcStart == m_szBuf)
  return;
  cout << "Writing to file" << endl;
  ::write(m_iFD, m_szBuf, m_pcStart - m_szBuf);
  cout << "Wrote to file" << endl;
  m_pcStart = m_szBuf;
}

void OutputFile::close() {
  flush();
  if (m_iFD == -1)
  return;
  if (!m_bSystemFD)
  ::close(m_iFD);
  m_bActive = false;
}

void OutputFile::largewrite(const void *x, size_t len) {
  if (m_pcStart != m_szBuf)
  ::write(m_iFD, m_szBuf, m_pcStart - m_szBuf);
  ::write(m_iFD, x, len);
  m_pcStart = m_szBuf;
}
