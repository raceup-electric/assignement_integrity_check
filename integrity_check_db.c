#include "./integrity_check_db.h"

char gas = 0;
float brk1= 0;
unsigned char voltage_level[8]= {};
double steering_wheel = 0.0;
short motor_pos[3] = {}; 
float brk_pressure = 0.0;
struct MultyDataBuffer sensors[13];

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define DIV 1048576 
#define WIDTH 7
#define usleep(time) Sleep(time/150)
#endif


#ifdef linux
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#endif

#include <assert.h>
#include <limits.h> 
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
#ifdef linux
static thrd_t thrd = -1;
#endif
static FILE *log_r = NULL;

enum TYPE_RACEUP{
    RACEUP_CHAR,
    RACEUP_U_CHAR,
    RACEUP_SHORT,
    RACEUP_FLOAT,
    RACEUP_DOUBLE,
    RACEUP_MULTY_DATA,
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
    char result[1024] = {};
    int len =0;

    switch (type) {
        case RACEUP_U_CHAR:
        case RACEUP_CHAR:
            bu.c = rand() % (int) SCHAR_MAX;
            size = sizeof(char);
            len = snprintf(NULL, 0, "%hhx",bu.c);
            snprintf(result, len + 1, "%hhx", bu.c);
            break;
        case RACEUP_SHORT:
            bu.s = rand() % SHRT_MAX - SHRT_MAX/2;
            size = sizeof(short);
            len = snprintf(NULL, 0, "%hx",bu.s);
            snprintf(result, len + 1, "%hx", bu.s);
            break;
        case RACEUP_FLOAT:
            bu.f = ((float)rand()/(float)(RAND_MAX)) *150.0f;
            size = sizeof(float);
            len = snprintf(NULL, 0, "%fx",bu.f);
            snprintf(result, len + 1, "%fx", bu.f);
            break;
        case RACEUP_DOUBLE:
            bu.d = ((double)rand()/(double)(RAND_MAX)) * 150.0f;
            size = sizeof(double);
            len = snprintf(NULL, 0, "%lfx",bu.d);
            snprintf(result, len + 1, "%lfx", bu.d);
            break;
        case RACEUP_MULTY_DATA:
            size = sizeof(struct MultyDataBuffer);
            randon_multi_data(&bu.ss);

            len = snprintf(NULL, 0, "%hhx",bu.ss.spec);
            snprintf(result, len + 1, "%hhx", bu.ss.spec);
            fwrite(result, len, 1, log_r);
            memset(result, 0, len);

            len = snprintf(NULL, 0, "%fx",bu.ss.payload);
            snprintf(result, len + 1, "%fx", bu.ss.payload);
            fwrite(result, len, 1, log_r);
            memset(result, 0, len);

            len = snprintf(NULL, 0, "%x",bu.ss.size);
            snprintf(result, len + 1, "%x", bu.ss.size);
            fwrite(result, len, 1, log_r);
            memset(result, 0, len);

            break;
    }
    memcpy(o_buffer, &bu, size);
    if (len) {
        fwrite(result, len, 1, log_r);
    }
}

//private

static void print_var_in_file(void *var, enum TYPE_RACEUP type){
    char result[1024] = {};
    int len = snprintf(NULL, 0, "%LX",*(unsigned long long *) var);
    snprintf(result, len + 1, "%LX",*(unsigned long long*) var);
    fwrite(result, len, 1, log_r);
}

static int generate_values_imp(void *args){
    static void* data_b[] ={
        &gas,
        &brk1,
        &voltage_level,
        &steering_wheel,
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
                ty= RACEUP_CHAR;
                char* v = "gas:";
                fwrite(v, 1, strlen(v), log_r);
                memcpy(var_name, &v, sizeof(gas));
                break;
            case 1:
                ty=RACEUP_FLOAT;
                char* v1 = "brk1:";
                fwrite(v1, 1, strlen(v1), log_r);
                break;
            case 2:
                ty=RACEUP_U_CHAR;
                char* v2 = "voltage_level";
                fwrite(v2, 1, strlen(v2), log_r);
                RAND_VAR_ARR(voltage_level,var);
                break;
            case 3:
                ty = RACEUP_DOUBLE;
                char* v5 = "steering_wheel:";
                fwrite(v5, 1, strlen(v5), log_r);
                break;
            case 4:
                ty = RACEUP_SHORT;
                char* v3 = "motor_pos";
                fwrite(v3, 1, strlen(v3), log_r);
                RAND_VAR_ARR(motor_pos,var);
                break;
            case 5:
                ty = RACEUP_FLOAT;
                char* v4 = "brk_pos:";
                fwrite(v4, 1, strlen(v4), log_r);
                break;
            case 6:
                ty = RACEUP_MULTY_DATA;
                char* v6 = "sensors";
                fwrite(v6, 1, strlen(v6), log_r);
                RAND_VAR_ARR(sensors, var);
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
        generate_random_value(ty, var);
        print_var_in_file(var,ty);
        char *end = "\n";
        fwrite(end, 1, strlen(end), log_r);
        usleep(200);
    }
}

#ifdef _WIN32

DWORD WINAPI MyThreadFunction( LPVOID lpParam )
{
    generate_values_imp(NULL);
    return 0;
}

#endif

//public
void generate_values(){
    if (active) return;

    remove(LOG_FILE);
    log_r = fopen(LOG_FILE, "w");
    if (!log_r) {
        goto failed_file;
    }
#ifdef linux

    thrd_create(&thrd, generate_values_imp, NULL);
    if (thrd < 0) {
        goto failed_thread;
    }
    active = 1;
#endif

#ifdef _WIN32
    DWORD out;
    CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MyThreadFunction,       // thread function name
            NULL,                   // argument to thread function 
            0,                      // use default creation flags 
            &out);                  // returns the thread identifier

#endif

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

// int main(int argc, char *argv[])
// {
//     generate_values();
//     
//     while (1) {
//         getchar();
//         printf("gas: %d\n",gas);
//         printf("brk1: %f\n",brk1);
//         for (int i =0;i < sizeof(voltage_level)/sizeof(voltage_level[0]); i++) {
//             printf("bat lev %d: %d\n",i,voltage_level[i]);
//         }
//         printf("str weel: %lf\n",steering_wheel);
//         for (int i =0;i < sizeof(motor_pos)/sizeof(motor_pos[0]); i++) {
//             printf("mot pos %d: %d\n",i,motor_pos[i]);
//         }
//         printf("brk1 press: %f\n",brk_pressure);
//         for (int i =0;i < sizeof(sensors)/sizeof(sensors[0]); i++) {
//             printf("sensor[%d]: [spec: %c, size: %d, payload: %f]\n",
//                     i,sensors[i].spec,sensors[i].size,sensors[i].payload);
//         }
//     }
//     return EXIT_SUCCESS;
// }
