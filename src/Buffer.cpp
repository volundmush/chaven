#pragma implementation

#include "Buffer.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
//#include_next "Buffer.h"
#include "const.h"
#define FALSE 0
#define TRUE 1

int Buffer::find_mem_size(int min_size) {
  int i;

  for (i = 64;; i *= 2)
  if (i > min_size)
  return i;

  return -1;
}

Buffer::Buffer(int min_size) {
  if ((size = find_mem_size(min_size)) < 0) {
    exit(1);
  }

  data = (char *)buffer_alloc_mem(size);
  overflowed = FALSE;
  len = 0;
  data[0] = '\0';
}

Buffer::Buffer(const char *initial) {
  if ((size = find_mem_size(strlen(initial) + 1)) < 0) {
    exit(1);
  }

  data = (char *)buffer_alloc_mem(size);
  overflowed = FALSE;
  len = strlen(initial);
  strcpy(data, initial);
}

bool Buffer::strcat(const char *text) {
  int new_size;
  int text_len;
  char *new_data;
  bool have_flushed = false;

  if (overflowed) /* Do not attempt to add anymore if buffer is already
overflowed */
  return FALSE;

  if (!text) /* Adding NULL string ? */
  return TRUE;

  text_len = strlen(text);

  if ((text_len + len) >= MAX_BUF_CPP)
  return FALSE;

  if (text_len == 0) /* Adding empty string ? */
  return TRUE;

  /* Will the combined len of the added text and the current text exceed our
* buffer? */
again:
  if ((text_len + len + 1) > size) /* expand? */
  {
    new_size = find_mem_size(size + text_len + 1);
    if (new_size < 0) /* New size too big ? */
    {
      if (!have_flushed && flush()) // Try to flush buffer so it's empty
      {
        have_flushed = true;
        goto again; /* NASTY UGLY CODE */
      }

      overflowed = TRUE;
      return FALSE;
    }

    new_data = (char *)buffer_alloc_mem(new_size);

    memcpy(new_data, data, len);
    buffer_free_mem(data);
    data = new_data;
    size = new_size;
  }

  memcpy(data + len, text, text_len); /* Start copying */
  len += text_len;                    /* Adjust length */
  data[len] = '\0';                   /* Null-terminate at new end */

  return TRUE;
}

void Buffer::clear() {
  overflowed = FALSE;
  len = 0;
  data[0] = '\0';
}

#ifdef CHECK_PRINTF
#undef printf
#endif

int Buffer::printf(const char *fmt, ...) {
  char buf[2 * MSL];
  va_list va;
  int res;

  va_start(va, fmt);
  res = vsnprintf(buf, 2 * MSL, fmt, va);
  va_end(va);

  if (res >= 2 * MSL - 2) {
    buf[0] = '\0';
  }
  else
  strcat(buf);

  return res;
}
