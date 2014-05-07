#ifndef __MEVENT_PUBLIC_H__
#define __MEVENT_PUBLIC_H__

time_t  pub_plan_get_abssec(char *pdate, char *ptime);
time_t  pub_plan_get_relsec(HDF *node, time_t thatsec);
NEOERR* pub_plan_sort_by_time(HDF *node, float km, time_t thatsec, char *pdate);

#endif    /* __MEVENT_PUBLIC_H__ */
