/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#ifndef __VT_LINE_ISCII_H__
#define __VT_LINE_ISCII_H__

#include "../vt_line.h"

#define vt_line_is_using_iscii(line) ((line)->ctl_info_type == VINFO_ISCII)

int vt_line_set_use_iscii(vt_line_t *line, int flag);

int vt_line_iscii_render(vt_line_t *line);

int vt_line_iscii_visual(vt_line_t *line);

int vt_line_iscii_logical(vt_line_t *line);

int vt_line_iscii_convert_logical_char_index_to_visual(vt_line_t *line, int logical_char_index);

int vt_line_iscii_convert_visual_char_index_to_logical(vt_line_t *line, int visual_char_index);

#endif
