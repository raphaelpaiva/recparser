#ifndef _SSA_H

#define _SSA_H

#include "../cfg/cfg.h"

void dom_tree(CFG* cfg);
void dom_frontier(CFG *cfg);
void find_globals(CFG *cfg);
void full_ssa(CFG *cfg);

#endif
