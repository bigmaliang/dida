#ifndef __LERR_H__
#define __LERR_H__
#include "mheads.h"

__BEGIN_DECLS

extern NERR_TYPE LERR_NOTLOGIN;

extern NERR_TYPE LERR_LOGINPSW;
extern NERR_TYPE LERR_LIMIT;                
extern NERR_TYPE LERR_MISS_DATA;
extern NERR_TYPE LERR_MISS_TPL;            
extern NERR_TYPE LERR_ATTACK;            

extern NERR_TYPE LERR_MEMBERED;
extern NERR_TYPE LERR_CARED;
extern NERR_TYPE LERR_MEMBER_NEXIST;
extern NERR_TYPE LERR_PLAN_NEXIST;
extern NERR_TYPE LERR_PLAN_NMATCH;

extern NERR_TYPE LERR_USERINPUT;
extern NERR_TYPE LERR_IMGPROE;            

NEOERR* lerr_init();
void lerr_opfinish_json(NEOERR *err, HDF *hdf);

__END_DECLS
#endif    /* __LERR_H__ */
