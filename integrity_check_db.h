#ifndef __INTEGRITY_CHECK_DB__
#define __INTEGRITY_CHECK_DB__

typedef struct MultyDataBuffer{
    char spec;
    int size;
    float payload;
}MultyDataBuffer;

extern char gas;
extern float brk1;
extern unsigned char voltage_level[8];
extern double steering_wheel;
extern short motor_pos[3]; 
extern float brk_pressure;
extern struct MultyDataBuffer sensors[13];

void generate_values();

#endif // !__INTEGRITY_CHECK_DB__
