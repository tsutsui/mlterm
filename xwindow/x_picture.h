/*
 *	$Id$
 */

#ifndef  __X_PICTURE_H__
#define  __X_PICTURE_H__


#include  <kiklib/kik_types.h>		/* u_int16_t */
#include  <ml_term.h>

#include  "x.h"				/* XA_PIXMAP */
#include  "x_window.h"


typedef struct x_picture_modifier
{
	u_int16_t  brightness ;		/* 0 - 65535 */
	u_int16_t  contrast ;		/* 0 - 65535 */
	u_int16_t  gamma ;		/* 0 - 65535 */

	u_int8_t  alpha ;		/* 0 - 255 */
	u_int8_t  blend_red ;
	u_int8_t  blend_green ;
	u_int8_t  blend_blue ;

} x_picture_modifier_t ;

typedef struct x_picture
{
	Display *  display ;
	x_picture_modifier_t *  mod ;
	char *  file_path ;
	u_int  width ;
	u_int  height ;

	Pixmap  pixmap ;

	u_int  ref_count ;

} x_picture_t ;

typedef struct x_icon_picture
{
	x_display_t *  disp ;
	char *  file_path ;

	Pixmap  pixmap ;
	PixmapMask  mask ;
	u_int32_t *  cardinal ;

	u_int  ref_count ;
	
} x_icon_picture_t ;

typedef struct x_inline_picture
{
	Pixmap  pixmap ;
	PixmapMask  mask ;
	char *  file_path ;
	u_int  width ;
	u_int  height ;
	x_display_t *  disp ;
	ml_term_t *  term ;
	u_int8_t  col_width ;
	u_int8_t  line_height ;

	u_int16_t  ref_count ;

} x_inline_picture_t ;


#define  INLINEPIC_ID_SHIFT   14
#define  INLINEPIC_ID(glyph)  ((glyph) >> 14 & 0x3ff)
#define  INLINEPIC_POS(glyph) ((glyph) & 0x3fff)


#ifdef  NO_IMAGE

#define  x_picture_display_opened(display)  (0)
#define  x_picture_display_closed(display)  (0)
#define  x_picture_modifiers_equal(a,b)  (0)
#define  x_acquire_bg_picture(win,mod,file_path)  (NULL)
#define  x_release_picture(pic)  (0)
#define  x_acquire_icon_picture(disp,file_path)  (NULL)
#define  x_release_icon_picture(pic)  (0)
#define  x_load_inline_picture(disp,file_path,width,height,col_width,line_height,term)  (-1)
#define  x_get_inline_picture(idx)  (NULL)

#else

int  x_picture_display_opened( Display *  display) ;

int  x_picture_display_closed( Display *  display) ;

int  x_picture_modifiers_equal( x_picture_modifier_t *  a , x_picture_modifier_t *  b) ;

x_picture_t *  x_acquire_bg_picture( x_window_t *  win , x_picture_modifier_t *  mod ,
			char *  file_path) ;

int  x_release_picture( x_picture_t *  pic) ;

x_icon_picture_t *  x_acquire_icon_picture( x_display_t *  disp , char *  file_path) ;

int  x_release_icon_picture( x_icon_picture_t *  pic) ;

int  x_load_inline_picture( x_display_t *  disp , char *  file_path ,
	u_int *  width , u_int *  height , u_int  col_width , u_int  line_height ,
	ml_term_t *  term) ;

x_inline_picture_t *  x_get_inline_picture( int  idx) ;

#endif

#define  x_picture_modifier_is_normal(pic_mod) (x_picture_modifiers_equal((pic_mod), NULL))


#endif
