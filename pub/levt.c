#include "mheads.h"
#include "lheads.h"

NEOERR* levt_init(HASH **evth)
{
    mevent_t *evt;
    char *ename;
    HDF *node;
    HASH *levth;
    NEOERR *err;

    node = hdf_get_obj(g_cfg, "Mevent");
    if (!node) return nerr_raise(NERR_ASSERT, "Mevent config not found");
    
    err = hash_init(&levth, hash_str_hash, hash_str_comp);
    if (err != STATUS_OK) return nerr_pass(err);

    node = hdf_obj_child(node);
    while (node != NULL) {
        ename = hdf_obj_value(node);
        evt = mevent_init_plugin(ename);
        if (evt) {
            mtc_dbg("event %s init ok", ename);
            hash_insert(levth, (void*)strdup(ename), (void*)evt);
        } else {
            mtc_err("event %s init failure", ename);
        }
        
        node = hdf_obj_next(node);
    }

    *evth = levth;
    return STATUS_OK;
}

void levt_destroy(HASH *evth)
{
    char *key = NULL;
    
    mevent_t *evt = (mevent_t*)hash_next(evth, (void**)&key);

    while (evt != NULL) {
        mevent_free(evt);
        evt = hash_next(evth, (void**)&key);
    }

    hash_destroy(&evth);
}

