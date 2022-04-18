#include<stdio.h>
#include<stdlib.h>

int main(){
    float* px = NULL;
    float tmp;
    scanf("%f", &tmp);
    int i =0;
    int count = 0;
    do{
        if(tmp != 0){
            if(px == NULL){
                px = (float*)calloc(1,sizeof(float));
                px[0] = tmp;
                count++;
            }
            else{
                px = (float*)realloc(px, sizeof(float));
            }
        }
    }
}