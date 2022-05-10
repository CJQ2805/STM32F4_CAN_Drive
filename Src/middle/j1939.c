#include "j1939.h"

/**
@brief CJQ2805
���ﻹ��ʱ��Щ�궨������
*/
#define TP_CM_START_OR_END   0xEC         //����Э�����ӹ��� 
#define TP_CM_DATA           0xEB         //���ݷ���

#define TP_CM_RTS            0x10         //TP.CM_RTS ����ģʽ��������()
#define TP_CM_CTS            0x11         //TP.CM_CTS ����ģʽ��׼������(���߷��ͷ�������)
#define TP_CM_END_OF_MSG_ACK 0x13        //TP.CM_EndofMsgAck ��Ϣ����Ӧ��
#define TP_CM_ABORT          0xFF        //TP.CM_Abort ��Ϣ����Ӧ��


void j1939_set_packet(pt_j1939_packet ptj1939_packet, u8 u8P, u8 u8DP, u8 u8cmd, u8 u8dest_addr, u8 u8source_addr, void* pvdata, u8 u8len)
{
	ptj1939_packet->updu.tpdu.b3unused = 0;
	ptj1939_packet->updu.tpdu.b3P = u8P;
	ptj1939_packet->updu.tpdu.b1R = 0;
	ptj1939_packet->updu.tpdu.b1DP = u8DP; 
	ptj1939_packet->updu.tpdu.b8PF = u8cmd;
	ptj1939_packet->updu.tpdu.b8PS_DA = u8dest_addr;
	ptj1939_packet->updu.tpdu.b8SA = u8source_addr;
	
	ptj1939_packet->u8len    = u8len <= sizeof(ptj1939_packet->au8data) ? u8len : 0;
	
	if(pvdata)
		memcpy(ptj1939_packet->au8data, pvdata, ptj1939_packet->u8len);
}


void j1939_rx_data(void * pvcan_rx_header, u8 *pu8data)
{
	t_j1939_packet  tj1939_packet;
	CAN_RxHeaderTypeDef * ptcan_rx_header = (CAN_RxHeaderTypeDef *)pvcan_rx_header;
	
	memset((u8*)&tj1939_packet, 0x00,sizeof(tj1939_packet));
	
	tj1939_packet.u8len = ptcan_rx_header->DLC;
	tj1939_packet.updu.u32ExtId  = ptcan_rx_header->ExtId;
	
	//���ｫ���ݴ浽��tj1939_packet�У����Ǵ�����ȥ��Ҫ����һ��ȫ������
	memcpy(tj1939_packet.au8data, pu8data, sizeof(tj1939_packet.au8data));
	
	j1939_rx_data_handle(&tj1939_packet);
}

int j1939_rx_data_handle(const pt_j1939_packet ptj1939_rxpacket)
{
	int ierro = -1;
	/*
		�Ƕ�����ͱ���
	*/
	//�������Ϊʲô�������ж��Ƿ�Ϊ���
	if((TP_CM_START_OR_END != ptj1939_rxpacket->updu.tpdu.b8PF) && (TP_CM_DATA != ptj1939_rxpacket->updu.tpdu.b8PF))
		return ierro;
	
	/*
		���յ��������������
	*/
	//ͬ�����ﲻ���Ϊʲô�ж���������
	if((TP_CM_START_OR_END == ptj1939_rxpacket->updu.tpdu.b8PF) && (TP_CM_RTS == ptj1939_rxpacket->au8data[0]))
	{
		//������ж�����ĵĸ�ֵ�ͽ���
	
	}
	else if((TP_CM_START_OR_END == ptj1939_rxpacket->updu.tpdu.b8PF) && (TP_CM_ABORT == ptj1939_rxpacket->au8data[0]))
	{
		//����������յ��˽�������
		printf("rx j1939 abort");
		
	}

}