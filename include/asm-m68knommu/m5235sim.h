/****************************************************************************/

/*
 *	m5235sim.h -- ColdFire 5235 System Integration Module support.
 *
 *	(C) Copyright 2003, Greg Ungerer (gerg@snapgear.com)
 */

/****************************************************************************/
#ifndef	m5235sim_h
#define	m5235sim_h
/****************************************************************************/

#include <linux/config.h>

/*
 *	Define the 5235 SIM register set addresses.
 */
#define	MCFICM_INTC0		0x0c00		/* Base for Interrupt Ctrl 0 */
#define	MCFICM_INTC1		0x0d00		/* Base for Interrupt Ctrl 0 */
#define	MCFINTC_IPRH		0x00		/* Interrupt pending 32-63 */
#define	MCFINTC_IPRL		0x04		/* Interrupt pending 1-31 */
#define	MCFINTC_IMRH		0x08		/* Interrupt mask 32-63 */
#define	MCFINTC_IMRL		0x0c		/* Interrupt mask 1-31 */
#define	MCFINTC_INTFRCH		0x10		/* Interrupt force 32-63 */
#define	MCFINTC_INTFRCL		0x14		/* Interrupt force 1-31 */
#define	MCFINTC_IRLR		0x18		/* */
#define	MCFINTC_IACKL		0x19		/* */
#define	MCFINTC_ICR0		0x40		/* Base ICR register */

#define	MCFINT_UART0		13		/* Interrupt number for UART0 */
#define	MCFINT_QSPI		18		/* Interrupt number for QSPI */
#define	MCFINT_PIT1		36		/* Interrupt number for PIT1 */

#define MCFINT_CAN_BUF00	43		/* Interrupt for CAN Buffer*/
#define MCFINT_CAN_BUF01	44		/* Interrupt for CAN Buffer*/
#define MCFINT_CAN_WARN		59		/* Interrupt for CAN Error int*/
#define MCFINT_CAN_BUSOFF	60		/* Interrupt for CAN On/Off */

#define MCF5235_GPIO_PFECI2CPAR 0x100047        /* */

/****************************************************************************/
#endif	/* m5235sim_h */
