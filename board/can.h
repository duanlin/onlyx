#ifndef CAN_H
#define CAN_H


#include <stdbool.h>
#include <stdint.h>

#include "trap.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	Can0 = 0,
	Can1,
	
	CanCount
	
} Can;

typedef struct
{
	
} CanConfig;


typedef int (*CanRxHandler)(unsigned canid, const uint8_t* frame, unsigned length, void* arg);
typedef int (*CanTxHandler)(void* arg);
typedef int (*CanEwmcHandler)(void* arg);


int initCan(void);

bool canTx0Ready(Can can);
bool canTx1Ready(Can can);
bool canTx2Ready(Can can);

int canTx0Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length);
int canTx1Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length);
int canTx2Frame(Can can, unsigned canid, const uint8_t* frame, unsigned length);

int registerCanInterrupt(Can can, CanRxHandler rxHandler, CanTxHandler txHandler, CanEwmcHandler ewmcHandler, void* arg, uint8_t rxPriority, uint8_t txPriority, uint8_t ewmcPriority);
int disableCanInterrupt(Can can);


#ifdef __cplusplus
}
#endif


#endif
