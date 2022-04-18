#include <stdio.h>
int main(){
    float x = 2.5;
    float *ptr = &x;
    for (int i = 0; i<4; i++){
        printf("%02x\t" ,(unsigned int)ptr[i]);
    }
    // printf("%lx", (long unsigned int)ptr);
    return 0;
    
}
