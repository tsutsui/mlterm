/*
 *	$Id: $
 */

#include  "indian.h"
#include  "table/telugu.table"

struct tabl *
libind_get_table(
	u_int *  table_size
	)
{
	*table_size = sizeof( iscii_telugu_table) / sizeof( struct tabl) ;

	return  iscii_telugu_table ;
}
