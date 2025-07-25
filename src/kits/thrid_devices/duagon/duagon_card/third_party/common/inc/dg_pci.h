/* ==========================================================================
 *
 *  File      : DG_PCI.H
 *
 *  Purpose   : PCI Library
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

#ifndef DG_PCI_H
#define DG_PCI_H


#include "dg_hdio.h"
/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  NOTE:
 *  The obvious few that compilers may define for you.
 *  But in case they don't:
 * --------------------------------------------------------------------------
 */
#ifndef NULL
#   define NULL     (0)
#endif

#ifndef TRUE
#   define TRUE     (1==1)
#endif

#ifndef FALSE
#   define FALSE    (!TRUE)
#endif


/* ==========================================================================
 *
 *  Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Data Types
 * --------------------------------------------------------------------------
 */
typedef DG_U8    PCI_UINT8;
typedef DG_U16   PCI_UINT16;
typedef DG_U32   PCI_UINT32;

/* --------------------------------------------------------------------------
 *  Enumerated Type : PCI_RESULT
 *
 *  Purpose         : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    PCI_OK           = 0,   /* correct termination                         */
    PCI_ERROR        = 1   /* unspecified error                           */
}   PCI_RESULT;

/* --------------------------------------------------------------------------
 *  Structured Type   : PCI_CONFIG
 * --------------------------------------------------------------------------
 */
typedef struct
{
    /* common header */
    PCI_UINT16  vendor_id;
    PCI_UINT16  device_id;

    PCI_UINT16  command;
    PCI_UINT16  status;

    PCI_UINT8   revision_id;
    PCI_UINT8   programming_interface;
    PCI_UINT8   sub_class;
    PCI_UINT8   base_class;

    PCI_UINT8   cache_line_size;
    PCI_UINT8   latency_timer;
    PCI_UINT8   header_type;
    PCI_UINT8   bist;

    /* base address registers */
    PCI_UINT32  base[6];
    PCI_UINT32  size[6];

    /* miscellaneous registers */
    PCI_UINT16  subsystem_vendor_id;
    PCI_UINT16  subsystem_id;

    PCI_UINT8   irq_pin;
    PCI_UINT8   irq_line;

    /* control */
    PCI_UINT8   bus;
    PCI_UINT8   dev;
    PCI_UINT8   func;

}   PCI_CONFIG;


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : pci_probe
 * --------------------------------------------------------------------------
 */
extern
PCI_UINT16
pci_probe
(
    PCI_UINT8   bus,
    PCI_UINT8   dev,
    PCI_UINT8   func,
    PCI_UINT8   print,
    PCI_UINT8   sizing,
    PCI_CONFIG  *cfg
);

/* --------------------------------------------------------------------------
 *  Procedure : pci_scan
 * --------------------------------------------------------------------------
 */
extern
PCI_UINT16
pci_scan (void);

/* --------------------------------------------------------------------------
 *  Procedure : pci_search
 * --------------------------------------------------------------------------
 */
extern
PCI_UINT16
pci_search
(
    PCI_CONFIG  *pci_cfg,
    PCI_UINT16  *found
);

/* --------------------------------------------------------------------------
 *  Procedure : pci_init
 * --------------------------------------------------------------------------
 */
extern
PCI_RESULT
pci_init (void);


#endif /* PCI_LIB_H */
