/*
 *	$Id$
 */

#include  "x_sample_sb_view_lib.h"

#include  <stdio.h>


/* --- global functions --- */

Pixmap
x_get_icon_pixmap(
	x_sb_view_t *  view ,
	GC  gc ,
	char **  data ,
	unsigned int  width ,
	unsigned int  height
	)
{
	Pixmap  pix ;
	char  cur ;
	int  x ;
	int  y ;
	
	pix = XCreatePixmap( view->display , view->window , width , height ,
		DefaultDepth( view->display , view->screen)) ;

	cur = '\0' ;
	for( y = 0 ; y < height ; y ++)
	{
		for( x = 0 ; x < width ; x ++)
		{
			if( cur != data[y][x])
			{
				if( data[y][x] == ' ')
				{
					XSetForeground( view->display , gc ,
						WhitePixel( view->display , view->screen)) ;
				}
				else if( data[y][x] == '#')
				{
					XSetForeground( view->display , gc ,
						BlackPixel( view->display , view->screen)) ;
				}
				else
				{
					continue ;
				}

				cur = data[y][x] ;
			}

			XDrawPoint( view->display , pix , gc , x , y) ;
		}

		x = 0 ;
	}

	return  pix ;
}

int
x_draw_icon_pixmap_fg(
	x_sb_view_t *  view ,
	Pixmap  arrow ,
	char **  data ,
	unsigned int  width ,
	unsigned int  height
	)
{
	int  x ;
	int  y ;
	
	for( y = 0 ; y < height ; y ++)
	{
		for( x = 0 ; x < width ; x ++)
		{
			if( data[y][x] == '-')
			{
				XDrawPoint( view->display , arrow , view->gc , x , y) ;
			}
		}
	}

	return  1 ;
}
