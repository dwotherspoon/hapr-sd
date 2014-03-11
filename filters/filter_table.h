#ifndef FT_H
#define FT_H
#include "reverb_filter.h"
#include "tremolo_filter.h"
#include "test_filter.h"
#include "ngate_filter.h"
#include "bp_filter.h"
#include "flanging_filter.h"
#include "echo_filter.h"
#include "parallel_filter.h"
#include "gain_filter.h"
#include "lp_filter.h"
#include "hp_filter.h"
#endif

#define FTABLE_LEN 11


extern char * ftable_names[];

extern char * ftable_descs[];

extern char * ftable_params[];

extern applyFilter ftable_applys[];

extern createFilter ftable_creators[];
