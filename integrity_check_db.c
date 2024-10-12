#include "./integrity_check_db.h"

#include <assert.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define LOG_FILE "./generated_values_history.txt"

#define ERR_FILE_CREATION_FAILED 87
#define ERR_THRD_CREATION_FAILED 88

#define RAND_FROM_TYPE(TYPE) (rand() %(long) pow(2,sizeof(TYPE)*8))
#define RAND_VAR_ARR(VAR,PTR) \
{\
    char* c = "[";\
    char* c1 = "]:";\
    fwrite(c, 1, 1, log_r);\
    int macro_rand_index = rand() % ((sizeof(VAR)/sizeof(VAR[0])));\
    char ci[1024] = {};\
    int length = snprintf( NULL, 0, "%d", macro_rand_index);\
    snprintf( ci, length + 1, "%d", macro_rand_index );\
    fwrite(&ci, 1, strlen(ci), log_r);\
    fwrite(c1, 1, 2, log_r);\
    PTR = &VAR[macro_rand_index];\
}

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

static void randon_multi_data(struct MultyDataBuffer* buff){
    buff->size = rand() % INT_MAX;
    buff->payload =((float)rand()/(float)(RAND_MAX)) * 76.0f;
    buff->spec = rand() % SCHAR_MAX;
    int idx[2] = {};
    idx[0] = rand() % 10;
    idx[1] = rand() % 10;
}

void generate_random_value(uint8_t type, void* o_buffer){
    union {
        char c;
        unsigned char uc;
        short s;
        float f;
        double d;
        struct MultyDataBuffer ss;
    }bu;
    memset(&bu, 0, sizeof(bu));
    uint8_t size = 0;

    switch (type) {
        case U_CHAR:
        case CHAR:
            bu.c = rand() % (int) SCHAR_MAX;
            size = sizeof(char);
            break;
        case SHORT:
            bu.s = rand() % SHRT_MAX;
            size = sizeof(short);
            break;
        case FLOAT:
            bu.f = ((float)rand()/(float)(RAND_MAX)) * 4000.0f;
            size = sizeof(float);
            break;
        case DOUBLE:
            bu.d = ((double)rand()/(double)(RAND_MAX)) * 32.0f;
            size = sizeof(double);
            break;
        case MULTY_DATA:
            size = sizeof(struct MultyDataBuffer);
            randon_multi_data(&bu.ss);
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
        void* var = data_b[rand_index];
        char var_name[64] = {};
        enum TYPE_RACEUP ty = 99;
        switch (rand_index) {
            case 0:
                ty= CHAR;
                char* v = "gas:";
                fwrite(v, 1, strlen(v), log_r);
                memcpy(var_name, &v, sizeof(gas));
                generate_random_value(ty, var);
                fwrite(var, 1, 1, log_r);
                break;
            case 1:
                ty=FLOAT;
                char* v1 = "brk:";
                fwrite(v1, 1, strlen(v1), log_r);
                generate_random_value(ty, var);
                int len = snprintf(NULL, 0, "%f", var);
                char result[1024] = {};
                snprintf(result, len + 1, "%f", var);
                fwrite(result, len, 1, log_r);
                break;
            case 2:
                ty=U_CHAR;
                char* v2 = "battery_level";
                fwrite(v2, 1, strlen(v2), log_r);
                RAND_VAR_ARR(battery_level,var);
                generate_random_value(ty, var);
                fwrite(var, 1, 1, log_r);
                break;
            case 3:
                ty = DOUBLE;
                char* v5 = "steering_wheel:";
                fwrite(v5, 1, strlen(v5), log_r);
                generate_random_value(ty, var);
                int len1 = snprintf(NULL, 0, "%f", var);
                char result1[1024] = {};
                snprintf(result1, len1 + 1, "%f", var);
                fwrite(result1, len, 1, log_r);
                break;
            case 4:
                ty = SHORT;
                char* v3 = "motor_pos";
                fwrite(v3, 1, strlen(v3), log_r);
                RAND_VAR_ARR(motor_pos,var);
                generate_random_value(ty, var);
                fwrite(var, sizeof(short), 1, log_r);
                break;
            case 5:
                ty = FLOAT;
                char* v4 = "brk_pos:";
                fwrite(v4, 1, strlen(v4), log_r);
                generate_random_value(ty, var);
                int len2 = snprintf(NULL, 0, "%f", var);
                char result2[1024] = {};
                snprintf(result2, len2 + 1, "%f", var);
                fwrite(result, len2, 1, log_r);
                break;
            case 6:
                ty = MULTY_DATA;
                char* v6 = "sensors";
                fwrite(v6, 1, strlen(v6), log_r);
                RAND_VAR_ARR(sensors, var);
                generate_random_value(ty, var);
                break;
            default:
                fprintf(stderr, "invalid var index: %d\n",rand_index);
                perror("invalid var index");
                assert(1==0);
                break;
        }
        if (ty == 99) {
            fprintf(stderr, "invalid var index: %d\n",rand_index);
            assert(1==0);
        }
        char *end = "\n";
        fwrite(end, 1, strlen(end), log_r);
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
        printf("gas: %d\n",gas);
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
            printf("sensor[%d]: [spec: %c, size: %d, payload: %f]\n",
                    i,sensors[i].spec,sensors[i].size,sensors[i].payload);
        }
    }
    return EXIT_SUCCESS;
}
