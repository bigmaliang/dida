#include "mheads.h"
#include "lheads.h"

/*
 * TODO how make local dlsym ok? so tired 
 */
static void ltpl_donotcall()
{
    zero_data_get(NULL, NULL, NULL, NULL);
    spd_pre_data_get(NULL, NULL, NULL, NULL);
    pos_data_get(NULL, NULL, NULL, NULL);
    plan_match_data_get(NULL, NULL, NULL, NULL);
    member_info_data_get(NULL, NULL, NULL, NULL);
    bore_data_get(NULL, NULL, NULL, NULL);
    comment_data_get(NULL, NULL, NULL, NULL);
    trace_data_add(NULL, NULL, NULL, NULL);
    system_view_data_get(NULL, NULL, NULL, NULL);
}

#ifdef __MACH__
int ltpl_config(struct dirent *ent)
#else
int ltpl_config(const struct dirent *ent)
#endif
{
    if (reg_search(".*.hdf", ent->d_name))
        return 1;
    else
        return 0;
}

void ltpl_prepare_rend(HDF *hdf, char *tpl)
{
    char key[LEN_ST];
    HDF *tmphdf, *ahdf; 
    if (hdf == NULL) return;

    /*
     * merge dataset from g_cfg 
     */
    snprintf(key, sizeof(key), PRE_CFG_DATASET".%s", tpl);
    tmphdf = hdf_get_obj(g_cfg, key);
    if (tmphdf != NULL) hdf_copy(hdf, PRE_CFG_LAYOUT, tmphdf);
    
    /*
     * special actions
     */
    tmphdf = hdf_get_child(hdf, PRE_WALK_SACTION);
    while (tmphdf) {
        snprintf(key, sizeof(key), PRE_LAYOUT".%s", hdf_obj_value(tmphdf));
        ahdf = hdf_get_obj(hdf, key);
        if (ahdf) hdf_copy(hdf, PRE_LAYOUT".actions", ahdf);
        
        tmphdf = hdf_obj_next(tmphdf);
    }
    
    /*
     * set classes
     */
    char *pos = hdf_get_value(hdf, "Layout.tabpart", NULL);
    if (pos) hdf_set_valuef(hdf, "Layout.tabs.%s.class=selected", pos);
    pos = hdf_get_value(hdf, "Layout.actionpart", NULL);
    if (pos) hdf_set_valuef(hdf, "Layout.actions.%s.class=selected", pos);
}

NEOERR* ltpl_parse_file(HASH *dbh, HASH *evth,
                        void *lib, char *dir, char *name, HASH *outhash)
{
    char *tp = NULL, *tpl = NULL, *val = NULL;
    HDF *node = NULL, *dhdf = NULL, *child = NULL, *thdf = NULL;
    CSPARSE *cs = NULL;
    STRING str;
    char fname[_POSIX_PATH_MAX], tok[64], *outfile;
    NEOERR* (*data_handler)(HDF *hdf, HASH *dbh, HASH *evth);
    NEOERR *err;
    
    memset(fname, 0x0, sizeof(fname));
    snprintf(fname, sizeof(fname), "%s/%s", dir, name);
    err = hdf_init(&node);
    if (err != STATUS_OK) return nerr_pass(err);
 
    err = hdf_read_file(node, fname);
    if (err != STATUS_OK) return nerr_pass(err);

    child = hdf_obj_child(node);
    while (child != NULL) {
        mtc_dbg("parse node %s", hdf_obj_name(child));
        string_init(&str);

        val = mcs_obj_attr(child, "merge");
        if (val) {
            ULIST *list;
            string_array_split(&list, val, ",", 10);
            ITERATE_MLIST(list) {
                snprintf(fname, sizeof(fname), "%s/%s",
                         dir, neos_strip((char*)list->items[t_rsv_i]));
                err = hdf_init(&dhdf);
                JUMP_NOK(err, wnext);
                err = hdf_read_file(dhdf, fname);
                JUMP_NOK(err, wnext);
                err = hdf_copy(child, NULL, dhdf);
                JUMP_NOK(err, wnext);
            }
            uListDestroy(&list, ULIST_FREE);
        }

        /*
         * can't use dataset directly, because we'll destroy the whole node
         */
        err = hdf_init(&dhdf);
        JUMP_NOK(err, wnext);
        err = hdf_get_node(child, PRE_CFG_DATASET, &thdf);
        JUMP_NOK(err, wnext);
        err = hdf_copy(dhdf, NULL, thdf);
        JUMP_NOK(err, wnext);
        
        err = cs_init(&cs, dhdf);
        JUMP_NOK(err, wnext);

        hdf_set_value(cs->hdf, "hdf.loadpaths.tpl", PATH_TPL);
        hdf_set_value(cs->hdf, "hdf.loadpaths.local", dir);

        err = cgi_register_strfuncs(cs);
        JUMP_NOK(err, wnext);
        err = mcs_register_bitop_functions(cs);
        JUMP_NOK(err, wnext);
        err = mcs_register_mkd_functions(cs);
        JUMP_NOK(err, wnext);
        err = mcs_register_string_uslice(cs);
        JUMP_NOK(err, wnext);

        tpl = hdf_get_value(child, PRE_CFG_LAYOUT, "null.html");
        snprintf(fname, sizeof(fname), "%s/%s", PATH_TPL, tpl);
        err = cs_parse_file(cs, fname);
        JUMP_NOK(err, wnext);

        if (outhash != NULL) {
            /*
             * store template for rend stage use
             */
            hdf_set_value(cs->hdf, PRE_RESERVE"."PRE_CFG_LAYOUT, tpl);
            
            /*
             * strdup the key, baby, because we'll free the hdf later
             */
            err = hash_insert(outhash, (void*)strdup(hdf_obj_name(child)), (void*)cs);
            JUMP_NOK(err, wnext);

            snprintf(tok, sizeof(tok), "%s_hdf", hdf_obj_name(child));
            err = hash_insert(outhash, (void*)strdup(tok), (void*)cs->hdf);
            JUMP_NOK(err, wnext);
        }

        if ((outfile = hdf_get_value(child, PRE_CFG_OUTPUT, NULL)) != NULL) {
            ltpl_prepare_rend(cs->hdf, tpl);
                
            /*
             * get_data
             */
            val = hdf_get_value(child, PRE_CFG_DATAER, NULL);
            if (val != NULL && lib) {
                data_handler = dlsym(lib, val);
                if( (tp = dlerror()) != NULL) {
                    mtc_err("%s", tp);
                    //continue;
                } else {
                    err = (*data_handler)(cs->hdf, dbh, evth);
                    TRACE_NOK(err);
                }
            }

            err = cs_render(cs, &str, mcs_strcb);
            JUMP_NOK(err, wnext);

            /*
             * produce output filename
             */
            val = mcs_hdf_attr(child, PRE_CFG_OUTPUT, "ftime");
            if (val) {
                char tm[LEN_TM];
                mutil_getdatetime(tm, sizeof(tm), val, 0);
                outfile = mstr_repstr(1, outfile, "$ftime$", tm);
                /* TODO memory leak */
            }
            snprintf(fname, sizeof(fname), PATH_DOC"%s", outfile);

            /*
             * output file
             */
            err = mfile_makesure_dir(fname);
            JUMP_NOK(err, wnext);

            err = mcs_str2file(str, fname);
            JUMP_NOK(err, wnext);
#ifdef DEBUG_HDF
            snprintf(fname, sizeof(fname), "%s/hdf.%s",
                     TC_ROOT, hdf_obj_name(child));
            hdf_write_file(child, fname);
#endif
        }

    wnext:
        if (cs != NULL && outhash == NULL)
            cs_destroy(&cs);
        string_clear(&str);
        child = hdf_obj_next(child);
    }
        
    if (node != NULL) hdf_destroy(&node);

    return STATUS_OK;
}

NEOERR* ltpl_parse_dir(char *dir, HASH *outhash)
{
    struct dirent **eps = NULL;
    int n;
    NEOERR *err;

    if (!dir) return nerr_raise(NERR_ASSERT, "can't read null directory");

    HASH *dbh, *evth;
    void *lib = dlopen(NULL, RTLD_NOW|RTLD_GLOBAL);
    if (!lib) return nerr_raise(NERR_SYSTEM, "dlopen %s", dlerror());
    
    err = ldb_init(&dbh);
    if (err != STATUS_OK) return nerr_pass(err);
    
    err = levt_init(&evth);
    if (err != STATUS_OK) return nerr_pass(err);
    
    n = scandir(dir, &eps, ltpl_config, alphasort);
    for (int i = 0; i < n; i++) {
        mtc_dbg("parse file %s", eps[i]->d_name);
        err = ltpl_parse_file(dbh, evth, lib, dir, eps[i]->d_name, outhash);
        TRACE_NOK(err);
        free(eps[i]);
    }

    ldb_destroy(dbh);
    levt_destroy(evth);
    dlclose(lib);
    
    if (n > 0) free(eps);
    else mtc_warn("no .hdf file found in %s", dir);

    return STATUS_OK;
}

NEOERR* ltpl_init(HASH **tplh, char *path)
{
    HASH *ltplh = NULL;
    NEOERR *err;

    *tplh = NULL;
    
    path = path ? path: PATH_TPL"config/run/";
	
    err = hash_init(&ltplh, hash_str_hash, hash_str_comp);
    if (err != STATUS_OK) return nerr_pass(err);

    err = ltpl_parse_dir(path, ltplh);
    if (err != STATUS_OK) return nerr_pass_ctx(err, "pase dir %s", path);
    
    *tplh = ltplh;
    return STATUS_OK;
}

void ltpl_destroy(HASH *tplh)
{
    char *key, *buf;

    if (!tplh) return;
    
    buf = (char*)hash_next(tplh, (void**)&key);

    while (buf != NULL) {
        free(buf);
        buf = hash_next(tplh, (void**)&key);
    }

    hash_destroy(&tplh);
}

NEOERR* ltpl_render(CGI *cgi, HASH *tplh, session_t *ses)
{
    STRING str; string_init(&str);
    CSPARSE *cs;
    HDF *dhdf;
    NEOERR *err;

    char *render = NULL;

    render = ses->render;
    cs = (CSPARSE*)hash_lookup(tplh, render);
    dhdf = (HDF*)hash_lookupf(tplh, "%s_hdf", render);

    if (!cs) return nerr_raise(LERR_MISS_TPL, "render %s not found", render);
    if (dhdf) hdf_copy(cgi->hdf, NULL, dhdf);
    
    ltpl_prepare_rend(cgi->hdf, hdf_get_value(cgi->hdf,
                                              PRE_RESERVE"."PRE_CFG_LAYOUT,
                                              "layout.html"));
    
    if (ses->tm_cache_browser > 0) {
        hdf_set_valuef(cgi->hdf, "cgiout.other.cache=Cache-Control: max-age=%lu",
                       ses->tm_cache_browser);
    }
    cs->hdf = cgi->hdf;

    err = cs_render(cs, &str, mcs_strcb);
    if (err != STATUS_OK) return nerr_pass(err);

    err = cgi_output(cgi, &str);
    if (err != STATUS_OK) return nerr_pass(err);

    cs->hdf = NULL;
    string_clear(&str);

    return STATUS_OK;
}

/*
 * we don't pass tplh parameter to it,
 * because walker don't have it. so, tplh stored in the g_datah
 */
NEOERR* ltpl_render2file(CGI *cgi, char *render, char *fname)
{
    STRING str; string_init(&str);
    HASH *tplh;
    CSPARSE *cs;
    HDF *dhdf;
    NEOERR *err;

    MCS_NOT_NULLC(cgi->hdf, render, fname);
    
    tplh = hash_lookup(g_datah, "runtime_templates");
    MCS_NOT_NULLA(tplh);

    cs = hash_lookup(tplh, render);
    MCS_NOT_NULLA(cs);

    dhdf = hash_lookupf(tplh, "%s_hdf", render);
    if (dhdf) hdf_copy(cgi->hdf, NULL, dhdf);

    ltpl_prepare_rend(cgi->hdf, hdf_get_value(cgi->hdf,
                                              PRE_RESERVE"."PRE_CFG_LAYOUT,
                                              "layout.html"));
    cs->hdf = cgi->hdf;

    err = cs_render(cs, &str, mcs_strcb);
    if (err != STATUS_OK) return nerr_pass(err);

    err = mfile_makesure_dir(fname);
    if (err != STATUS_OK) return nerr_pass(err);
    
    err = mcs_str2file(str, fname);
    if (err != STATUS_OK) return nerr_pass(err);

    string_clear(&str);

    return STATUS_OK;
}

NEOERR* ltpl_render2filef(CGI *cgi, char *render, char *fmt, ...)
{
    char tok[_POSIX_PATH_MAX];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(tok, sizeof(tok), fmt, ap);
    va_end(ap);

    return ltpl_render2file(cgi, render, tok);
}
