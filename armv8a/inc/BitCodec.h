#ifndef BITCODEC_H
#define BITCODEC_H


#include <stdbool.h>
#include <stdint.h>


#define BIT_GET(VAR, INDEX) ((VAR) & ((uint64_t)0b1 << (INDEX)) ? 0b1 : 0b0)
#define BIT_SET(VAR, INDEX, BIT) ((BIT) ? ((VAR) |= (uint64_t)0b1 << (INDEX)) : ((VAR) &= ~((uint64_t)0b1 << (INDEX))))

#define REG16_GET(ADDR) (*(volatile uint16_t*)(uintptr_t)(ADDR))
#define REG16_SET(ADDR, VALUE) (*(volatile uint16_t*)(uintptr_t)(ADDR) = (uint16_t)(VALUE))
#define REG16_GET_BIT(ADDR, INDEX) (*(volatile uint16_t*)(uintptr_t)(ADDR) & ((uint16_t)0b1 << (INDEX)))
#define REG16_SET_BIT(ADDR, INDEX, VALUE) ((VALUE) ? (*(volatile uint16_t*)(uintptr_t)(ADDR) |= (uint16_t)0b1 << (INDEX)) : (*(volatile uint16_t*)(uintptr_t)(ADDR) &= ~((uint16_t)0b1 << (INDEX))))
#define REG16_DECODE(ADDR, MASK) (*(volatile uint16_t*)(uintptr_t)(ADDR) & (uint16_t)(MASK))
#define REG16_ENCODE(ADDR, MASK, VALUE) (*(volatile uint16_t*)(uintptr_t)(ADDR) = (*(volatile uint16_t*)(uintptr_t)(ADDR) & ~(uint16_t)(MASK)) | ((uint16_t)(VALUE) & (uint16)(MASK)))

#define REG32_GET(ADDR) (*(volatile uint32_t*)(uintptr_t)(ADDR))
#define REG32_SET(ADDR, VALUE) (*(volatile uint32_t*)(uintptr_t)(ADDR) = (uint32_t)(VALUE))
#define REG32_GET_BIT(ADDR, INDEX) (*(volatile uint32_t*)(uintptr_t)(ADDR) & ((uint32_t)0b1 << (INDEX)))
#define REG32_SET_BIT(ADDR, INDEX, VALUE) ((VALUE) ? (*(volatile uint32_t*)(uintptr_t)(ADDR) |= (uint32_t)0b1 << (INDEX)) : (*(volatile uint32_t*)(uintptr_t)(ADDR) &= ~((uint32_t)0b1 << (INDEX))))
#define REG32_DECODE(ADDR, MASK) (*(volatile uint32_t*)(uintptr_t)(ADDR) & (uint32_t)(MASK))
#define REG32_ENCODE(ADDR, MASK, VALUE) (*(volatile uint32_t*)(uintptr_t)(ADDR) = (*(volatile uint32_t*)(uintptr_t)(ADDR) & ~(uint32_t)(MASK)) | ((uint32_t)(VALUE) & (uint32_t)(MASK)))

#define REG64_GET(ADDR) (*(volatile uint64_t*)(uintptr_t)(ADDR))
#define REG64_SET(ADDR, VALUE) (*(volatile uint64_t*)(uintptr_t)(ADDR) = (uint64_t)(VALUE))
#define REG64_GET_BIT(ADDR, INDEX) (*(volatile uint64_t*)(uintptr_t)(ADDR) & ((uint64_t)0b1 << (INDEX)))
#define REG64_SET_BIT(ADDR, INDEX, VALUE) ((VALUE) ? (*(volatile uint64_t*)(uintptr_t)(ADDR) |= (uint64_t)0b1 << (INDEX)) : (*(volatile uint64_t*)(uintptr_t)(ADDR) &= ~((uint64_t)0b1 << (INDEX))))
#define REG64_DECODE(ADDR, MASK) (*(volatile uint64_t*)(uintptr_t)(ADDR) & (uint64_t)(MASK))
#define REG64_ENCODE(ADDR, MASK, VALUE) (*(volatile uint64_t*)(uintptr_t)(ADDR) = (*(volatile uint64_t*)(uintptr_t)(ADDR) & ~(uint64_t)(MASK)) | ((uint64_t)(VALUE) & (uint64_t)(MASK)))

static inline uint64_t BIT_GET_RANGE(uint64_t VAR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint64_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= (uint64_t)0b1 << i;
	
	return (VAR & mask) >> LOW;
}

#define BIT_SET_RANGE(VAR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint64_t mask; \
	\
	for(mask = 0, i = (LOW); i <= (HIGH); i ++) \
		mask |= (uint64_t)0b1 << i; \
	\
	(VAR) = ((VAR) & ~mask) | (((uint64_t)(VALUE) << (LOW)) & mask); \
	\
} while(false)

static inline uint16_t REG16_GET_RANGE(uintptr_t ADDR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint16_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= (uint16_t)0b1 << i;
	
	return REG16_DECODE(ADDR, mask) >> LOW;
}

#define REG16_SET_RANGE(ADDR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint16_t mask; \
	\
	for(mask = 0, i = (LOW); i <= (HIGH); i ++) \
		mask |= (uint16_t)0b1 << i; \
	\
	REG16_ENCODE(ADDR, mask, (uint16_t)(VALUE) << (LOW)); \
	\
} while(false)

static inline uint32_t REG32_GET_RANGE(uintptr_t ADDR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint32_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= (uint32_t)0b1 << i;
	
	return REG32_DECODE(ADDR, mask) >> LOW;
}

#define REG32_SET_RANGE(ADDR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint32_t mask; \
	\
	for(mask = 0, i = (LOW); i <= (HIGH); i ++) \
		mask |= (uint32_t)0b1 << i; \
	\
	REG32_ENCODE(ADDR, mask, (uint32_t)(VALUE) << (LOW)); \
	\
} while(false)

static inline uint64_t REG64_GET_RANGE(uintptr_t ADDR, unsigned HIGH, unsigned LOW)
{
	unsigned i;
	uint64_t mask;
	
	for(mask = 0, i = LOW; i <= HIGH; i ++)
		mask |= (uint64_t)0b1 << i;
	
	return REG64_DECODE(ADDR, mask) >> LOW;
}

#define REG64_SET_RANGE(ADDR, HIGH, LOW, VALUE) do \
{ \
	unsigned i; \
	uint64_t mask; \
	\
	for(mask = 0, i = (LOW); i <= (HIGH); i ++) \
		mask |= (uint64_t)0b1 << i; \
	\
	REG64_ENCODE(ADDR, mask, (uint64_t)(VALUE) << (LOW)); \
	\
} while(false)


#endif
