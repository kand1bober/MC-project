#include "basics.h"

void init_controller() {
    init();

    CLKPR = (1 << CLKPCE);    // allow writing to CLKPR
    CLKPR = 0;                // set divisor = 1 (without division)
}
