#ifndef __LNUM_H__
#define __LNUM_H__

#include "mheads.h"

__BEGIN_DECLS

enum {
    CGI_REQ_HTML = 0,
    CGI_REQ_AJAX,
    CGI_REQ_IMAGE,
    CGI_REQ_TERMINAL,
    CGI_REQ_UNSUPPORT
};

__END_DECLS
#endif    /* __LNUM_H__ */
