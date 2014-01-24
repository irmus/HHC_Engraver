
/*
 * HHC
 * HHC_Engraver firmware
 * https://github.com/irmus/HHC_Engraver
 */

#include "stm32f4xx.h"

#include "util.h"
//#include "led.h"
#include "switch.h"
#include "motor.h"

#define COUNTER_PER_SEC             10000        // systick timer frequency. 10kHz -> 0.1ms interval tick


#define STEP_RESOLUTION_TICK          1           // 스텝 모터 구동 클럭 해상도
#define SCAN_INTERVAL_TICK            50          // 조이스틱 스캔 시간 간격






static volatile int flag_step;          // step motor 제어 클럭 생성
static volatile int flag_scan;          // 조이스틱 스캔
static volatile unsigned long g_tick;






enum
{
    CMD_STOP,
    CMD_SLOW,
    CMD_MIDDLE,
    CMD_FAST,
    CMD_MAX
};


static int x_cmd, y_cmd, z_cmd;
static int x_cmd_prev, y_cmd_prev, z_cmd_prev;
static int step_interval_ms[CMD_MAX] = {9999, 30, 12, 6};
static int x_cnt, y_cnt, z_cnt;


static int x_dir, y_dir, z_dir;



enum
{
    KEYSTAT_UP,
    KEYSTAT_DOWN,
};


static unsigned char joystick_previous[NO_KEY];
static unsigned char joystick_status[NO_KEY];




static void ScanJoystick(void);
static void DriveMotor(void);




int main()
{
//    Init_LED();

    Init_SW();

    Motor_Init();


    if (SysTick_Config(SystemCoreClock / COUNTER_PER_SEC))
    {
        /* Capture error */
        while (1);
    }

    while(1)
    {

        __WFI();


        if (flag_step)
        {
            flag_step = 0;

            DriveMotor();
        }

        if (flag_scan)
        {
            flag_scan = 0;

            ScanJoystick();
        }
    }

}


void SysTick_Handler(void)
{
    unsigned long tick = g_tick++;

    if (tick % STEP_RESOLUTION_TICK == 0)
    {
        flag_step = 1;
    }


    if (tick % SCAN_INTERVAL_TICK == 0)
    {
        flag_scan = 1;
    }
}




static void ScanJoystick(void)
{
    int i;
    unsigned char current;


    // 순차적으로 모든 키를 스캔해서 상태를 읽어온다.
    for (i = KEY_LEFT; i < NO_KEY; i++)
    {
        current = Read_SW(i);

        // debouncing을 위해 바로 직전 값과 비교해서 동일한 경우만 인정한다.
        if (current == joystick_previous[i])
        {
            if (current)
                joystick_status[i] = KEYSTAT_UP;
            else
                joystick_status[i] = KEYSTAT_DOWN;
        }
        else
        {
            joystick_status[i] = KEYSTAT_UP;
        }

        joystick_previous[i] = current;
    }




    // 읽어온 상태를 종합적으로 해석해서 모터 제어 명령을 만든다.

    // X축 해석
    x_cmd = CMD_STOP;
    if (joystick_status[KEY_LEFT] == KEYSTAT_DOWN || joystick_status[KEY_RIGHT] == KEYSTAT_DOWN)
    {
        // 속도 설정
        if (joystick_status[KEY_X1] == KEYSTAT_DOWN &&
            joystick_status[KEY_X2] == KEYSTAT_DOWN)
        {
            x_cmd = CMD_FAST;
        }
        else if (joystick_status[KEY_X1] == KEYSTAT_DOWN)
        {
            x_cmd = CMD_SLOW;
        }
        else if (joystick_status[KEY_X2] == KEYSTAT_DOWN)
        {
            x_cmd = CMD_MIDDLE;
        }

        // 방향 설정
        if (joystick_status[KEY_LEFT] == KEYSTAT_DOWN)
            x_dir = DIR_CW;
        else
            x_dir = DIR_CCW;
    }


    // Y축 해석
    y_cmd = CMD_STOP;
    if (joystick_status[KEY_UP] == KEYSTAT_DOWN || joystick_status[KEY_DOWN] == KEYSTAT_DOWN)
    {
        // 속도 설정
        if (joystick_status[KEY_Y1] == KEYSTAT_DOWN &&
            joystick_status[KEY_Y2] == KEYSTAT_DOWN)
        {
            y_cmd = CMD_FAST;
        }
        else if (joystick_status[KEY_Y1] == KEYSTAT_DOWN)
        {
            y_cmd = CMD_SLOW;
        }
        else if (joystick_status[KEY_Y2] == KEYSTAT_DOWN)
        {
            y_cmd = CMD_MIDDLE;
        }

        // 방향 설정
        if (joystick_status[KEY_UP] == KEYSTAT_DOWN)
            y_dir = DIR_CCW;
        else
            y_dir = DIR_CW;
    }


    // Z축 해석
    z_cmd = CMD_STOP;
    if (joystick_status[KEY_Z_UP] == KEYSTAT_DOWN || joystick_status[KEY_Z_DOWN] == KEYSTAT_DOWN)
    {
        // 속도 설정
        if (joystick_status[KEY_Z1] == KEYSTAT_DOWN &&
            joystick_status[KEY_Z2] == KEYSTAT_DOWN)
        {
            z_cmd = CMD_FAST;
        }
        else if (joystick_status[KEY_Z1] == KEYSTAT_DOWN)
        {
            z_cmd = CMD_SLOW;
        }
        else if (joystick_status[KEY_Z2] == KEYSTAT_DOWN)
        {
            z_cmd = CMD_MIDDLE;
        }

        // 방향 설정
        if (joystick_status[KEY_Z_UP] == KEYSTAT_DOWN)
            z_dir = DIR_CCW;
        else
            z_dir = DIR_CW;
    }



}



// 현재 설정된 cmd에 따라 모터 구동 펄스를 생성한다.
static void DriveMotor(void)
{
    if (x_cmd != x_cmd_prev)
        x_cnt = step_interval_ms[x_cmd];        // reload counter value
    x_cmd_prev = x_cmd;

    if (x_cmd == CMD_STOP)
    {
        x_cnt = step_interval_ms[CMD_STOP];
    }
    else
    {
        if (--x_cnt <= 0)       // timeout
        {
            x_cnt = step_interval_ms[x_cmd];        // reload counter value

            Motor_SetDirection(MOTOR_X, x_dir);

            Motor_ToggleClock(MOTOR_X);
        }
    }




    if (y_cmd != y_cmd_prev)
        y_cnt = step_interval_ms[y_cmd];        // reload counter value
    y_cmd_prev = y_cmd;

    if (y_cmd == CMD_STOP)
    {
        y_cnt = step_interval_ms[CMD_STOP];
    }
    else
    {
        if (--y_cnt <= 0)       // timeout
        {
            y_cnt = step_interval_ms[y_cmd];        // reload counter value

            Motor_SetDirection(MOTOR_Y, y_dir);

            Motor_ToggleClock(MOTOR_Y);
        }
    }




    if (z_cmd != z_cmd_prev)
        z_cnt = step_interval_ms[z_cmd];        // reload counter value
    z_cmd_prev = z_cmd;

    if (z_cmd == CMD_STOP)
    {
        z_cnt = step_interval_ms[CMD_STOP];
    }
    else
    {
        if (--z_cnt <= 0)       // timeout
        {
            z_cnt = step_interval_ms[z_cmd];        // reload counter value

            Motor_SetDirection(MOTOR_Z, z_dir);

            Motor_ToggleClock(MOTOR_Z);
        }
    }

}






