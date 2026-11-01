#include "peltier_fsm.h"
#include "peltier_aod4184.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "PELTIER_FSM";

static int64_t now_ms(void)
{
    return esp_timer_get_time() / 1000;
}

void peltier_fsm_init(peltier_fsm_state_t *fsm)
{
    if (!fsm) return;
    fsm->peltier_on = false;
    fsm->overheat_latched = false;
    fsm->last_switch_time_ms = now_ms();
    fsm->error_flags = PELTIER_ERR_NONE;
    fsm->filtered_temp_c = 0.0f;
    fsm->filtered_hum_rh = 0.0f;
    fsm->temp_hot_c = 0.0f;
    peltier_aod4184_set_state(false);
}

void peltier_fsm_update(peltier_fsm_state_t *fsm,
                        const app_config_t *cfg,
                        float temp_c,
                        float hum_rh,
                        float temp_hot_c,
                        bool sht35_ok,
                        bool ntc_ok,
                        bool can_ok)
{
    if (!fsm || !cfg) return;

    fsm->filtered_temp_c = temp_c;
    fsm->filtered_hum_rh = hum_rh;
    fsm->temp_hot_c = temp_hot_c;
    fsm->error_flags = PELTIER_ERR_NONE;

    // 1. Assess hardware errors and overheat protection
    if (!cfg->system_enable) {
        fsm->error_flags |= PELTIER_ERR_SYS_DISABLED;
    }
    if (!sht35_ok) {
        fsm->error_flags |= PELTIER_ERR_SHT35_FAULT;
    }
    if (!ntc_ok) {
        fsm->error_flags |= PELTIER_ERR_NTC_FAULT;
    } else {
        if (temp_hot_c > cfg->ntc_limit_c) {
            fsm->overheat_latched = true;
            ESP_LOGE(TAG, "OVERHEAT PROTECTION: T_hot=%.1fC exceeded limit %.1fC!", temp_hot_c, cfg->ntc_limit_c);
        } else if (fsm->overheat_latched && temp_hot_c < cfg->ntc_recover_c) {
            fsm->overheat_latched = false;
            ESP_LOGI(TAG, "Hot surface temperature is safe (T_hot=%.1fC < %.1fC).", temp_hot_c, cfg->ntc_recover_c);
        }
    }
    if (fsm->overheat_latched) {
        fsm->error_flags |= PELTIER_ERR_OVERHEAT;
    }
    if (!can_ok) {
        fsm->error_flags |= PELTIER_ERR_CAN_TIMEOUT;
    }

    // 2. Fail-safe mechanism: Immediately emergency disconnect pin D4 (ignores Min ON time)
    if (fsm->error_flags != PELTIER_ERR_NONE) {
        if (fsm->peltier_on) {
            fsm->peltier_on = false;
            fsm->last_switch_time_ms = now_ms();
            peltier_aod4184_set_state(false);
            ESP_LOGW(TAG, "Emergency Peltier disconnect (Fail-safe), Error code: 0x%02lX", (unsigned long)fsm->error_flags);
        }
        return;
    }
    
    // 3. On-Off + Hysteresis algorithm
    bool target_state = fsm->peltier_on;
    if (!fsm->peltier_on) {
        // TURN ON when (T > T_on) OR (H > H_on)
        // if (temp_c > cfg->temp_on_c || hum_rh > cfg->hum_on_rh) {
        if (hum_rh > cfg->hum_on_rh) {
            target_state = true;
        }
    } else {
        // TURN OFF when (T < T_off) AND (H < H_off)
        // if (temp_c < cfg->temp_off_c && hum_rh < cfg->hum_off_rh) {
        if (hum_rh < cfg->hum_off_rh) {
            target_state = false;
        }
    }

    // 4. Minimum state hold time constraint (Min ON/OFF time: 10 - 30s)
    if (target_state != fsm->peltier_on) {
        int64_t elapsed_ms = now_ms() - fsm->last_switch_time_ms;
        int64_t min_hold_ms = (int64_t)cfg->min_on_off_sec * 1000;

        if (elapsed_ms >= min_hold_ms) {
            fsm->peltier_on = target_state;
            fsm->last_switch_time_ms = now_ms();
            peltier_aod4184_set_state(fsm->peltier_on);
            ESP_LOGI(TAG, "Peltier state changed to %s (T=%.1fC, H=%.1f%%, T_hot=%.1fC)",
                     fsm->peltier_on ? "ON" : "OFF", temp_c, hum_rh, temp_hot_c);
        }
    }
}