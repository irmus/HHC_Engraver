
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


#define STEP_RESOLUTION_TICK          1           // ���� ���� ���� Ŭ�� �ػ�
#define SCAN_INTERVAL_TICK            50          // ���̽�ƽ ��ĵ �ð� ����






static volatile int flag_step;          // step motor ���� Ŭ�� ����
static volatile int flag_scan;          // ���̽�ƽ ��ĵ
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


    // ���������� ��� Ű�� ��ĵ�ؼ� ���¸� �о�´�.
    for (i = KEY_LEFT; i < NO_KEY; i++)
    {
        current = Read_SW(i);

        // debouncing�� ���� �ٷ� ���� ���� ���ؼ� ������ ��츸 �����Ѵ�.
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




    // �о�� ���¸� ���������� �ؼ��ؼ� ���� ���� ����� �����.

    // X�� �ؼ�
    x_cmd = CMD_STOP;
    if (joystick_status[KEY_LEFT] == KEYSTAT_DOWN || joystick_status[KEY_RIGHT] == KEYSTAT_DOWN)
    {
        // �ӵ� ����
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

        // ���� ����
        if (joystick_status[KEY_LEFT] == KEYSTAT_DOWN)
            x_dir = DIR_CW;
        else
            x_dir = DIR_CCW;
    }


    // Y�� �ؼ�
    y_cmd = CMD_STOP;
    if (joystick_status[KEY_UP] == KEYSTAT_DOWN || joystick_status[KEY_DOWN] == KEYSTAT_DOWN)
    {
        // �ӵ� ����
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

        // ���� ����
        if (joystick_status[KEY_UP] == KEYSTAT_DOWN)
            y_dir = DIR_CCW;
        else
            y_dir = DIR_CW;
    }


    // Z�� �ؼ�
    z_cmd = CMD_STOP;
    if (joystick_status[KEY_Z_UP] == KEYSTAT_DOWN || joystick_status[KEY_Z_DOWN] == KEYSTAT_DOWN)
    {
        // �ӵ� ����
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

        // ���� ����
        if (joystick_status[KEY_Z_UP] == KEYSTAT_DOWN)
            z_dir = DIR_CCW;
        else
            z_dir = DIR_CW;
    }



}



// ���� ������ cmd�� ���� ���� ���� �޽��� �����Ѵ�.
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






