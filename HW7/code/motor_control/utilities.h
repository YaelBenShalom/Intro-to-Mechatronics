#ifndef utilities__H__
#define utilities__H__

typedef enum {
  IDLE,
  PWM,
  ITEST,
  HOLD,
  TRACK
} modes;


volatile modes MODE;

modes get_mode(modes);

#endif
