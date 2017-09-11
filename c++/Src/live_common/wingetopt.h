#pragma once

#ifdef __GNUC__
#include <getopt.h>
#else
#ifdef __cplusplus
extern "C" {
#endif
    extern int opterr;
    extern int optind;
    extern int optopt;
    extern char *optarg;
    extern int getopt(int argc, char **argv, char *opts);
#ifdef __cplusplus
}
#endif
#endif 