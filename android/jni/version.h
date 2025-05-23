/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#ifndef __VERSION_H__
#define __VERSION_H__

#include <pobl/bl_util.h>

#define MAJOR_VERSION 3
#define MINOR_VERSION 9
#define REVISION 4
#define PATCH_LEVEL 0

#if 0
#define CHANGE_DATE "pre/@CHANGE_DATE@"
#elif 0
#define CHANGE_DATE "post/@CHANGE_DATE@"
#else
#define CHANGE_DATE ""
#endif

#define VERSION \
  BL_INT_TO_STR(MAJOR_VERSION) "." BL_INT_TO_STR(MINOR_VERSION) "." BL_INT_TO_STR(REVISION)

#if PATCH_LEVEL == 0
#define DETAIL_VERSION VERSION " " CHANGE_DATE
#else
#define DETAIL_VERSION VERSION " patch level " BL_INT_TO_STR(PATCH_LEVEL) " " CHANGE_DATE
#endif

#endif
