/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include "ef_jis_property.h"

/*
 * COMBINING_ENCLOSING_CIRCLE is regarded as LARGE_CIRCLE in JISX0208_1997.
 */
#if 0
#define JISX0208_COMBINING_ENCLOSING_CIRCLE
#endif

/* --- static variables --- */

static u_int8_t jisx0213_2000_1_property_table[] = {
    /* 0x2b52 */
    EF_COMBINING,             /* COMBINING DOUBLE INVERTED BREVE */
    0, 0, 0, 0, EF_COMBINING, /* COMBINING BREVE */
    0, EF_COMBINING,          /* COMBINING DOUBLE ACUTE ACCENT */
    EF_COMBINING,             /* COMBINING ACUTE ACCENT */
    EF_COMBINING,             /* COMBINING MACRON */
    EF_COMBINING,             /* COMBINING GRAVE ACCENT */
    EF_COMBINING,             /* COMBINING DOUBLE GRAVE ACCENT */
    EF_COMBINING,             /* COMBINING CARON */
    EF_COMBINING,             /* COMBINING CIRCUMFLEX ACCENT */

    /* 0x2b60 */
    0, 0, 0, 0, 0, 0, 0, EF_COMBINING, /* COMBINING RING BELOW */
    EF_COMBINING,                      /* COMBINING CARON BELOW */
    EF_COMBINING,                      /* COMBINING RIGHT HALF RING BELOW */
    EF_COMBINING,                      /* COMBINING LEFT HALF RING BELOW */
    EF_COMBINING,                      /* COMBINING PLUS SIGN BELOW */
    EF_COMBINING,                      /* COMBINING MINUS SIGN BELOW */
    EF_COMBINING,                      /* COMBINING DIAERESIS */
    EF_COMBINING,                      /* COMBINING X ABOVE */
    EF_COMBINING,                      /* COMBINING VERTICAL LINE BELOW */

    /* 0x2b70 */
    EF_COMBINING,    /* COMBINING INVERTED BREVE BELOW */
    0, EF_COMBINING, /* COMBINING DIAERESIS BELOW */
    EF_COMBINING,    /* COMBINING TILDE BELOW */
    EF_COMBINING,    /* COMBINING SEAGULL BELOW */
    EF_COMBINING,    /* COMBINING TILDE OVERLAY */
    EF_COMBINING,    /* COMBINING UP TACK BELOW */
    EF_COMBINING,    /* COMBINING DOWN TACK BELOW */
    EF_COMBINING,    /* COMBINING LEFT TACK BELOW */
    EF_COMBINING,    /* COMBINING RIGHT TACK BELOW */
    EF_COMBINING,    /* COMBINING BRIDGE BELOW */
    EF_COMBINING,    /* COMBINING INVERTED BRIDGE BELOW */
    EF_COMBINING,    /* COMBINING SQUARE BELOW */
    EF_COMBINING,    /* COMBINING TILDE */
    EF_COMBINING,    /* COMBINING LEFT ANGLE ABOVE */
};

/* --- global functions --- */

ef_property_t ef_get_jisx0208_1983_property(u_char *ch /* 2 bytes */) {
#ifdef JISX0208_COMBINING_ENCLOSING_CIRCLE
  if (memcmp(ch, "\x22\x7e", 2) == 0) {
    return EF_COMBINING;
  }
#endif

  return 0;
}

ef_property_t ef_get_jisx0213_2000_1_property(u_char *ch /* 2 bytes */) {
  if (ch[0] == 0x2b) {
    if (0x52 <= ch[1] && ch[1] <= 0x7e) {
      return jisx0213_2000_1_property_table[ch[1] - 0x52];
    }
  }

  return 0;
}
