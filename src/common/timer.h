#ifndef TIMER_H
#define TIMER_H

#define TIME_1MS (1)
#define TIME_1S (1000 * TIME_1MS)


#define TIMER_ISR_COMPARE (F_CPU / 64 / 1000L - 1)

#define TIMER_T uint32_t
#define TIMER_TU int32_t
#define TIMER_INIT(timer,timeout) (timer = timer_millis() + timeout)
#define TIMER_ENDED(timer) ((TIMER_TU)(timer_millis() - timer) >= 0)
#define TIMER_NEXT(timer,timeout) (timer += timeout)


TIMER_T timer_millis(void);

void timer_init_timer(void);




#endif // TIMER_H
