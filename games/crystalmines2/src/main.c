/* main.c - Crystal Mines II host: run the *recompiled* game C.
 *
 * Executes the recompiled functions directly (no interpreter). Loads a post-init
 * RAM image (game resident, jump tables built), registers the recompiled
 * functions, and calls the game entry ($5259). The runtime's cooperative ticks
 * drive the timers and deliver the VBL interrupt into the recompiled handler;
 * the recompiled game builds its sprite lists and the emulated Suzy blitter
 * draws them. The main loop never returns, so a bounded run longjmps out of the
 * frame hook after N frames.
 *
 *   crystalmines2 <ram.bin> [frames] [out.ppm]
 *
 * Produce the inputs with the toolkit (see the repo README):
 *   lynxrun --snapshot "Crystal Mines II (USA, Europe).lnx" lynxboot.img ram.bin 60
 *   m65c02recomp recompbin ram.bin 0x0000 generated \
 *       0x5259 0x5757 0x56C6 0x56CF 0x042B 0x051A
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "lynxrecomp/recomp_rt.h"
#include "lynxrecomp/mem.h"
#include "lynxrecomp/suzy.h"
#include "lynxrecomp/mikey.h"
#include "lynxrecomp/timer.h"
#include "lynxrecomp/input.h"
#include "recomp_funcs.h"

#define GAME_ENTRY 0x5259

static jmp_buf  g_exit;
static long     g_frames = 0, g_max = 0;
static const char *g_out = "frame.ppm";

static void on_frame(void) {
    if (g_max && ++g_frames >= g_max) {
        lynx_video_write_ppm(g_out);
        longjmp(g_exit, 1);
    }
}

static long g_gap_calls = 0; static uint16_t g_gaps[64]; static int g_ngaps = 0;
static void record_gap(uint16_t addr) {
    g_gap_calls++;
    for (int i = 0; i < g_ngaps; i++) if (g_gaps[i] == addr) return;
    if (g_ngaps < 64) g_gaps[g_ngaps++] = addr;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <ram.bin> [frames] [out.ppm]\n", argv[0]); return 2; }
    g_max = (argc > 2) ? strtol(argv[2], NULL, 0) : 400;
    g_out = (argc > 3) ? argv[3] : "frame.ppm";

    lynx_mem_init(); lynx_suzy_init(); lynx_mikey_init(); lynx_timer_init();
    lynx_input_set(0x00, 0x00);

    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "cannot read %s\n", argv[1]); return 1; }
    size_t n = fread(lynx_ram, 1, 0x10000, f);
    fclose(f);
    printf("loaded %zu bytes of RAM image from %s\n", n, argv[1]);

    lynx_dispatch_reset();
    lynx_recomp_register();
    lynx_dispatch_fallback = record_gap;
    lynx_frame_hook = on_frame;

    printf("running recompiled Crystal Mines II from $%04X for %ld frames...\n", GAME_ENTRY, g_max);
    if (setjmp(g_exit) == 0) {
        lynx_call_addr(GAME_ENTRY);
        printf("note: game entry returned (unexpected for a main loop)\n");
    }
    printf("rendered %ld frames -> %s\n", g_frames, g_out);
    if (g_ngaps) {
        printf("WARNING: %ld dispatches to %d unrecompiled addrs (add as seeds):\n  ", g_gap_calls, g_ngaps);
        for (int i = 0; i < g_ngaps; i++) printf("0x%04X ", g_gaps[i]);
        printf("\n");
    }
    return 0;
}
