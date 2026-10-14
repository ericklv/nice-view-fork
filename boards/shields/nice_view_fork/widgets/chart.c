#include <zephyr/kernel.h>
#include "chart.h"

#define CHART_X 3
#define CHART_Y 72
#define CHART_ROWS 6
#define CHART_SQUARE 2
#define CHART_STEP 4
#define WPM_MAX 100

void draw_chart_status(const struct canvas *canvas, const struct status_state *state) {
    for (int col = 0; col < WPM_HISTORY; col++) {
        int rows = MIN((state->wpm[col] * CHART_ROWS + WPM_MAX / 2) / WPM_MAX, CHART_ROWS);

        for (int row = 0; row < rows; row++) {
            canvas_fill_rect(canvas, CHART_X + col * CHART_STEP,
                             CHART_Y + (CHART_ROWS - 1 - row) * CHART_STEP, CHART_SQUARE,
                             CHART_SQUARE, 0);
        }
    }
}
