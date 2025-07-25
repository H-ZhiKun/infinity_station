/* ==========================================================================
 *
 *  File      : DG_HDREG.H
 *
 *  Purpose   : Duagon Host Device IO -
 *              Register access interface
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

#ifndef DG_HDREG_H
#define DG_HDREG_H

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_hdio.h"

/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

#ifndef GET_REG_CONFIG
#	define GET_REG_CONFIG(c,i)  (DG_HDREG_CONFIG*)&dg_card_config[(c)].dg_hdio_configs[(i)]
#else
#    error GET_REG_CONFIG(c,i) already defined.
#endif

#define GET_REG_BASE(c,i)    ((GET_REG_CONFIG(c,i))->base)


#define MIN(a,b) ((a)>(b)?(b):(a))
#define FIFO_SIZE(a) (a == 0)? 1 : (a << 7)
/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */
typedef struct {
	DG_HDIO_CONFIG_SHORT dg_hdio_common_config;
    /* same as DG_HDIO_CONFIG up to here */
    DG_U32 fifo_size;
    void* base;
    DG_U8 padding[HDIO_CONFIG_SIZE
                        - sizeof(DG_U32) /* fifo_size */
                        - sizeof(void*)  /* base */
                    ];
#ifdef __GNUC__
} __attribute__((__may_alias__)) DG_HDREG_CONFIG;
#else
} DG_HDREG_CONFIG;
#endif

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */
DG_DECL_PUBLIC
DG_U8
hd_reg_get_status
(
        REG_SIZE* addr,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_install
(
		DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_init
(
        DG_U8 channel_num,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_transmit
(
        DG_U8 channel_num,
        DG_U32 *curr_size,
        DG_U8* txBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_receive
(
        DG_U8 channel_num,
        DG_U32 *curr_size,
        DG_U8* txBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_wait_to_send
(
        DG_U8 channel_num
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_set_loopback
(
        DG_U8 channel_num,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_release_loopback
(
        DG_U8 channel_num,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_sanity_check
(
        DG_U8 channel_num,
        DG_U8 card_index
);

#endif /* DG_HDREG_H */
