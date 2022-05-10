#ifndef __J1939_H
#define __J1939_H

#include "bsp.h"


struct _pdu_t{

	u32 b8SA      :   8;
	u32 b8PS_DA   :   8;
	u32 b8PF      :   8;
	u32 b1DP      :   1;
	u32 b1R		  :   1;
	u32 b3P       :   3;
	u32 b3unused  :   1;
	
}__attribute__((packed));

union _pdu_u{

	u32 u32ExtId;
	struct _pdu_t	tpdu;
	
}__attribute__((packed));

typedef __packed struct __j1939_packet_t {

	union _pdu_u updu;
	u8 u8len;
	u8 au8data[8];

}t_j1939_packet, *pt_j1939_packet;


void j1939_rx_data(void * pvcan_rx_header, u8 *pu8data);
int j1939_rx_data_handle(const pt_j1939_packet ptj1939_rxpacket);
#endif