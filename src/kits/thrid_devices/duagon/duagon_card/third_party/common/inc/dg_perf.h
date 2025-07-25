/* ==========================================================================
 *
 *  File      : DG_PERF.H
 *
 *  Purpose   : Performance Measurement for RPC Calls
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */

#ifndef DG_PERF_H
#define DG_PERF_H
#include "os_def.h"

#define DG_PERF_MAX_MEASUREMENTS 256

#ifndef DG_MULTI_CARD

extern
void
dg_perf_init
(
        void
);

extern
void
dg_perf_print_statistics
(
        void
);

extern
DG_BOOL
dg_perf_print_statistic_line
(
        DG_CHAR8 *Line,
        DG_CHAR8 length,
        DG_S32 *proto_index,
        DG_S32 *meas_index
);
#endif

extern
void
dg_perf_init_nbr
(
        DG_U8 card_number
);

extern
void
dg_perf_print_statistics_nbr
(
        DG_U8 card_number
);

extern
DG_BOOL
dg_perf_print_statistic_line_nbr
(
        DG_CHAR8 *Line,
        DG_CHAR8 length,
        DG_S32 *proto_index,
        DG_S32 *meas_index,
        DG_U8 card_number
);

typedef DG_BOOL (*PERF_PRINT_FN)(
        DG_CHAR8 *Line,
        DG_CHAR8 length,
        DG_S32 *proto_index,
        DG_S32 *meas_index
);


typedef void (*PerfFunc)(DG_U8 channel, DG_U8 card_index);
typedef DG_U8 (*PerfFuncU8)(DG_U8 channel, DG_U8 card_index);

/* workaround for MVB MDFULL performance measurements */
#define MVB_PROTOCOL_ID         0x10
#define MVB_PROTOCOL_CHANNEL    0
#define CMD_MVB_PD_GET          0x01
#define CMD_MVB_PD_PUT          0x02


typedef struct dg_perf_measurement
{
    DG_U8   protocol;
    DG_U8   command;
    UINT64  curr_start;
    UINT64  avg_time;
    UINT64  max_time;
    UINT64  min_time;
    DG_U32  nb_calls;
#ifdef MEASURE_DETAILED
    UINT64  get_mutex_avg;
    UINT64  get_mutex_min;
    UINT64  get_mutex_max;
    UINT64  send_start;
    UINT64  send_done;
    UINT64  send_avg;
    UINT64  send_min;
    UINT64  send_max;
    UINT64  recv_h_done;
    UINT64  recv_h_avg;
    UINT64  recv_h_min;
    UINT64  recv_h_max;
    UINT64  recv_p_done;
    UINT64  recv_p_avg;
    UINT64  recv_p_min;
    UINT64  recv_p_max;
#ifdef MEASURE_BYTE_ACCESS
    UINT64  write_byte_avg_min;
    UINT64  write_byte_avg_max;
    DG_U32  write_byte_cnt_max;
    UINT64  write_byte_min;
    UINT64  write_byte_max;
    UINT64  read_byte_avg_min;
    UINT64  read_byte_avg_max;
    DG_U32  read_byte_cnt_max;
    UINT64  read_byte_min;
    UINT64  read_byte_max;
    UINT64  read_status_avg_min;
    UINT64  read_status_avg_max;
    DG_U32  read_status_cnt_max;
    UINT64  read_status_min;
    UINT64  read_status_max;
#endif
    DG_U32 cnt_bucket;
    DG_U32 bucket_latency[20];
    UINT64  bucket_min;
    UINT64  bucket_max;
    UINT64  bucket_delta;
#endif
} DG_PERF_MEASUREMENT_DEF;

#ifdef MEASURE_BYTE_ACCESS
typedef struct {
    UINT64  access_byte_start[DG_NR_OF_CHANNELS];
    UINT64  write_byte_sum[DG_NR_OF_CHANNELS];
    UINT64  write_byte_min[DG_NR_OF_CHANNELS];
    UINT64  write_byte_max[DG_NR_OF_CHANNELS];
    UINT64  write_byte_cnt[DG_NR_OF_CHANNELS];
    UINT64  read_byte_sum[DG_NR_OF_CHANNELS];
    UINT64  read_byte_min[DG_NR_OF_CHANNELS];
    UINT64  read_byte_max[DG_NR_OF_CHANNELS];
    UINT64  read_byte_cnt[DG_NR_OF_CHANNELS];
    UINT64  read_status_sum[DG_NR_OF_CHANNELS];
    UINT64  read_status_min[DG_NR_OF_CHANNELS];
    UINT64  read_status_max[DG_NR_OF_CHANNELS];
    UINT64  read_status_cnt[DG_NR_OF_CHANNELS];
} BYTE_ACCESS_MEASUREMENTS;
#endif


#endif /* DG_PERF_H */
