#include "./integrity_check_db.h"

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define LOG_FILE "./generated_values_history.txt"

#define ERR_FILE_CREATION_FAILED 87
#define ERR_THRD_CREATION_FAILED 88

static char active = 0;
static thrd_t thrd = -1;
static FILE *log_r = NULL;

enum TYPE_RACEUP{
    CHAR,
    U_CHAR,
    SHORT,
    FLOAT,
    DOUBLE,
    MULTY_DATA,
};

void generate_random_value(uint8_t type, void* o_buffer){
    union {
        char c;
        unsigned char uc;
        short s;
        float f;
        double d;
        struct MultyDataBuffer md;
    }bu;
    uint8_t size = 0;

    switch (type) {
        case CHAR:
            bu.c = (rand() % (int) pow(2,sizeof(char)));
            size = sizeof(char);
            break;
        case U_CHAR:
            bu.uc = (rand() % (int) pow(2,sizeof(unsigned char)));
            size = sizeof(char);
            break;
        case SHORT:
            bu.s = (rand() % (int) pow(2,sizeof(short)));
            size = sizeof(short);
            break;
        case FLOAT:
            bu.f = (rand() % (int) pow(2,sizeof(float)));
            size = sizeof(float);
            break;
        case DOUBLE:
            bu.d = (rand() % (int) pow(2,sizeof(double)));
            size = sizeof(double);
            break;
        case MULTY_DATA:
            size = sizeof(struct MultyDataBuffer);
            break;
    }

    memcpy(o_buffer, &bu, size);
}

//private

static int generate_values_imp(void *args){
    static void* data_b[] ={
        &gas,
        &brk,
        &battery_level,
        &steering_whell,
        &motor_pos,
        &brk_pressure,
        &sensors,
    };
    int rand_index = 0;

    srand(time(NULL));
    for (;;) {
        rand_index = rand() % sizeof(data_b)/sizeof(data_b[0]);
        switch (rand_index) {
            case 0:
                break;
        
        }
    }
}

//public

void generate_values(){
    if (active) return;

    remove(LOG_FILE);
    log_r = fopen(LOG_FILE, "w");
    if (!log_r) {
        goto failed_file;
    }
    
    thrd_create(&thrd, generate_values_imp, NULL);
    if (thrd < 0) {
        goto failed_thread;
    }
    active = 1;
    return;

failed_file:
    fprintf(stderr, "library error, contact raceup if error persist, check permissions: %d\n", 
            ERR_FILE_CREATION_FAILED);
    return;

failed_thread:
    fprintf(stderr, "library error, contact raceup if error persist: %d\n", 
            ERR_THRD_CREATION_FAILED);
    return;

}

int main(int argc, char *argv[])
{
    generate_values();
    while (1) {
        getchar();
        printf("gas: %c\n",gas);
        printf("brk: %f\n",brk);
        for (int i =0;i < sizeof(battery_level)/sizeof(battery_level[0]); i++) {
            printf("bat lev %d: %d\n",i,battery_level[i]);
        }
        printf("str weel: %lf\n",steering_whell);
        for (int i =0;i < sizeof(motor_pos)/sizeof(motor_pos[0]); i++) {
            printf("mot pos %d: %d\n",i,motor_pos[i]);
        }
        printf("brk press: %f\n",brk_pressure);
        for (int i =0;i < sizeof(sensors)/sizeof(sensors[0]); i++) {
        }
    }
    return EXIT_SUCCESS;
}
