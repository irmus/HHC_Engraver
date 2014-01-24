

#ifndef __MOTOR_H__
#define __MOTOR_H__




enum
{
    MOTOR_X,
    MOTOR_Y,
    MOTOR_Z,
    MOTOR_CNT,
};





enum
{
    DIR_CW,
    DIR_CCW,
    DIR_MAX
};






void Motor_Init(void);
void Motor_SetDirection(int ch, int dir);
void Motor_ToggleClock(int ch);


#endif /* __MOTOR_H__ */

