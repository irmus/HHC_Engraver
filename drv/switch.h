
#ifndef _SWITCH_H_
#define _SWITCH_H_


enum KEY_MAP {
    KEY_LEFT = 0,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_UP,
    KEY_F2,
    KEY_F1,
    KEY_F3,
    KEY_X1,
    KEY_X2,
    KEY_Y1,
    KEY_Y2,
    KEY_Z1,
    KEY_Z2,
    KEY_Z_UP,
    KEY_Z_DOWN,
    NO_KEY
};



void Init_SW(void);
uint8_t Read_SW(uint8_t ch);

#endif // _SWITCH_H_