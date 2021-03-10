#include "eta_csp_io.h"

extern void test_sqrt(void);
extern void test_log(void);
extern void test_exp(void);

int main(void)
{
    int i = 0;

    set_rounding_mode(2);
    set_saturation_mode(1);

    EtaCspIoPrintfViaDspUart(true); //enable prints.
    etaPrintf("eta math library test in progress\n\r");

    //test sqrt
    test_sqrt();

    //test exp
    test_exp();

    //test fft
    //test_fft();

    //test log
    test_log();

    etaPrintf("exiting  DSP\n\r");
    return 0;
}


