/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

/*
 * _GNU_SOURCE must be defined before including <features.h> to take effect.
 * since standard headers, bl_types.h and bl_def.h include features.h
 * indirectly,
 * ecplicitly evaluate only the autoconf's result here.
 * (for ptsname)
 */
#include <pobl/bl_config.h>
#ifdef HAVE_GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "vt_pty_intern.h"

#include <pobl/bl_debug.h>
#include <pobl/bl_mem.h> /* realloc/alloca */
#include <pobl/bl_path.h>
#include <pobl/bl_str.h>
#include <pobl/bl_util.h> /* DIGIT_STR_LEN */
#include <string.h>
#include <unistd.h> /* ttyname/pipe */
#include <stdio.h>  /* sscanf */
#include <stdlib.h> /* ptsname */
#include <fcntl.h>  /* fcntl/O_BINARY */
#ifdef USE_WIN32API
#include <windows.h>
#endif

#if 0
#define __DEBUG
#endif

/* --- global functions --- */

vt_pty_t *vt_pty_new(const char *cmd_path, /* can be NULL */
                     char **cmd_argv,      /* can be NULL(only if cmd_path is NULL) */
                     char **env,           /* can be NULL */
                     const char *host,     /* DISPLAY env or remote host */
                     const char *work_dir, /* can be NULL */
                     const char *pass,     /* can be NULL */
                     const char *pubkey,   /* can be NULL */
                     const char *privkey,  /* can be NULL */
                     u_int cols, u_int rows, u_int width_pix, u_int height_pix) {
  vt_pty_t *pty;

#ifndef USE_WIN32API
  if (!pass) {
    pty =
        vt_pty_unix_new(cmd_path, cmd_argv, env, host, work_dir, cols, rows, width_pix, height_pix);
  } else
#endif
  {
#if defined(USE_LIBSSH2)
    pty = vt_pty_ssh_new(cmd_path, cmd_argv, env, host, pass, pubkey, privkey, cols, rows,
                         width_pix, height_pix);
#elif defined(USE_WIN32API)
    pty = vt_pty_pipe_new(cmd_path, cmd_argv, env, host, pass, cols, rows);
#else
    pty = NULL;
#endif
  }

  if (pty) {
    vt_config_menu_init(&pty->config_menu);
  }

  return pty;
}

vt_pty_ptr_t vt_pty_new_with(int master, int slave, pid_t child_pid, u_int cols, u_int rows,
                             u_int width_pix, u_int height_pix) {
  vt_pty_t *pty;

#ifndef USE_WIN32API
  if (ptsname(master)) {
    pty = vt_pty_unix_new_with(master, slave, child_pid, ":0.0", cols, rows, width_pix, height_pix);
  } else
#endif
  {
    pty = NULL;
  }

  vt_config_menu_init(&pty->config_menu);

  return pty;
}

int vt_pty_delete(vt_pty_t *pty) {
#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " vt_pty_delete is called for %p.\n", pty);
#endif

  if (pty->pty_listener && pty->pty_listener->closed) {
    (*pty->pty_listener->closed)(pty->pty_listener->self);
  }
#ifdef DEBUG
  else {
    bl_debug_printf(BL_DEBUG_TAG " %s is not set.\n",
                    pty->pty_listener ? "pty_listener->closed" : "pty listener");
  }
#endif

  free(pty->buf);
  free(pty->cmd_line);
  vt_config_menu_final(&pty->config_menu);

  (*pty->final)(pty);

  free(pty);

  return 1;
}

int vt_set_pty_winsize(vt_pty_t *pty, u_int cols, u_int rows, u_int width_pix, u_int height_pix) {
  return (*pty->set_winsize)(pty, cols, rows, width_pix, height_pix);
}

int vt_pty_set_listener(vt_pty_t *pty, vt_pty_event_listener_t *pty_listener) {
  pty->pty_listener = pty_listener;

  return 1;
}

/*
 * Return size of lost bytes.
 */
size_t vt_write_to_pty(vt_pty_t *pty, u_char *buf, size_t len /* if 0, flushing buffer. */
                       ) {
  u_char *w_buf;
  size_t w_buf_size;
  ssize_t written_size;
  void *p;

  w_buf_size = pty->left + len;
  if (w_buf_size == 0) {
    return 0;
  }
#if 0
  /*
   * Little influence without this buffering.
   */
  else if (len > 0 && w_buf_size < 16) {
    /*
     * Buffering until 16 bytes.
     */

    if (pty->size < 16) {
      if ((p = realloc(pty->buf, 16)) == NULL) {
#ifdef DEBUG
        bl_warn_printf(BL_DEBUG_TAG " realloc failed. %d characters not written.\n", len);
#endif

        return len;
      }

      pty->size = 16;
      pty->buf = p;
    }

    memcpy(&pty->buf[pty->left], buf, len);
    pty->left = w_buf_size;

#if 0
    bl_debug_printf("buffered(not written) %d characters.\n", pty->left);
#endif

    return 0;
  }
#endif

  if (/* pty->buf && */ len == 0) {
    w_buf = pty->buf;
  } else if (/* pty->buf == NULL && */ pty->left == 0) {
    w_buf = buf;
  } else if ((w_buf = alloca(w_buf_size))) {
    memcpy(w_buf, pty->buf, pty->left);
    memcpy(&w_buf[pty->left], buf, len);
  } else {
#ifdef DEBUG
    bl_warn_printf(BL_DEBUG_TAG " alloca() failed. %d characters not written.\n", len);
#endif

    return len;
  }

#ifdef __DEBUG
  {
    int i;
    for (i = 0; i < w_buf_size; i++) {
      bl_msg_printf("%.2x", w_buf[i]);
    }
    bl_msg_printf("\n");
  }
#endif

  if (pty->hook) {
    if (!pty->hook->pre_write) {
      written_size = w_buf_size;

      goto written;
    } else {
      written_size = (*pty->hook->pre_write)(pty->hook->self, w_buf, w_buf_size);
    }
  }

  written_size = (*pty->write)(pty, w_buf, w_buf_size);

written:
  if (written_size < 0) {
#ifdef DEBUG
    bl_warn_printf(BL_DEBUG_TAG " write() failed.\n");
#endif
    written_size = 0;
  }

  if (written_size == w_buf_size) {
    pty->left = 0;

    return 0;
  }

  /* w_buf_size - written_size == not_written_size */
  if (w_buf_size - written_size > pty->size) {
    if ((p = realloc(pty->buf, w_buf_size - written_size)) == NULL) {
      size_t lost;

      if (pty->size == 0) {
        lost = w_buf_size - written_size;
        pty->left = 0;
      } else {
        lost = w_buf_size - written_size - pty->size;
        memcpy(pty->buf, &w_buf[written_size], pty->size);
        pty->left = pty->size;
      }

#ifdef DEBUG
      bl_warn_printf(BL_DEBUG_TAG " realloc failed. %d characters are not written.\n", lost);
#endif

      return lost;
    } else {
      pty->size = pty->left = w_buf_size - written_size;
      pty->buf = p;
    }
  } else {
    pty->left = w_buf_size - written_size;
  }

  memcpy(pty->buf, &w_buf[written_size], pty->left);

#if 0
  bl_debug_printf("%d is not written.\n", pty->left);
#endif

  return 0;
}

size_t vt_read_pty(vt_pty_t *pty, u_char *buf, size_t left) {
  size_t read_size;

  read_size = 0;
  while (1) {
    ssize_t ret;

    ret = (*pty->read)(pty, &buf[read_size], left);
    if (ret <= 0) {
      return read_size;
    } else {
      read_size += ret;
      left -= ret;
    }
  }
}

void vt_response_config(vt_pty_t *pty, char *key, char *value, int to_menu) {
  char *res;
  char *fmt;
  size_t res_len;

  res_len = 1 + strlen(key) + 1;
  if (value) {
    res_len += (1 + strlen(value));
    fmt = "#%s=%s\n";
  } else {
    fmt = "#%s\n";
  }

  if (!(res = alloca(res_len + 1))) {
    res = "#error\n";
  }

  sprintf(res, fmt, key, value);

#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " %s\n", res);
#endif

  if (to_menu < 0) {
    if (pty->pty_listener && pty->pty_listener->show_config) {
      /* '\n' -> '\0' */
      res[strlen(res) - 1] = '\0';
      (*pty->pty_listener->show_config)(pty->pty_listener->self, res + 1);
    }
  } else if (to_menu > 0) {
    vt_config_menu_write(&pty->config_menu, res, res_len);
  } else {
    vt_write_to_pty(pty, res, res_len);
  }
}

pid_t vt_pty_get_pid(vt_pty_t *pty) { return pty->child_pid; }

int vt_pty_get_master_fd(vt_pty_t *pty) { return pty->master; }

/* Return: slave fd or -1 */
int vt_pty_get_slave_fd(vt_pty_t *pty) { return pty->slave; }

/*
 * Always return non-NULL value.
 * XXX Static data can be returned. (Not reentrant)
 */
char *vt_pty_get_slave_name(vt_pty_t *pty) {
  static char virt_name[9 + DIGIT_STR_LEN(int)+1];
#ifndef USE_WIN32API
  char *name;

  if (pty->slave >= 0 && (name = ttyname(pty->slave))) {
    return name;
  }
#endif

/* Virtual pty name */
#ifdef USE_LIBSSH2
  sprintf(virt_name, "/dev/vpty%d", ((pty->child_pid >> 1) & 0xfff)); /* child_pid == channel */
#else
  sprintf(virt_name, "/dev/vpty%d", pty->master);
#endif

  return virt_name;
}

int vt_start_config_menu(vt_pty_t *pty, char *cmd_path, int x, int y, char *display) {
  return vt_config_menu_start(&pty->config_menu, cmd_path, x, y, display, pty);
}

char *vt_pty_get_cmd_line(vt_pty_t *pty) { return pty->cmd_line; }

void vt_pty_set_hook(vt_pty_t *pty, vt_pty_hook_t *hook) { pty->hook = hook; }