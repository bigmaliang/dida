#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mevent.h"

int main(int argc, char *argv[])
{
    int ret;

    mevent_t *evt = mevent_init_plugin("city");
    if (evt == NULL) {
        printf("init error\n");
        return 1;
    }

    hdf_set_value(evt->hdfsnd, "ip", argv[1]);
    
    ret = mevent_trigger(evt, NULL, 1001, FLAGS_SYNC);
    if (PROCESS_OK(ret))
        hdf_dump(evt->hdfrcv, NULL);
    else {
        char *s;
        hdf_write_string(evt->hdfrcv, &s);
        printf("process failure %d\n %s", ret, s);
        free(s);
    }
    
    mevent_free(evt);
    return 0;
}
