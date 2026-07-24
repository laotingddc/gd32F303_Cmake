#include "input/button.h"

static void button_emit(button_t *btn, button_event_t event)
{
    if ((btn != 0) && (btn->cfg.event_cb != 0)) {
        btn->cfg.event_cb(event, btn->cfg.cb_user_arg);
    }
}

static uint16_t sat_add_u16(uint16_t v, uint16_t inc)
{
    uint32_t sum = (uint32_t)v + (uint32_t)inc;
    if (sum > 0xFFFFU) {
        return 0xFFFFU;
    }
    return (uint16_t)sum;
}

int button_init(button_t *btn, const button_cfg_t *cfg)
{
    if ((btn == 0) || (cfg == 0) || (cfg->read_level == 0)) {
        return -1;
    }

    btn->cfg = *cfg;

    if (btn->cfg.debounce_ms == 0U) {
        btn->cfg.debounce_ms = 20U;
    }
    if (btn->cfg.long_press_ms == 0U) {
        btn->cfg.long_press_ms = 800U;
    }
    if (btn->cfg.double_click_ms == 0U) {
        btn->cfg.double_click_ms = 250U;
    }

    button_reset(btn);
    return 0;
}

void button_reset(button_t *btn)
{
    uint8_t level;

    if ((btn == 0) || (btn->cfg.read_level == 0)) {
        return;
    }

    level = btn->cfg.read_level(btn->cfg.io_user_arg);
    btn->stable_pressed = (level == btn->cfg.active_level);
    btn->last_raw_pressed = btn->stable_pressed;
    btn->debounce_elapsed_ms = 0U;
    btn->press_elapsed_ms = 0U;
    btn->release_elapsed_ms = 0U;
    btn->click_count = 0U;
    btn->long_reported = false;
}

void button_process(button_t *btn, uint16_t elapsed_ms)
{
    uint8_t level;
    bool raw_pressed;

    if ((btn == 0) || (btn->cfg.read_level == 0)) {
        return;
    }

    level = btn->cfg.read_level(btn->cfg.io_user_arg);
    raw_pressed = (level == btn->cfg.active_level);

    if (raw_pressed != btn->last_raw_pressed) {
        btn->last_raw_pressed = raw_pressed;
        btn->debounce_elapsed_ms = 0U;
    } else {
        btn->debounce_elapsed_ms = sat_add_u16(btn->debounce_elapsed_ms, elapsed_ms);
        if ((btn->debounce_elapsed_ms >= btn->cfg.debounce_ms) &&
            (btn->stable_pressed != raw_pressed)) {
            btn->stable_pressed = raw_pressed;
            if (btn->stable_pressed) {
                btn->press_elapsed_ms = 0U;
                btn->long_reported = false;
                button_emit(btn, BUTTON_EVENT_DOWN);
            } else {
                button_emit(btn, BUTTON_EVENT_UP);
                if (!btn->long_reported) {
                    if (btn->click_count < 2U) {
                        btn->click_count++;
                    }
                    btn->release_elapsed_ms = 0U;
                } else {
                    btn->click_count = 0U;
                    btn->release_elapsed_ms = 0U;
                }
            }
        }
    }

    if (btn->stable_pressed) {
        btn->press_elapsed_ms = sat_add_u16(btn->press_elapsed_ms, elapsed_ms);
        if ((!btn->long_reported) && (btn->press_elapsed_ms >= btn->cfg.long_press_ms)) {
            btn->long_reported = true;
            btn->click_count = 0U;
            btn->release_elapsed_ms = 0U;
            button_emit(btn, BUTTON_EVENT_LONG_PRESS);
        }
    } else if (btn->click_count > 0U) {
        btn->release_elapsed_ms = sat_add_u16(btn->release_elapsed_ms, elapsed_ms);
        if (btn->release_elapsed_ms >= btn->cfg.double_click_ms) {
            if (btn->click_count >= 2U) {
                button_emit(btn, BUTTON_EVENT_DOUBLE_CLICK);
            } else {
                button_emit(btn, BUTTON_EVENT_SINGLE_CLICK);
            }
            btn->click_count = 0U;
            btn->release_elapsed_ms = 0U;
        }
    }
}
