#include "hardware.h"
#include "Hellow.h"
#include "DbgUtil.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    uint8_t dorkin[49] = {"012345678901234567890123456789012345678901234567"};
    uint8_t dorkout[1024];
    
    /* code */
    printf("Get a job \n");
    HelloMain();
    //DumpByteStr(dorkin,dorkout,48);

    //printf((char*) dorkout);

    return 0;
}
