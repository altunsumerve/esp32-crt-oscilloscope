#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "video.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---- Configuration ---- */
#define ADC_CHANNEL          ADC1_CHANNEL_6   /* GPIO34 (pot / signal input) */
#define BUTTON_GPIO          0                /* on-board BOOT button: cycles modes */
#define SAMPLE_DELAY_US      50

#define TEST_SIGNAL_ENABLE   1
#define TEST_SIGNAL_GPIO     27
#define TEST_SIGNAL_FREQ_HZ  250

#define TRACE_THICKNESS      4
#define ADC_VREF_MV          3300

#define LEVEL_BG     0
#define LEVEL_TEXT   54
#define LEVEL_TRACE  54

#define MAX_WIDTH    800

static uint8_t *fb;
static int screen_w;
static int screen_h;

static int g_buffers[2][MAX_WIDTH];
static volatile int g_ready_index = 0;
static volatile int64_t g_window_us = 1;

typedef enum { MODE_AMPLITUDE, MODE_FREQUENCY, MODE_SHAPE, MODE_LIVE, MODE_COUNT } scope_mode_t;

/* ---- 5x7 font: space, 0-9, '.', ':', A-Z ---- */
static const char FONT_CHARS[] = " 0123456789.:ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const uint8_t FONT[][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* space */
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, /* 0 */
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, /* 1 */
    {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}, /* 2 */
    {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}, /* 3 */
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, /* 4 */
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, /* 5 */
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, /* 6 */
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, /* 7 */
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, /* 8 */
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, /* 9 */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}, /* . */
    {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00}, /* : */
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, /* A */
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, /* B */
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, /* C */
    {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}, /* D */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, /* E */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, /* F */
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, /* G */
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, /* H */
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, /* J */
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, /* K */
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, /* M */
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, /* N */
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, /* P */
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, /* Q */
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, /* R */
    {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, /* S */
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, /* T */
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
    {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}, /* V */
    {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, /* W */
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, /* X */
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, /* Y */
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, /* Z */
};

static inline void put_pixel(int x, int y, uint8_t level)
{
    if (x < 0 || x >= screen_w || y < 0 || y >= screen_h) return;
    fb[y * screen_w + x] = level;
}

static void draw_vline(int x, int y0, int y1, uint8_t level)
{
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; y++) put_pixel(x, y, level);
}

static void draw_char(int x, int y, char c, uint8_t level)
{
    const char *p = strchr(FONT_CHARS, toupper((unsigned char)c));
    if (!p) return;
    int idx = (int)(p - FONT_CHARS);
    for (int row = 0; row < 7; row++)
        for (int col = 0; col < 5; col++)
            if (FONT[idx][row] & (1 << (4 - col)))
                put_pixel(x + col, y + row, level);
}

static void draw_text(int x, int y, const char *s, uint8_t level)
{
    for (; *s; s++) { draw_char(x, y, *s, level); x += 6; }
}

static void draw_text_centered(int y, const char *s, uint8_t level)
{
    int w = (int)strlen(s) * 6;
    draw_text((screen_w - w) / 2, y, s, level);
}

static void plot_trace(const int *ybuf)
{
    int prev = ybuf[0];
    for (int x = 0; x < screen_w; x++)
    {
        for (int t = 0; t < TRACE_THICKNESS; t++)
            draw_vline(x, prev + t, ybuf[x] + t, LEVEL_TRACE);
        prev = ybuf[x];
    }
}

/* returns -1..+1 for the selected shape at fractional position f (0..1) */
static float shape_value(int shape, float f)
{
    float frac = f - floorf(f);
    switch (shape)
    {
        case 0: return sinf(2.0f * (float)M_PI * f);                 /* sine */
        case 1: return (frac < 0.5f) ? 1.0f : -1.0f;                 /* square */
        case 2: return (frac < 0.5f) ? (4.0f*frac - 1.0f)           /* triangle */
                                     : (3.0f - 4.0f*frac);
        default: return 2.0f * frac - 1.0f;                          /* sawtooth */
    }
}

static void adc_setup(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
}

static void button_setup(void)
{
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io);
}

#if TEST_SIGNAL_ENABLE
static void test_signal_setup(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = TEST_SIGNAL_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num   = TEST_SIGNAL_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 512,
        .hpoint     = 0,
    };
    ledc_channel_config(&channel);
}
#endif

static void acquire_task(void *arg)
{
    int write_index = 1;
    while (1)
    {
        int *buf = g_buffers[write_index];
        int64_t t0 = esp_timer_get_time();
        for (int x = 0; x < screen_w; x++)
        {
            buf[x] = adc1_get_raw(ADC_CHANNEL);
            esp_rom_delay_us(SAMPLE_DELAY_US);
        }
        g_window_us = esp_timer_get_time() - t0;
        g_ready_index = write_index;
        write_index ^= 1;
    }
}

void app_main(void)
{
    video_graphics(PAL_384x288, FB_FORMAT_GREY_8BPP);
    fb = video_get_frame_buffer_address();
    screen_w = video_get_width();
    screen_h = video_get_height();
    if (screen_w > MAX_WIDTH) screen_w = MAX_WIDTH;

    adc_setup();
    button_setup();
#if TEST_SIGNAL_ENABLE
    test_signal_setup();
#endif

    xTaskCreatePinnedToCore(acquire_task, "acquire", 4096, NULL, 5, NULL, 1);

    static int ybuf[MAX_WIDTH];
    char line1[40];
    char line2[40];
    const char *shape_names[] = { "SINE", "SQUARE", "TRIANGLE", "SAW" };

    scope_mode_t mode = MODE_AMPLITUDE;
    int btn_low = 0;
    int btn_latched = 0;
    float phase = 0.0f;
    const int cy = screen_h / 2;
    const int max_amp = screen_h / 2 - 22;

    while (1)
    {
        const int *buf = g_buffers[g_ready_index];

        /* button: cycle mode on press (active low, debounced) */
        int btn = gpio_get_level(BUTTON_GPIO);
        if (btn == 0) {
            if (btn_low < 100) btn_low++;
        } else {
            btn_low = 0;
            btn_latched = 0;
        }
        if (btn_low >= 2 && !btn_latched) {
            btn_latched = 1;
            mode = (mode + 1) % MODE_COUNT;
        }

        /* pot value = average of the buffer (0..4095) */
        long sum = 0;
        int mn = 4095, mx = 0;
        for (int x = 0; x < screen_w; x++)
        {
            int v = buf[x];
            sum += v;
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        }
        int pot = (int)(sum / screen_w);

        line2[0] = '\0';

        if (mode == MODE_AMPLITUDE)
        {
            float amp = (pot / 4095.0f) * max_amp;
            for (int x = 0; x < screen_w; x++)
            {
                float f = 4.0f * x / screen_w + phase;
                ybuf[x] = cy - (int)(amp * shape_value(0, f));
            }
            snprintf(line1, sizeof(line1), "MODE AMPLITUDE");
            snprintf(line2, sizeof(line2), "AMP %d", pot * 100 / 4095);
        }
        else if (mode == MODE_FREQUENCY)
        {
            float cycles = 1.0f + (pot / 4095.0f) * 15.0f;
            for (int x = 0; x < screen_w; x++)
            {
                float f = cycles * x / screen_w + phase;
                ybuf[x] = cy - (int)(max_amp * shape_value(0, f));
            }
            snprintf(line1, sizeof(line1), "MODE FREQUENCY");
            snprintf(line2, sizeof(line2), "CYCLES %d", (int)(cycles + 0.5f));
        }
        else if (mode == MODE_SHAPE)
        {
            int shape = pot * 4 / 4096;
            if (shape > 3) shape = 3;
            for (int x = 0; x < screen_w; x++)
            {
                float f = 4.0f * x / screen_w + phase;
                ybuf[x] = cy - (int)(max_amp * shape_value(shape, f));
            }
            snprintf(line1, sizeof(line1), "MODE SHAPE");
            snprintf(line2, sizeof(line2), "%s", shape_names[shape]);
        }
        else /* MODE_LIVE */
        {
            int mean = (int)(sum / screen_w);
            for (int x = 0; x < screen_w; x++)
            {
                int centered = buf[x] - mean;
                ybuf[x] = cy - centered * (screen_h / 2) / 2048;
            }
            int hyst = (mx - mn) / 8; if (hyst < 30) hyst = 30;
            int cross = 0, state = 0;
            for (int x = 0; x < screen_w; x++)
            {
                if (!state && buf[x] > mean + hyst) { state = 1; cross++; }
                else if (state && buf[x] < mean - hyst) { state = 0; }
            }
            int freq_hz = (g_window_us > 0) ? (int)((int64_t)cross * 1000000 / g_window_us) : 0;
            int vdc_mv = mean * ADC_VREF_MV / 4095;
            int vpp_mv = (mx - mn) * ADC_VREF_MV / 4095;
            snprintf(line1, sizeof(line1), "MODE LIVE   F %dHZ", freq_hz);
            snprintf(line2, sizeof(line2), "VDC %d.%02dV  VPP %d.%02dV",
                     vdc_mv / 1000, (vdc_mv % 1000) / 10,
                     vpp_mv / 1000, (vpp_mv % 1000) / 10);
        }

        video_wait_frame();
        memset(fb, LEVEL_BG, (size_t)screen_w * screen_h);
        plot_trace(ybuf);
        draw_text_centered(12, line1, LEVEL_TEXT);
        draw_text_centered(22, line2, LEVEL_TEXT);

        phase += 0.15f;
    }
}
