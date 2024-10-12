#ifndef __INTEGRITY_CHECK_DB__
#define __INTEGRITY_CHECK_DB__

typedef struct MultyDataBuffer{
    char spec;
    int size;
    float payload;
}MultyDataBuffer;

char gas = 0;
float brk= 0;
unsigned char battery_level[8]= {};
double steering_whell = 0.0;
short motor_pos[3] = {}; 
float brk_pressure = 0.0;
struct MultyDataBuffer sensors[13];

void generate_values();

#endif // !__INTEGRITY_CHECK_DB__
