/*  $Header$
 *  
 *  Copyright (C) 2003 Intersil Americas Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _ISLMVC_MGT_H
#define _ISLMVC_MGT_H

#include <device_config.h>

/* Begin of DEFINES */

/* 
 * Uncomment this to enable the code for making a table of joined clients.
 * If you want to use this, you also need to install handlers that add the client
 * on an association and remove them on a disassociation.
 */
//#define ENABLE_CLIENT_TABLE_CODE

#ifdef ENABLE_CLIENT_TABLE_CODE
#define MAC_HASH_BITS 8
#define MAC_HASH_SIZE (1 << MAC_HASH_BITS)

/* Holds information about associated clients */
struct client_information
{
    struct client_information *next_client;
    struct client_information **pprev_client;
    char                      mac_addr[ETH_ALEN];
};

#endif /* ENABLE_CLIENT_TABLE_CODE */

#define MAX_ASSOC_IDS   MAX_ASSOCIATIONS+1 /* Association id can be -1. We add 1, so -1 maps on 0. */ 
#define MAX_IE_ELEMENTS 20

/* Include this definition if IAPP duplicate frame needs to be generated by the
   driver in order to update all forwarding tables in the connected layer 2
   devices */
#define HANDLE_IAPP_IN_DRIVER

/* Comment this out when an user space application is handling client association and authentication. 
   This is temporarily used when no user space app is ready to do this for us.
   FIXME, undef this when authentication application is ready!!!
 */
// #define HANDLE_ASSOC_IN_DRIVER

//FIXME, check this for ISL3893.
#define ETH_LAN     2
#define ETH_WLAN    1
#define ETH_WAN     3

#define LED_SET     1
#define LED_CLEAR   2

/* Structure definitions */

struct information_element
{
    struct information_element *next;   /* Pointer to next information element. */
    unsigned char elem_id;              /* Identifier of the element. */
    short size;                         /* Size of information element. */
    char  *data;                        /* Data of information element (including elem_id and size) */
};

struct frame_attachment
{
    struct frame_attachment *next;       /* Pointer to next frame_attachment. */
    short  frame_type;                   /* Frame type of this attachment. */
    struct information_element *ie_root; /* Pointer to first information element for this attachment. */
    char   *all_ie_data;                 /* Pointer to (temporary) buffer holding all information elements. */
};


/* mgmt */
void islmvc_mgt_init(struct net_device *dev);
int islmvc_send_mvc_msg(struct net_device *dev, int operation, int oid, long* data, unsigned long data_len);

int mgmt_set_led(struct net_device *dev, int mode);

unsigned char * pda_get_mac_address(void);
int set_rssi_linear_approximation_db(struct net_device *dev);
int set_zif_tx_iq_calibr_data(struct net_device *dev);
int set_hw_interface_variant(struct net_device *dev);

#endif /* _ISLMVC_MGT_H */
