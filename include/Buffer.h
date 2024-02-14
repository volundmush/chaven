#ifndef _CPPBUFFER_H
#define _CPPBUFFER_H

#include <stdio.h>
#include <stdlib.h>
//#ifndef WIN32
#include <string.h>
//#endif

#define buffer_alloc_mem(x) malloc(x)
#define buffer_free_mem(x) free(x)

#ifndef __GNUC__
#  define __attribute__(x) /*nothing*/
#endif
#define MAX_BUF_CPP         65536

class Buffer
{
public:
    Buffer (const Buffer&); // deliberate link error, not defined anywhere
    Buffer(int min_size = 1024);
    Buffer(const char *text);
    bool strcat(const char *text);
    int dataLength() { return strlen(data); }
    int printf (const char *fmt, ...) __attribute__((format(printf,2,3)));
    void clear();
    
    char *getBufferData() { return data; }; /* this is bad @@ */
    int getLen() const { return len; }
    
    operator const char *() const { return data; }
    const char *operator~() const { return data; }
    virtual ~Buffer() { buffer_free_mem(data); data=NULL; len=0; }
    // Flush: see if we can get the current output out so there is more space
    virtual bool flush() { return false; }

protected:
    int find_mem_size(int min_size);
    
    int size;       /* Allocated memory size */
    char *data;
    int len;        /* Currently used length */
    bool overflowed;
};
#endif

