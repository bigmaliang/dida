#ifndef __LEVT_H__
#define __LEVT_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* levt_init(HASH **evth);
void levt_destroy(HASH *evth);

__END_DECLS
#endif    /* __LEVT_H__ */
