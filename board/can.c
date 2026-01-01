#include <stddef.h>
#include <string.h>

#include "can.h"

#include "GD32E503.h"
#include "BitCodec.h"


typedef struct
{
	CanRxHandler rxHandler;
	CanTxHandler txHandler;
	CanEwmcHandler ewmcHandler;
	void* arg;
	
} CanInterruptInfo;


static CanInterruptInfo sCanInterruptInfo[CanCount];


int initCan(void)
{
	memset(&sCanInterruptInfo, 0, sizeof(sCanInterruptInfo));
	
	// CAN0
	// PA11 CAN0_RX
	// PA12 CAN0_TX
	//
	// GD32E503_GPIO_CTL
	// CTL11 [15:14] 0b01 Float
	// MD11 [13:12] 0b00 Input
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL1), 15, 12, 0b0100);
	// GD32E503_GPIO_CTL
	// CTL12 [19:18] 0b10 AFIO Push-Pull
	// MD12 [17:16] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL1), 19, 16, 0b1011);
	
	// CAN1
	// PB12 CAN1_RX
	// PB13 CAN1_TX
	//
	// GD32E503_GPIO_CTL
	// CTL12 [19:18] 0b01 Float
	// MD12 [17:16] 0b00 Input
	REG32_SET_RANGE(GD32E503_GPIOB(GD32E503_GPIO_CTL1), 19, 16, 0b0100);
	// GD32E503_GPIO_CTL
	// CTL13 [23:22] 0b10 AFIO Push-Pull
	// MD13 [21:20] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOB(GD32E503_GPIO_CTL1), 23, 20, 0b1011);
	
	// GD32E503_CAN_CTL
	// IWMOD [0] 0b1
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_CTL), 0b1);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_CTL), 0b1);
	
	// GD32E503_CAN_INTEN
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_INTEN), 0);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_INTEN), 0);
	
	// GD32E503_CAN_CTL
	// ABOR [6] 0b1
	// AWU [5] 0b1
	REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_CTL), 6, 5, 0b11);
	REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_CTL), 6, 5, 0b11);
	
	// GD32E503_CAN_BT
	// APB1 50MHz / PSC 5 = 10MHz
	// Baud 1Mbps 1bit = 10 clock cycles
	// SYN_SEG=1 + BS1=6 + BS2=3
	//
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_BT), 0);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_BT), 0);
	// BAUDPSC [9:0] 9
	// APB1 50MHz / (BAUDPSC + 1) 5 = 10MHz
	REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_BT), 9, 0, 4);
	REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_BT), 9, 0, 4);
	// BS1 [19:16] (BS1 + 1) 6
	REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_BT), 19, 16, 5);
	REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_BT), 19, 16, 5);
	// BS2 [22:20] (BS2 + 1) 3
	REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_BT), 22, 20, 2);
	REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_BT), 22, 20, 2);
	
	// GD32E503_CAN_FCTL
	// FLD [0] 0b1 Unlock
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_FCTL), 0b1);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_FCTL), 0b1);
	// HBC1F [13:8] 14
	REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_FCTL), 13, 8, 14);
	REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_FCTL), 13, 8, 14);
	
	// GD32E503_CAN_FW
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_FW), 0);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_FW), 0);
	
	// GD32E503_CAN_FxDATAy
	// Any data frame
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_F0DATA0), 0);
	REG32_SET(GD32E503_CAN0(GD32E503_CAN_F0DATA1), 0b10);
	// GD32E503_CAN_FMCFG
	// FMOD0 [0] 0b0 Mask mode
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_FMCFG), 0, 0b0);
	// GD32E503_CAN_FSCFG
	// FS0 [0] 0b1 32-bit
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_FSCFG), 0, 0b1);
	// GD32E503_CAN_FAFIFO
	// FAF0 [0] 0b0 FIFO0
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_FAFIFO), 0, 0b0);
	// GD32E503_CAN_FW
	// FW0 [0] 0b1 Enable
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_FW), 0, 0b1);
	
	// GD32E503_CAN_FxDATAy
	// Any data frame
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_F0DATA0) + 8 * 14, 0);
	REG32_SET(GD32E503_CAN1(GD32E503_CAN_F0DATA1) + 8 * 14, 0b10);
	// GD32E503_CAN_FMCFG
	// FMOD14 [14] 0b0 Mask mode
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_FMCFG), 14, 0b0);
	// GD32E503_CAN_FSCFG
	// FS14 [14] 0b1 32-bit
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_FSCFG), 14, 0b1);
	// GD32E503_CAN_FAFIFO
	// FAF14 [14] 0b0 FIFO0
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_FAFIFO), 14, 0b0);
	// GD32E503_CAN_FW
	// FW14 [14] 0b1 Enable
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_FW), 14, 0b1);
	
	// GD32E503_CAN_FCTL
	// FLD [0] 0b1 Lock
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_FCTL), 0, 0b0);
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_FCTL), 0, 0b0);
	
	// GD32E503_CAN_CTL
	// IWMOD [0] 0b0
	REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_CTL), 0, 0b0);
	REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_CTL), 0, 0b0);
	
	return 0;
}

static int canRxInterruptHandler(void* arg)
{
	unsigned i;
	
	Can can;
	unsigned count;
	
	unsigned canid;
	uint8_t frame[8];
	unsigned length;
	
	can = (Can)arg;
	
	switch(can)
	{
	case Can0:
		count = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFO0), 1, 0);
		
		for(i = 0; i < count; i ++)
		{
			// GD32E503_CAN_RFIFOMI
			// FT [1] 0b0 Data frame
			if(!REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_RFIFOMI0), 1))
			{
				// GD32E503_CAN_RFIFOMI
				// FF [2] 0b1 Extended frame
				if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_RFIFOMI0), 2))
					canid = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMI0), 31, 3);
				// FF [2] 0b0 Standard frame
				else
					canid = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMI0), 31, 21);
				
				frame[0] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA0), 7, 0);
				frame[1] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA0), 15, 8);
				frame[2] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA0), 23, 16);
				frame[3] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA0), 31, 24);
				frame[4] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA1), 7, 0);
				frame[5] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA1), 15, 8);
				frame[6] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA1), 23, 16);
				frame[7] = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMDATA1), 31, 24);
				
				length = REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFOMP0), 3, 0);
				
				if(sCanInterruptInfo[can].rxHandler)
					sCanInterruptInfo[can].rxHandler(canid, frame, length, sCanInterruptInfo[can].arg);
			}
			
			// GD32E503_CAN_RFIFO0
			// RFD0 [5]
			REG32_SET(GD32E503_CAN0(GD32E503_CAN_RFIFO0), (uint32_t)0b1 << 5);
		}
		
		break;
		
	case Can1:
		count = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFO0), 1, 0);
		
		for(i = 0; i < count; i ++)
		{
			// GD32E503_CAN_RFIFOMI
			// FT [1] 0b0 Data frame
			if(!REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_RFIFOMI0), 1))
			{
				// GD32E503_CAN_RFIFOMI
				// FF [2] 0b1 Extended frame
				if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_RFIFOMI0), 2))
					canid = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMI0), 31, 3);
				// FF [2] 0b0 Standard frame
				else
					canid = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMI0), 31, 21);
				
				frame[0] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA0), 7, 0);
				frame[1] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA0), 15, 8);
				frame[2] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA0), 23, 16);
				frame[3] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA0), 31, 24);
				frame[4] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA1), 7, 0);
				frame[5] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA1), 15, 8);
				frame[6] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA1), 23, 16);
				frame[7] = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMDATA1), 31, 24);
				
				length = REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFOMP0), 3, 0);
				
				if(sCanInterruptInfo[can].rxHandler)
					sCanInterruptInfo[can].rxHandler(canid, frame, length, sCanInterruptInfo[can].arg);
			}
			
			// GD32E503_CAN_RFIFO0
			// RFD0 [5]
			REG32_SET(GD32E503_CAN1(GD32E503_CAN_RFIFO0), (uint32_t)0b1 << 5);
		}
		
		break;
		
	default:
		return -1;
	}
	
	return 0;
}

static int canTxInterruptHandler(void* arg)
{
	Can can;
	
	can = (Can)arg;
	
	if(sCanInterruptInfo[can].txHandler)
		sCanInterruptInfo[can].txHandler(sCanInterruptInfo[can].arg);
	
	// GD32E503_CAN_TSTAT
	// MTF2 [16] 0b1 Clear
	// MTF1 [8] 0b1 Clear
	// MTF0 [0] 0b1 Clear
	switch(can)
	{
	case Can0:
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 16)) REG32_SET(GD32E503_CAN0(GD32E503_CAN_TSTAT), (uint32_t)0b1 << 16);
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 8)) REG32_SET(GD32E503_CAN0(GD32E503_CAN_TSTAT), (uint32_t)0b1 << 8);
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 0)) REG32_SET(GD32E503_CAN0(GD32E503_CAN_TSTAT), (uint32_t)0b1);
		
		break;
		
	case Can1:
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 16)) REG32_SET(GD32E503_CAN1(GD32E503_CAN_TSTAT), (uint32_t)0b1 << 16);
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 8)) REG32_SET(GD32E503_CAN1(GD32E503_CAN_TSTAT), (uint32_t)0b1 << 8);
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 0)) REG32_SET(GD32E503_CAN1(GD32E503_CAN_TSTAT), (uint32_t)0b1);
		
		break;
		
	default:
		return -1;
	}
	
	return 0;
}

static int canEwmcInterruptHandler(void* arg)
{
	Can can;
	
	can = (Can)arg;
	
	if(sCanInterruptInfo[can].ewmcHandler)
		sCanInterruptInfo[can].ewmcHandler(sCanInterruptInfo[can].arg);
	
	// GD32E503_CAN_STAT
	// SLPIF [4]
	// WUIF [3]
	// ERRIF [2]
	switch(can)
	{
	case Can0: REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_STAT), 4, 2, 0b111); break;
	case Can1: REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_STAT), 4, 2, 0b111); break;
	
	default:
		return -1;
	}
	
	return 0;
}

bool canTx0Ready(Can can)
{
	// TME0 [26]
	switch(can)
	{
	case Can0:
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 26))
			return true;
		
		break;
		
	case Can1:
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 26))
			return true;
		
		break;
		
	default:
		break;
	}
	
	return false;
}

bool canTx1Ready(Can can)
{
	// TME1 [27]
	switch(can)
	{
	case Can0:
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 27))
			return true;
		
		break;
		
	case Can1:
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 27))
			return true;
		
		break;
		
	default:
		break;
	}
	
	return false;
}

bool canTx2Ready(Can can)
{
	// TME2 [28]
	switch(can)
	{
	case Can0:
		if(REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 28))
			return true;
		
		break;
		
	case Can1:
		if(REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 28))
			return true;
		
		break;
		
	default:
		break;
	}
	
	return false;
}

int canTx0Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length)
{
	if(length > 8)
		return -1;
	
	// Mailbox 0
	switch(can)
	{
	case Can0:
		// TME0 [26]
		if(!REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 26))
			return -2;
		
		// GD32E503_CAN_TMI
		if(canid > 0x7FF)
		{
			// EFID [31:3]
			REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0), 31, 3, canid);
			// FF [2] 0b1 Extended
			// FT [1] 0b0 Data frame
			REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0), 2, 1, 0b10);
		}
		else
		{
			// SFID [31:21]
			REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0), 31, 21, canid);
			// FF [2] 0b0 Standard
			// FT [1] 0b0 Data frame
			REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0), 2, 1, 0b00);
		}
		
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0), 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0), 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0), 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0), 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1), 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1), 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1), 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1), 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMP0), 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_TMI0), 0, 0b1);
		
		break;
		
	case Can1:
		// TME0 [26]
		if(!REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 26))
			return -3;
		
		// GD32E503_CAN_TMI
		if(canid > 0x7FF)
		{
			// EFID [31:3]
			REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0), 31, 3, canid);
			// FF [2] 0b1 Extended
			// FT [1] 0b0 Data frame
			REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0), 2, 1, 0b10);
		}
		else
		{
			// SFID [31:21]
			REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0), 31, 21, canid);
			// FF [2] 0b0 Standard
			// FT [1] 0b0 Data frame
			REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0), 2, 1, 0b00);
		}
		
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0), 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0), 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0), 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0), 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1), 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1), 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1), 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1), 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMP0), 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_TMI0), 0, 0b1);
		
		break;
		
	default:
		return -4;
	}
	
	return 0;
}

int canTx1Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length)
{
	if(length > 8)
		return -1;
	
	// Mailbox 1
	switch(can)
	{
	case Can0:
		// TME1 [27]
		if(!REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 27))
			return -2;
		
		// GD32E503_CAN_TMI
		// EFID [31:3]
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x10, 31, 3, canid);
		// FF [2] 0b1 Extended
		// FT [1] 0b0 Data frame
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x10, 2, 1, 0b10);
		
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x10, 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x10, 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x10, 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x10, 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x10, 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x10, 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x10, 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x10, 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMP0) + 0x10, 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x10, 0, 0b1);
		
		break;
		
	case Can1:
		// TME1 [27]
		if(!REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 27))
			return -3;
		
		// GD32E503_CAN_TMI
		// EFID [31:3]
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x10, 31, 3, canid);
		// FF [2] 0b1 Extended
		// FT [1] 0b0 Data frame
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x10, 2, 1, 0b10);
		
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x10, 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x10, 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x10, 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x10, 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x10, 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x10, 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x10, 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x10, 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMP0) + 0x10, 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x10, 0, 0b1);
		
		break;
		
	default:
		return -4;
	}
	
	return 0;
}

int canTx2Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length)
{
	if(length > 8)
		return -1;
	
	// Mailbox 2
	switch(can)
	{
	case Can0:
		// TME2 [28]
		if(!REG32_GET_BIT(GD32E503_CAN0(GD32E503_CAN_TSTAT), 28))
			return -2;
		
		// GD32E503_CAN_TMI
		// EFID [31:3]
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x20, 31, 3, canid);
		// FF [2] 0b1 Extended
		// FT [1] 0b0 Data frame
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x20, 2, 1, 0b10);
		
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x20, 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x20, 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x20, 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA0) + 0x20, 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x20, 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x20, 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x20, 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMDATA1) + 0x20, 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_TMP0) + 0x20, 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_TMI0) + 0x20, 0, 0b1);
		
		break;
		
	case Can1:
		// TME2 [28]
		if(!REG32_GET_BIT(GD32E503_CAN1(GD32E503_CAN_TSTAT), 28))
			return -3;
		
		// GD32E503_CAN_TMI
		// EFID [31:3]
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x20, 31, 3, canid);
		// FF [2] 0b1 Extended
		// FT [1] 0b0 Data frame
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x20, 2, 1, 0b10);
		
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x20, 7, 0, frame[0]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x20, 15, 8, frame[1]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x20, 23, 16, frame[2]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA0) + 0x20, 31, 24, frame[3]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x20, 7, 0, frame[4]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x20, 15, 8, frame[5]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x20, 23, 16, frame[6]);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMDATA1) + 0x20, 31, 24, frame[7]);
		
		// GD32E503_CAN_TMP
		// DLENC [3:0] length
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_TMP0) + 0x20, 3, 0, length);
		
		// GD32E503_CAN_TMI
		// TEN [0] 0b1
		REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_TMI0) + 0x20, 0, 0b1);
		
		break;
		
	default:
		return -4;
	}
	
	return 0;
}

int registerCanInterrupt(Can can, CanRxHandler rxHandler, CanTxHandler txHandler, CanEwmcHandler ewmcHandler, void* arg, uint8_t rxPriority, uint8_t txPriority, uint8_t ewmcPriority)
{
	// GD32E503_CAN_INTEN
	switch(can)
	{
	case Can0: REG32_SET(GD32E503_CAN0(GD32E503_CAN_INTEN), 0); break;
	case Can1: REG32_SET(GD32E503_CAN1(GD32E503_CAN_INTEN), 0); break;
	
	default:
		return -1;
	}
	
	// GD32E503_CAN_RFIFO0
	// RFD0 [5] 0b1 Clear
	// RFO0 [4] 0b1 Clear
	// RFF0 [3] 0b1 Clear
	switch(can)
	{
	case Can0:
		while(REG32_GET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFO0), 1, 0))
			REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_RFIFO0), 5, 0b1);
		REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_RFIFO0), 4, 3, 0b11);
		
		break;
		
	case Can1:
		while(REG32_GET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFO0), 1, 0))
			REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_RFIFO0), 5, 0b1);
		REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_RFIFO0), 4, 3, 0b11);
		
		break;
		
	default:
		return -2;
	}
	
	// GD32E503_CAN_STAT
	// SLPIF [4] 0b1 Clear
	// WUIF [3] 0b1 Clear
	// ERRIF [2] 0b1 Clear
	switch(can)
	{
	case Can0: REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_STAT), 3, 2, 0b111); break;
	case Can1: REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_STAT), 3, 2, 0b111); break;
	
	default:
		return -2;
	}
	
	// Register
	switch(can)
	{
	case Can0:
		if(registerInterrupt(IrqRx0Can0, canRxInterruptHandler, (void*)can, rxPriority))
			return -3;
		if(registerInterrupt(IrqTxCan0, canTxInterruptHandler, (void*)can, txPriority))
			return -4;
		if(registerInterrupt(IrqEwmcCan0, canEwmcInterruptHandler, (void*)can, ewmcPriority))
			return -5;
		
		break;
		
	case Can1:
		if(registerInterrupt(IrqRx0Can1, canRxInterruptHandler, (void*)can, rxPriority))
			return -6;
		if(registerInterrupt(IrqTxCan1, canTxInterruptHandler, (void*)can, txPriority))
			return -7;
		if(registerInterrupt(IrqEwmcCan1, canEwmcInterruptHandler, (void*)can, ewmcPriority))
			return -8;
		
		break;
		
	default:
		return -9;
	}
	
	sCanInterruptInfo[can].arg = arg;
	sCanInterruptInfo[can].rxHandler = rxHandler;
	sCanInterruptInfo[can].txHandler = txHandler;
	sCanInterruptInfo[can].ewmcHandler = ewmcHandler;
	
	// GD32E503_CAN_INTEN
	// ERRIE [15] 0b1 Enable
	// ERRNIE [11] 0b1 Enable
	// BOIE [10] 0b1 Enable
	// PERRIE [9] 0b1 Enable
	// WERRIE [8] 0b1 Enable
	// RFNEIE [1] 0b1 Enable
	// TMEIE [0] 0b1 Enable
	switch(can)
	{
	case Can0: REG32_SET_BIT(GD32E503_CAN0(GD32E503_CAN_INTEN), 15, 0b1); REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_INTEN), 11, 8, 0b1111); REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_INTEN), 1, 0, 0b11); break;
	case Can1: REG32_SET_BIT(GD32E503_CAN1(GD32E503_CAN_INTEN), 15, 0b1); REG32_SET_RANGE(GD32E503_CAN0(GD32E503_CAN_INTEN), 11, 8, 0b1111); REG32_SET_RANGE(GD32E503_CAN1(GD32E503_CAN_INTEN), 1, 0, 0b11); break;
	
	default:
		return -10;
	}
	
	return 0;
}

int disableCanInterrupt(Can can)
{
	// GD32E503_CAN_INTEN
	switch(can)
	{
	case Can0: REG32_SET(GD32E503_CAN0(GD32E503_CAN_INTEN), 0); break;
	case Can1: REG32_SET(GD32E503_CAN1(GD32E503_CAN_INTEN), 0); break;
	
	default:
		return -1;
	}
	
	sCanInterruptInfo[can].rxHandler = NULL;
	sCanInterruptInfo[can].txHandler = NULL;
	sCanInterruptInfo[can].ewmcHandler = NULL;
	sCanInterruptInfo[can].arg = NULL;
	
	switch(can)
	{
	case Can0:
		if(disableInterrupt(IrqRx0Can0))
			return -2;
		if(disableInterrupt(IrqTxCan0))
			return -3;
		if(disableInterrupt(IrqEwmcCan0))
			return -4;
		
		break;
		
	case Can1:
		if(disableInterrupt(IrqRx0Can1))
			return -5;
		if(disableInterrupt(IrqTxCan1))
			return -6;
		if(disableInterrupt(IrqEwmcCan1))
			return -7;
		
		break;
		
	default:
		return -8;
	}
	
	return 0;
}
