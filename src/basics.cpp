
void init_controller() {
    init();
  
    u8g2.begin();
    u8g2.setBusClock(800000);

    CLKPR = (1 << CLKPCE);    // allow writing to CLKPR
    CLKPR = 0;                // set divisor = 1 (without division)
}
