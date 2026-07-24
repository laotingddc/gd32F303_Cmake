#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_DOWN,
    BUTTON_EVENT_UP,
    BUTTON_EVENT_SINGLE_CLICK,
    BUTTON_EVENT_DOUBLE_CLICK,
    BUTTON_EVENT_LONG_PRESS,
} button_event_t;

typedef uint8_t (*button_read_level_fn)(void *user_arg);
typedef void (*button_event_cb)(button_event_t event, void *user_arg);

typedef struct {
    button_read_level_fn read_level;
    void *io_user_arg;

    button_event_cb event_cb;
    void *cb_user_arg;

    uint8_t active_level;
    uint16_t debounce_ms;
    uint16_t long_press_ms;
    uint16_t double_click_ms;
} button_cfg_t;

typedef struct {
    button_cfg_t cfg;

    bool stable_pressed;
    bool last_raw_pressed;

    uint16_t debounce_elapsed_ms;
    uint16_t press_elapsed_ms;
    uint16_t release_elapsed_ms;

    uint8_t click_count;
    bool long_reported;
} button_t;

int button_init(button_t *btn, const button_cfg_t *cfg);
void button_reset(button_t *btn);
void button_process(button_t *btn, uint16_t elapsed_ms);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */
