/* This file is part of the bladeRF project:
 *   http://www.github.com/nuand/bladeRF
 *
 * Copyright (c) 2018 Nuand LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef BLADERF_NIOS_PC_SIMULATION

#include "devices.h"
#include "devices_rfic.h"

#ifdef BLADERF_NIOS_LIBAD936X

/******************************************************************************/
/* Queue Management */
/******************************************************************************/

/**
 * @brief       Enqueue a command
 *
 * @param       q       Queue
 * @param[in]   ch      Channel
 * @param[in]   cmd     Command
 * @param[in]   value   Value to pass to command
 *
 * @return  queue size after the enqueue operation, or COMMAND_QUEUE_FULL if
 *          the queue was full.
 */
uint8_t rfic_enqueue(struct rfic_queue *q,
                     uint8_t ch,
                     uint8_t cmd,
                     uint64_t value)
{
    if (q->count >= COMMAND_QUEUE_MAX) {
        return COMMAND_QUEUE_FULL;
    }

    q->entries[q->ins_idx].ch    = ch;
    q->entries[q->ins_idx].cmd   = cmd;
    q->entries[q->ins_idx].value = value;
    q->entries[q->ins_idx].state = ENTRY_STATE_NEW;
    q->entries[q->ins_idx].rv    = 0xFF;

    q->ins_idx = (q->ins_idx + 1) & (COMMAND_QUEUE_MAX - 1);

    return ++q->count;
}

/**
 * @brief       Dequeue a command
 *
 * @param       q   Queue
 * @param[out]  e   Pointer to a queue entry struct
 *
 * @return  queue size after the dequeue operation, or COMMAND_QUEUE_EMPTY if
 *          the queue was empty.
 */
uint8_t rfic_dequeue(struct rfic_queue *q, struct rfic_queue_entry *e)
{
    if (0 == q->count) {
        return COMMAND_QUEUE_EMPTY;
    }

    if (e != NULL) {
        memcpy(&e, &q->entries[q->rem_idx], sizeof(e[0]));
    }

    q->rem_idx = (q->rem_idx + 1) & (COMMAND_QUEUE_MAX - 1);

    return --q->count;
}

/**
 * @brief       Get the state of the next item in the queue
 *
 * @param[in]   q   Queue
 *
 * @return      pointer to the next item in the queue, or NULL if the
 *              queue is empty
 */
struct rfic_queue_entry *rfic_queue_peek(struct rfic_queue *q)
{
    if (0 == q->count) {
        return NULL;
    } else {
        return &q->entries[q->rem_idx];
    }
}

/**
 * @brief      Reset/initialize the queue
 *
 * @param      q    Queue
 */
void rfic_queue_reset(struct rfic_queue *q)
{
    q->count = 0;

    for (size_t i = 0; i < COMMAND_QUEUE_MAX; i++) {
        q->entries[i].state = ENTRY_STATE_INVALID;
    }

    q->last_rv = 0xFF;
    q->rem_idx = 0;
    q->ins_idx = 0;
}

#endif  // BLADERF_NIOS_LIBAD936X

#endif  // !BLADERF_NIOS_PC_SIMULATION
