#include "eta_csp_io.h"
int main(void)
{
    int i = 0;
    EtaCspIoPrintfViaDspUart(true); //enable prints.
    etaPrintf("started  DSP hello world app\n\r");
    for (i=0; i<10;i++)
        etaPrintf("Hello world DSP %d\n\r", i);
    etaPrintf("exiting  DSP\n\r");
    return 0;
}


