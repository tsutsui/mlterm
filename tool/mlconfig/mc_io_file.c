/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include "mc_io.h"

#include <stdio.h>
#include <string.h>
#include <pobl/bl_mem.h>    /* malloc */
#include <pobl/bl_str.h>    /* strdup */
#include <pobl/bl_debug.h>
#include <pobl/bl_conf_io.h>
#include <pobl/bl_locale.h>

#if 0
#define __DEBUG
#endif

/* --- static variables --- */

static bl_conf_write_t *main_config;

/* --- static functions --- */

static char *get_value(bl_conf_write_t *conf, const char *key) {
  u_int count;

  for (count = 0; count < conf->num; count++) {
    if (strncmp(conf->lines[count], key, strlen(key)) == 0) {
      char *value = conf->lines[count] + strlen(key);

      while (*value == ' ' || *value == '\t' || *value == '=') { value ++; }

      return value;
    }
  }

  return NULL;
}

static bl_conf_write_t *open_conf(const char *file) {
  char *path;

  if ((path = bl_get_user_rc_path(file))) {
    bl_conf_write_t *conf = bl_conf_write_open(path);
    free(path);

    return conf;
  }

  return NULL;
}

static void main_config_set(const char *key, const char *value) {
  if (key == NULL) {
    if (main_config) {
      bl_conf_write_close(main_config);
      main_config = NULL;
    }
  } else if (main_config || (main_config = open_conf("mlterm/main"))) {
    bl_conf_io_write(main_config, key, value);
  }
}

static char *main_config_get(const char *key) {
  if (main_config || (main_config = open_conf("mlterm/main"))) {
    return get_value(main_config, key);
  }

  return NULL;
}

/* --- global functions --- */

void mc_exec_file(const char *cmd) {}

void mc_set_str_value_file(const char *key, const char *value) {
  if (value == NULL) {
    return;
  }

  if (strcmp(key, "font_policy") == 0) {
    if (strcmp(value, "unicode") == 0) {
      mc_set_flag_value("only_use_unicode_font", 1);

      return;
    } else if (strcmp(value, "nounicode") == 0) {
      mc_set_flag_value("not_use_unicode_font", 1);

      return;
    } else {
      mc_set_flag_value("only_use_unicode_font", 0);
      mc_set_flag_value("not_use_unicode_font", 0);

      return;
    }
  } else if (strcmp(key, "logging_vt_seq") == 0) {
    if (strcmp(key, "no") == 0) {
      mc_set_flag_value("logging_vt_seq", 0);

      return;
    } else {
      mc_set_flag_value("logging_vt_seq", 1);
      mc_set_str_value("vt_seq_format", value);

      return;
    }
  }

#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " Set %s=%s\n", key, value);
#endif

  main_config_set(key, value);
}

void mc_set_flag_value_file(const char *key, int flag_val) {
#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " Set %s=%s\n", key, flag_val ? "true" : "false");
#endif

  main_config_set(key, flag_val ? "true" : "false");
}

void mc_flush_file(mc_io_t io) {
  main_config_set(NULL, NULL);
}

char *mc_get_str_value_file(const char *key) {
  static struct {
    char *key;
    char *value;
  } options[] = {
    { "alpha", "255" },
    { "auto_detect_encodings", "" },
    { "baseline_offset", "0" },
    { "bd_color", "" },
    { "bel_mode", "sound" },
    { "bg_color", "white" },
    { "bidi_mode", "normal" },
    { "bidi_separators", "" },
    { "bl_color", "" },
    { "box_drawing_font", "noconv" },
    { "brightness", "100" },
    { "click_interval", "250" },
    { "co_color", "" },
    { "col_size_of_width_a", "1" },
    { "cols", "80" },
    { "contrast", "100" },
    { "cursor_bg_color", "" },
    { "cursor_fg_color", "" },
    { "emoji_path", "" },
    { "encoding", "AUTO" },
    { "fade_ratio", "100" },
    { "fg_color", "black" },
    { "fontsize", "16" },
    { "gamma", "100" },
    { "icon_path", "" },
    { "input_method", "xim" },
    { "it_color", "" },
    { "letter_space", "0" },
    { "line_space", "0" },
    { "local_echo_wait", "0" },
    { "logsize", "128" },
    { "mod_meta_key", "none" },
    { "mod_meta_mode", "8bit" },
    { "ot_features", "liga,clig,dlig,hlig,rlig" },
    { "ot_script", "latn" },
    { "pty_list", "/dev/null:1;" },
    { "pty_name", "/dev/null" },
    { "rows", "24" },
    { "rv_color", "" },
    { "sb_bg_color", "" },
    { "sb_fg_color", "" },
    { "screen_width_ratio", "100" },
    { "scrollbar_mode", "left" },
    { "scrollbar_view_name", "simple" },
    { "tabsize", "8" },
    { "type_engine", "xcore" },
    { "ul_color", "" },
    { "underline_offset", "0" },
    { "unicode_full_width_areas", "" },
    { "unicode_half_width_areas", "" },
    { "unicode_noconv_areas", "" },
    { "vertical_mode", "none" },
    { "vt_seq_format", "raw" },
    { "wall_picture", "" },
    { "word_separators", " ,.:;/|@()[]{}" }
  };
  char *value;

#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " Get %s\n", key);
#endif

  if (strcmp(key, "font_policy") == 0) {
    if (mc_get_flag_value("only_use_unicode_font")) {
      return strdup("unicode");
    } else if (mc_get_flag_value("not_use_unicode_font")) {
      return strdup("nounicode");
    } else {
      return strdup("noconv");
    }
  } else if (strcmp(key, "logging_vt_seq") == 0) {
    if (mc_get_flag_value("logging_vt_seq")) {
      return mc_get_str_value("vt_seq_format");
    } else {
      return strdup("no");
    }
  } else if (strcmp(key, "locale") == 0) {
    return strdup(bl_get_locale());
  }

  if ((value = main_config_get(key)) == NULL) {
    int count;

    for (count = 0; count < sizeof(options) / sizeof(options[0]); count++) {
      if (strcmp(options[count].key, key) == 0) {
        return strdup(options[count].value);
      }
    }

    return strdup("");
  } else {
    return strdup(value);
  }
}

int mc_get_flag_value_file(const char *key) {
  static struct {
    char *key;
    int flag;
  } options[] = {
    { "allow_osc52", 0 },
    { "allow_scp", 0, },
    { "auto_detect_encodings", 0 },
    { "borderless", 0 },
    { "logging_vt_seq", 0 },
    { "not_use_unicode_font", 0 },
    { "only_use_unicode_font", 0 },
    { "regard_uri_as_word", 0 },
    { "static_backscroll_mode", 0 },
    { "trim_trailing_newline_in_pasting", 0 },
    { "use_aafont", 0 },
    { "use_alt_buffer", 0 },
    { "use_ansi_colors", 0 },
    { "use_anti_alias", 0 },
    { "use_auto_detect", 0 },
    { "use_bold_font", 0 },
    { "use_clipboard", 0 },
    { "use_combining", 1 },
    { "use_ctl", 1 },
    { "use_dynamic_comb", 0 },
    { "use_italic_font", 0 },
    { "use_local_echo", 0 },
    { "use_multi_column_char", 1 },
    { "use_ot_layout", 0 },
    { "use_transbg", 0 },
    { "use_variable_column_width", 0 },
    { "use_vertical_cursor", 0 }
  };
  char *value;

#ifdef __DEBUG
  bl_debug_printf(BL_DEBUG_TAG " Get %s\n", key);
#endif

  if (strcmp(key, "is_auto_encoding") == 0) {
    value = mc_get_str_value("encoding");
    if (value && strcasecmp(value, "AUTO") == 0) {
      free(value);

      return 1;
    }
  }

  if ((value = main_config_get(key)) == NULL) {
    int count;

    for (count = 0; count < sizeof(options) / sizeof(options[0]); count++) {
      if (strcmp(options[count].key, key) == 0) {
        return options[count].flag;
      }
    }

    return 0;
  }

  if (strcmp(value, "true") == 0) {
    return 1;
  } else {
    return 0;
  }
}

const char *mc_get_gui_file(void) {
#if defined(USE_WIN32GUI)
  return "win32";
#elif defined(USE_QUARTZ)
  return "quartz";
#else
  return "xlib";
#endif
}

void mc_set_font_name_file(mc_io_t io, const char *file, const char *cs, const char *font_name) {
  char *path;
  bl_conf_write_t *conf;

  if ((path = alloca(7 + strlen(file) + 1))) {
    sprintf(path, "mlterm/%s", file);

    if ((path = bl_get_user_rc_path(path))) {
      if ((conf = bl_conf_write_open(path))) {
        bl_conf_io_write(conf, cs, font_name);
        bl_conf_write_close(conf);
      }

      free(path);
    }
  }
}

char *mc_get_font_name_file(const char *file, const char *cs) {
  char *path;
  bl_conf_write_t *conf;
  char *name;

  if ((path = alloca(7 + strlen(file) + 1))) {
    sprintf(path, "mlterm/%s", file);

    if ((path = bl_get_user_rc_path(path))) {
      conf = bl_conf_write_open(path);
      free(path);

      if (conf) {
        name = get_value(conf, cs);
        bl_conf_write_close(conf);

        if (name) {
          return strdup(name);
        }
      }
    }
  }

  return strdup("");
}

void mc_set_color_name_file(mc_io_t io, const char *color, const char *value) {
  char *path;
  bl_conf_write_t *conf;

  if ((path = bl_get_user_rc_path("mlterm/color"))) {
    if ((conf = bl_conf_write_open(path))) {
      bl_conf_io_write(conf, color, value);
      bl_conf_write_close(conf);
    }

    free(path);
  }
}

char *mc_get_color_name_file(const char *color) {
  static char *vt_colors[] = {
    "hl_black", "hl_red", "hl_green", "hl_yellow", "hl_blue", "hl_magenta", "hl_cyan", "hl_white",
  };
  static char *color_rgbs[] = {
    "#000000", "#cd0000", "#00cd00", "#cdcd00", "#0000ee", "#cd00cd", "#00cdcd", "#e5e5e5",
    "#7f7f7f", "#ff0000", "#00ff00", "#ffff00", "#5c5cff", "#ff00ff", "#00ffff", "#ffffff",
  };
  char *path;
  bl_conf_write_t *conf;

  if ((path = bl_get_user_rc_path("mlterm/color"))) {
    char *name;

    conf = bl_conf_write_open(path);
    free(path);

    if (conf) {
      name = get_value(conf, color);
      bl_conf_write_close(conf);

      if (name) {
        return strdup(name);
      }
    }
  }

  if ('0' <= *color && *color <= '9') {
    int color_num = 0;

    do {
      color_num = color_num * 10 + *color - '0';
      color++;
    } while ('0' <= *color && *color <= '9');

    return strdup(color_rgbs[color_num]);
  } else {
    int count;

    for (count = 0; count < sizeof(vt_colors) / sizeof(vt_colors[0]); count++) {
      if (strcasecmp(color, vt_colors[count] + 3) == 0) {
        return strdup(color_rgbs[count]);
      } else if (strcasecmp(color, vt_colors[count]) == 0) {
        return strdup(color_rgbs[count + 8]);
      }
    }
  }

  return strdup("");
}
