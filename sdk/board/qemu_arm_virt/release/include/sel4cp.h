/*
 * Copyright 2021, Breakaway Consulting Pty. Ltd.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
/* seL4 Core Platform interface */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define __thread
#include <sel4/sel4.h>

typedef unsigned int sel4cp_channel;
typedef unsigned int sel4cp_pd;
typedef seL4_MessageInfo_t sel4cp_msginfo;

#define BASE_OUTPUT_NOTIFICATION_CAP 10
#define BASE_ENDPOINT_CAP 74
#define BASE_IRQ_CAP 138
#define BASE_TCB_CAP 202

#define SEL4CP_MAX_CHANNELS 63

/* User provided functions */
void init(void);
void notified(sel4cp_channel ch);
sel4cp_msginfo protected(sel4cp_channel ch, sel4cp_msginfo msginfo);
void fault(sel4cp_channel ch, sel4cp_msginfo msginfo);

extern char sel4cp_name[16];

/*
 * Output a single character on the debug console.
 */
void sel4cp_dbg_putc(int c);

/*
 * Output a NUL terminated string to the debug console.
 */
void sel4cp_dbg_puts(const char *s);

static inline void
sel4cp_internal_crash(seL4_Error err)
{
    /*
     * Currently crash be dereferencing NULL page
     *
     * Actually derference 'err' which means the crash reporting will have
     * `err` as the fault address. A bit of a cute hack. Not a good long term
     * solution but good for now.
     */
    int *x = (int *)(uintptr_t) err;
    *x = 0;
}

static inline void
sel4cp_notify(sel4cp_channel ch)
{
    seL4_Signal(BASE_OUTPUT_NOTIFICATION_CAP + ch);
}

static inline void
sel4cp_irq_ack(sel4cp_channel ch)
{
    seL4_IRQHandler_Ack(BASE_IRQ_CAP + ch);
}

static inline void
sel4cp_pd_restart(sel4cp_pd pd, uintptr_t entry_point)
{
    seL4_Error err;
    seL4_UserContext ctxt = {0};
    ctxt.pc = entry_point;
    err = seL4_TCB_WriteRegisters(
        BASE_TCB_CAP + pd,
        true,
        0, /* No flags */
        1, /* writing 1 register */
        &ctxt
    );

    if (err != seL4_NoError) {
        sel4cp_dbg_puts("sel4cp_pd_restart: error writing registers\n");
        sel4cp_internal_crash(err);
    }
}

static inline void
sel4cp_pd_stop(sel4cp_pd pd)
{
    seL4_Error err;
    err = seL4_TCB_Suspend(BASE_TCB_CAP + pd);
    if (err != seL4_NoError) {
        sel4cp_dbg_puts("sel4cp_pd_restart: error writing registers\n");
        sel4cp_internal_crash(err);
    }
}

static inline sel4cp_msginfo
sel4cp_ppcall(sel4cp_channel ch, sel4cp_msginfo msginfo)
{
    return seL4_Call(BASE_ENDPOINT_CAP + ch, msginfo);
}

static inline sel4cp_msginfo
sel4cp_msginfo_new(uint64_t label, uint16_t count)
{
    return seL4_MessageInfo_new(label, 0, 0, count);
}

static inline uint64_t
sel4cp_msginfo_get_label(sel4cp_msginfo msginfo)
{
    return seL4_MessageInfo_get_label(msginfo);
}

static void
sel4cp_mr_set(uint8_t mr, uint64_t value)
{
    seL4_SetMR(mr, value);
}

static uint64_t
sel4cp_mr_get(uint8_t mr)
{
    return seL4_GetMR(mr);
}
