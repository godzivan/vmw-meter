#include <stdint.h>

#include "include/pi-pico.h"
#include "include/gpio.h"

typedef uint64_t absolute_time_t;

static void tight_loop_contents(void) {}

#define TIMER_BASE 0x40054000


#define NUM_TIMERS 4

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;

typedef struct {
    io_wo_32 timehw;
    io_wo_32 timelw;
    io_ro_32 timehr;
    io_ro_32 timelr;
    io_rw_32 alarm[NUM_TIMERS];
    io_rw_32 armed;
    io_ro_32 timerawh;
    io_ro_32 timerawl;
    io_rw_32 dbgpause;
    io_rw_32 pause;
    io_rw_32 intr;
    io_rw_32 inte;
    io_rw_32 intf;
    io_ro_32 ints;
} timer_hw_t;

#define timer_hw ((timer_hw_t *const)TIMER_BASE)


static uint64_t to_us_since_boot(absolute_time_t t) {
    return t;
}


static void update_us_since_boot(absolute_time_t *t, uint64_t us_since_boot) {
//#ifdef NDEBUG
    *t = us_since_boot;
//#else
//    assert(us_since_boot <= INT64_MAX);
//    t->_private_us_since_boot = us_since_boot;
//#endif
}


uint64_t time_us_64(void) {
    // Need to make sure that the upper 32 bits of the timer
    // don't change, so read that first
    uint32_t hi = timer_hw->timerawh;
    uint32_t lo;
    do {
        // Read the lower 32 bits
        lo = timer_hw->timerawl;
        // Now read the upper 32 bits again and
        // check that it hasn't incremented. If it has loop around
        // and read the lower 32 bits again to get an accurate value
        uint32_t next_hi = timer_hw->timerawh;
        if (hi == next_hi) break;
        hi = next_hi;
    } while (1);
    return ((uint64_t) hi << 32u) | lo;
}


void busy_wait_until(absolute_time_t t) {
    uint64_t target = to_us_since_boot(t);
    uint32_t hi_target = (uint32_t)(target >> 32u);
    uint32_t hi = timer_hw->timerawh;
    while (hi < hi_target) {
        hi = timer_hw->timerawh;
        tight_loop_contents();
    }
    while (hi == hi_target && timer_hw->timerawl < (uint32_t) target) {
        hi = timer_hw->timerawh;
        tight_loop_contents();
    }
}


void busy_wait_us(uint64_t delay_us) {
    uint64_t base = time_us_64();
    uint64_t target = base + delay_us;
    if (target < base) {
        target = (uint64_t)-1;
    }
    absolute_time_t t;
    update_us_since_boot(&t, target);
    busy_wait_until(t);
}


void busy_wait_us_32(uint32_t delay_us) {
    if (0 <= (int32_t)delay_us) {
        // we only allow 31 bits, otherwise we could have a race in the loop below with
        // values very close to 2^32
        uint32_t start = timer_hw->timerawl;
        while (timer_hw->timerawl - start < delay_us) {
            tight_loop_contents();
        }
    } else {
        busy_wait_us(delay_us);
    }
}

static absolute_time_t get_absolute_time(void) {
    absolute_time_t t;
    update_us_since_boot(&t, time_us_64());
    return t;
}


static inline absolute_time_t delayed_by_us(const absolute_time_t t, uint64_t us) {
    absolute_time_t t2;
    uint64_t base = to_us_since_boot(t);
    uint64_t delayed = base + us;
    if (delayed < base) {
        delayed = (uint64_t)-1;
    }
    update_us_since_boot(&t2, delayed);
    return t2;
}


static absolute_time_t make_timeout_time_us(uint64_t us) {
    return delayed_by_us(get_absolute_time(), us);
}


void sleep_us(uint64_t us) {
//#if !PICO_TIME_DEFAULT_ALARM_POOL_DISABLED
//    sleep_until(make_timeout_time_us(us));
//#else
    if (us >> 32u) {
        busy_wait_until(make_timeout_time_us(us));
    } else {
        busy_wait_us_32(us);
    }
//#endif

}

void sleep_ms(uint32_t ms) {
    sleep_us(ms * 1024ull);	// FIXME
}


int main(int argc, char **argv) {

	const uint32_t LED_PIN = PICO_DEFAULT_LED_PIN;

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	while (1) {
		gpio_put(LED_PIN, 1);
		sleep_ms(250);
		gpio_put(LED_PIN, 0);
		sleep_ms(250);
	}

	return 0;

}
