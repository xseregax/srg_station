#ifndef PID_H
#define PID_H

#define PID_STEP 10 //интервал измерений, 20msec


//пид и фаза паяльника
#define IRON_KP 10
#define IRON_KI 0.3 * PID_STEP
#define IRON_KD 0.1 / PID_STEP


#endif // PID_H
