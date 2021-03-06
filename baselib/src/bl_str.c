/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include "bl_str.h"

#include <stdio.h>  /* sprintf */
#include <stdarg.h> /* va_list */
#include <ctype.h>  /* isdigit */

#include "bl_debug.h"
#include "bl_mem.h"

#undef bl_str_sep
#undef bl_basename

/* --- global functions --- */

#ifndef HAVE_STRSEP

char* __bl_str_sep(char **strp, const char *delim) {
  char *s;
  const char *spanp;
  int c;
  int sc;
  char *tok;

  if ((s = *strp) == NULL) {
    return NULL;
  }

  for (tok = s;;) {
    c = *s++;
    spanp = delim;
    do {
      if ((sc = *spanp++) == c) {
        if (c == 0) {
          s = NULL;
        } else {
          s[-1] = 0;
        }

        *strp = s;

        return tok;
      }
    } while (sc != 0);
  }
}

#endif

/*
 * !! Notice !!
 * It is a caller that is responsible to check buffer overrun.
 */
int bl_snprintf(char *str, size_t size, const char *format, ...) {
  va_list arg_list;

  va_start(arg_list, format);

#ifdef HAVE_SNPRINTF
  return vsnprintf(str, size, format, arg_list);
#else
  /*
   * XXX
   * this may cause buffer overrun.
   */

  return vsprintf(str, format, arg_list);
#endif
}

char *__bl_str_dup(const char *str, const char *file /* should be allocated memory. */,
                   int line, const char *func /* should be allocated memory. */) {
  char *new_str;

  if ((new_str = bl_mem_malloc(strlen(str) + 1, file, line, func)) == NULL) {
    return NULL;
  }

  strcpy(new_str, str);

  return new_str;
}

#ifndef REMOVE_FUNCS_MLTERM_UNUSE
/*
 * XXX
 * this doesn't concern about ISO2022 sequences or so.
 * dst/src must be u_char since 0x80 - 0x9f is specially dealed.
 */
size_t bl_str_tabify(u_char *dst, size_t dst_len, const u_char *src, size_t src_len,
                     size_t tab_len) {
  size_t pos_in_tab;
  size_t space_num;
  int dst_pos;
  int src_pos;
  int count;

  if (tab_len == 0) {
#ifdef BL_DEBUG
    bl_warn_printf(BL_DEBUG_TAG " 0 is illegal tab length.\n");
#endif

    return 0;
  }

  dst_pos = 0;
  pos_in_tab = 0;
  space_num = 0;
  for (src_pos = 0; src_pos < src_len; src_pos++) {
    if (src[src_pos] == ' ') {
      if (pos_in_tab == tab_len - 1) {
        dst[dst_pos++] = '\t';

        if (dst_pos >= dst_len) {
          return dst_pos;
        }

        space_num = 0;

        /* next */
        pos_in_tab = 0;
      } else {
        space_num++;

        /* next */
        pos_in_tab++;
      }
    } else {
      if (space_num > 0) {
        for (count = 0; count < space_num; count++) {
          dst[dst_pos++] = ' ';

          if (dst_pos >= dst_len) {
            return dst_pos;
          }
        }

        space_num = 0;
      }

      dst[dst_pos++] = src[src_pos];

      if (dst_pos >= dst_len) {
        return dst_pos;
      }

      if (src[src_pos] == '\n' || src[src_pos] == '\t') {
        /* next */
        pos_in_tab = 0;
      } else if ((0x20 <= src[src_pos] && src[src_pos] < 0x7f) || 0xa0 <= src[src_pos]) {
        /* next */
        if (pos_in_tab == tab_len - 1) {
          pos_in_tab = 0;
        } else {
          pos_in_tab++;
        }
      } else if (src[src_pos] == 0x1b) {
        /* XXX  ISO2022 seq should be considered. */
      }
    }
  }

  if (space_num > 0) {
    for (count = 0; count < space_num; count++) {
      dst[dst_pos++] = ' ';

      if (dst_pos >= dst_len) {
        return dst_pos;
      }
    }
  }

  return dst_pos;
}
#endif

char *bl_str_chop_spaces(char *str) {
  size_t pos;

  pos = strlen(str);

  while (pos > 0) {
    pos--;

    if (str[pos] != ' ' && str[pos] != '\t') {
      str[pos + 1] = '\0';

      break;
    }
  }

  return str;
}

int bl_str_n_to_uint(u_int *i, const char *s, size_t n) {
  u_int _i;
  int digit;

  if (n == 0) {
    return 0;
  }

  _i = 0;
  for (digit = 0; digit < n && s[digit]; digit++) {
    if (!isdigit(s[digit])) {
      return 0;
    }

    _i *= 10;
    _i += (s[digit] - 0x30);
  }

  *i = _i;

  return 1;
}

int bl_str_n_to_int(int *i, const char *s, size_t n) {
  u_int _i;
  int is_minus;

  if (n == 0) {
    return 0;
  }

  if (*s == '-') {
    if (--n == 0) {
      return 0;
    }

    s++;

    is_minus = 1;
  } else {
    is_minus = 0;
  }

  if (!bl_str_n_to_uint(&_i, s, n)) {
    return 0;
  }

  if ((int)_i < 0) {
    return 0;
  }

  if (is_minus) {
    *i = -((int)_i);
  } else {
    *i = (int)_i;
  }

  return 1;
}

int bl_str_to_uint(u_int *i, const char *s) {
  u_int _i;

  if (*s == '\0') {
    return 0;
  }

  _i = 0;
  while (*s) {
    if (!isdigit(*s)) {
      return 0;
    }

    _i *= 10;
    _i += (*s - 0x30);

    s++;
  }

  *i = _i;

  return 1;
}

int bl_str_to_int(int *i, const char *s) {
  u_int _i;
  int is_minus;

  if (*s == '\0') {
    return 0;
  }

  if (*s == '-') {
    if (*(++s) == '\0') {
      return 0;
    }

    is_minus = 1;
  } else {
    is_minus = 0;
  }

  if (!bl_str_to_uint(&_i, s)) {
    return 0;
  }

  if ((int)_i < 0) {
    return 0;
  }

  if (is_minus) {
    *i = -((int)_i);
  } else {
    *i = (int)_i;
  }

  return 1;
}

u_int bl_count_char_in_str(const char *str, char ch) {
  u_int count;

  count = 0;
  while (*str) {
    if (*str == ch) {
      count++;
    }

    str++;
  }

  return count;
}

/* str1 and str2 can be NULL */
int bl_compare_str(const char *str1, const char *str2) {
  if (str1 == str2) {
    return 0;
  }

  if (str1 == NULL) {
    return -1;
  } else if (str2 == NULL) {
    return 1;
  }

  return strcmp(str1, str2);
}

char *bl_str_replace(const char *str, const char *orig, /* Don't specify "". */
                     const char *new) {
  size_t orig_len;
  size_t new_len;
  int diff;
  const char *p;
  char *new_str;
  char *dst;

  orig_len = strlen(orig);
  new_len = strlen(new);
  if ((diff = new_len - orig_len) != 0) {
    int num;

    for (num = 0, p = str; (p = strstr(p, orig)); num++, p += orig_len);

    if (num == 0) {
      return NULL;
    }

    diff *= num;
  }

  if (!(p = strstr(str, orig)) || !(dst = new_str = malloc(strlen(str) + diff + 1))) {
    return NULL;
  }

  do {
    memcpy(dst, str, p - str);
    dst += (p - str);
    memcpy(dst, new, new_len);
    dst += new_len;
    str = p + orig_len;
  } while ((p = strstr(str, orig)));
  strcpy(dst, str);

  return new_str;
}

#if 0
char *bl_str_escape_backslash(char *str) {
  char *escaped_str;
  char *p;

  if (!(p = escaped_str = malloc(strlen(str) + bl_count_char_in_str(str, '\\') + 1))) {
    return str;
  }

  while (1) {
    *(p++) = *str;

    if (*str == '\0') {
      g_free(str);

      return escaped_str;
    } else if (*str == '\\') {
      *(p++) = '\\';
    }

    str++;
  }
}
#endif

char *bl_str_unescape(const char *str) {
  char *new_str;
  char *p;

  if ((new_str = malloc(strlen(str) + 1)) == NULL) {
    return NULL;
  }

  for (p = new_str; *str; str++, p++) {
    if (*str == '\\') {
      u_int digit;

      if (*(++str) == '\0') {
        break;
      } else if (sscanf(str, "x%2x", &digit) == 1) {
        *p = (char)digit;
        str += 2;
      } else if (*str == 'n') {
        *p = '\n';
      } else if (*str == 'r') {
        *p = '\r';
      } else if (*str == 't') {
        *p = '\t';
      } else if (*str == 'e' || *str == 'E') {
        *p = '\033';
      } else {
        *p = *str;
      }
    } else if (*str == '^') {
      if (*(++str) == '\0') {
        break;
      } else if ('@' <= *str && *str <= '_') {
        *p = *str - 'A' + 1;
      } else if (*str == '?') {
        *p = '\x7f';
      } else {
        *p = *str;
      }
    } else {
      *p = *str;
    }
  }

  *p = '\0';

  return new_str;
}

#ifdef BL_DEBUG

#include <assert.h>

void TEST_bl_str(void) {
  char *str;

  str = bl_str_replace("abcdefabcdef", "abc", "xxxx");
  assert(strcmp(str, "xxxxdefxxxxdef") == 0);
  free(str);

  str = bl_str_unescape("abc\\n\\r\\t\\e\\E^A\\x1b");
  assert(strcmp(str, "abc\n\r\t\x1b\x1b\x01\x1b") == 0);
  free(str);

  bl_msg_printf("PASS bl_str test.\n");
}

#endif
