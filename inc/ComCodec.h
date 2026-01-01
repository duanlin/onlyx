#ifndef COMCODEC_H
#define COMCODEC_H


#include <stdint.h>


#define DECODE_BYTE(p) (*((const uint8_t*)(p)) & 0xFF)
#define ENCODE_BYTE(p, b) (*((uint8_t*)(p)) = (b) & 0xFF)

#define DECODE_BE_WORD(p) (((uint16_t)*((const uint8_t*)(p) + 1) & 0x00FF) | (((uint16_t)*((const uint8_t*)(p)) << 8) & 0xFF00))
#define DECODE_BE_TBYTE(p) (((uint32_t)*((const uint8_t*)(p) + 2) & 0x0000FF) | (((uint32_t)*((const uint8_t*)(p) + 1) << 8) & 0x00FF00) | (((uint32_t)*((const uint8_t*)(p)) << 16) & 0xFF0000))
#define DECODE_BE_DWORD(p) (((uint32_t)*((const uint8_t*)(p) + 3) & 0x000000FF) | (((uint32_t)*((const uint8_t*)(p) + 2) << 8) & 0x0000FF00) | (((uint32_t)*((const uint8_t*)(p) + 1) << 16) & 0x00FF0000) | (((uint32_t)*((const uint8_t*)(p)) << 24) & 0xFF000000))
#define DECODE_BE_LWORD(p) (((uint64_t)*((const uint8_t*)(p) + 7) & 0x00000000000000FF) | (((uint64_t)*((const uint8_t*)(p) + 6) << 8) & 0x000000000000FF00) | (((uint64_t)*((const uint8_t*)(p) + 5) << 16) & 0x0000000000FF0000) | (((uint64_t)*((const uint8_t*)(p) + 4) << 24) & 0x00000000FF000000) | (((uint64_t)*((const uint8_t*)(p) + 3) << 32) & 0x000000FF00000000) | (((uint64_t)*((const uint8_t*)(p) + 2) << 40) & 0x0000FF0000000000) | (((uint64_t)*((const uint8_t*)(p) + 1) << 48) & 0x00FF000000000000) | (((uint64_t)*((const uint8_t*)(p)) << 56) & 0xFF00000000000000))

#define DECODE_BE_DATA(p, s) (((s) == 1) ? DECODE_BYTE(p) : (((s) == 2) ? DECODE_BE_WORD(p) : (((s) == 3) ? DECODE_BE_TBYTE(p) : (((s) == 4) ? DECODE_BE_DWORD(p) : (((s) == 5) ? (((uint64_t)*((const uint8_t*)(p) + 4) & 0x00000000000000FF) | (((uint64_t)*((const uint8_t*)(p) + 3) << 8) & 0x000000000000FF00) | (((uint64_t)*((const uint8_t*)(p) + 2) << 16) & 0x0000000000FF0000) | (((uint64_t)*((const uint8_t*)(p) + 1) << 24) & 0x00000000FF000000) | (((uint64_t)*((const uint8_t*)(p)) << 32) & 0x000000FF00000000)) : (((s) == 6) ? (((uint64_t)*((const uint8_t*)(p) + 5) & 0x00000000000000FF) | (((uint64_t)*((const uint8_t*)(p) + 4) << 8) & 0x000000000000FF00) | (((uint64_t)*((const uint8_t*)(p) + 3) << 16) & 0x0000000000FF0000) | (((uint64_t)*((const uint8_t*)(p) + 2) << 24) & 0x00000000FF000000) | (((uint64_t)*((const uint8_t*)(p) + 1) << 32) & 0x000000FF00000000) | (((uint64_t)*((const uint8_t*)(p)) << 40) & 0x0000FF0000000000)) : (((s) == 7) ? (((uint64_t)*((const uint8_t*)(p) + 6) & 0x00000000000000FF) | (((uint64_t)*((const uint8_t*)(p) + 5) << 8) & 0x000000000000FF00) | (((uint64_t)*((const uint8_t*)(p) + 4) << 16) & 0x0000000000FF0000) | (((uint64_t)*((const uint8_t*)(p) + 3) << 24) & 0x00000000FF000000) | (((uint64_t)*((const uint8_t*)(p) + 2) << 32) & 0x000000FF00000000) | (((uint64_t)*((const uint8_t*)(p) + 1) << 40) & 0x0000FF0000000000) | (((uint64_t)*((const uint8_t*)(p)) << 48) & 0x00FF000000000000)) : (((s) == 8) ? DECODE_BE_LWORD(p) : 0))))))))


#define ENCODE_BE_WORD(p, w) (*((uint8_t*)(p)) = (uint8_t)(((uint16_t)(w) >> 8) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)((uint16_t)(w) & 0xFF))
#define ENCODE_BE_TBYTE(p, t) (*((uint8_t*)(p)) = (uint8_t)(((uint32_t)(t) >> 16) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint32_t)(t) >> 8) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)((uint32_t)(t) & 0xFF))
#define ENCODE_BE_DWORD(p, d) (*((uint8_t*)(p)) = (uint8_t)(((uint32_t)(d) >> 24) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint32_t)(d) >> 16) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)(((uint32_t)(d) >> 8) & 0xFF), *((uint8_t*)(p) + 3) = (uint8_t)((uint32_t)(d) & 0xFF))
#define ENCODE_BE_LWORD(p, l) (*((uint8_t*)(p)) = (uint8_t)(((uint64_t)(l) >> 56) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint64_t)(l) >> 48) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)(((uint64_t)(l) >> 40) & 0xFF), *((uint8_t*)(p) + 3) = (uint8_t)(((uint64_t)(l) >> 32) & 0xFF), *((uint8_t*)(p) + 4) = (uint8_t)(((uint64_t)(l) >> 24) & 0xFF), *((uint8_t*)(p) + 5) = (uint8_t)(((uint64_t)(l) >> 16) & 0xFF), *((uint8_t*)(p) + 6) = (uint8_t)(((uint64_t)(l) >> 8) & 0xFF), *((uint8_t*)(p) + 7) = (uint8_t)((uint64_t)(l) & 0xFF))

#define ENCODE_BE_DATA(p, x, s) (((s) == 1) ? ENCODE_BYTE(p, x) : (((s) == 2) ? ENCODE_BE_WORD(p, x) : (((s) == 3) ? ENCODE_BE_TBYTE(p, x) : (((s) == 4) ? ENCODE_BE_DWORD(p, x) : (((s) == 5) ? (*(uint8_t*)(p) = ((uint64_t)(x) >> 32) & 0xFF, *((uint8_t*)(p) + 1) = ((uint64_t)(x) >> 24) & 0xFF, *((uint8_t*)(p) + 2) = ((uint64_t)(x) >> 16) & 0xFF, *((uint8_t*)(p) + 3) = ((uint64_t)(x) >> 8) & 0xFF, *((uint8_t*)(p) + 4) = (uint64_t)(x) & 0xFF) : (((s) == 6) ? (*(uint8_t*)(p) = ((uint64_t)(x) >> 40) & 0xFF, *((uint8_t*)(p) + 1) = ((uint64_t)(x) >> 32) & 0xFF, *((uint8_t*)(p) + 2) = ((uint64_t)(x) >> 24) & 0xFF, *((uint8_t*)(p) + 3) = ((uint64_t)(x) >> 16) & 0xFF, *((uint8_t*)(p) + 4) = ((uint64_t)(x) >> 8) & 0xFF, *((uint8_t*)(p) + 5) = (uint64_t)(x) & 0xFF) : (((s) == 7) ? (*(uint8_t*)(p) = ((uint64_t)(x) >> 48) & 0xFF, *((uint8_t*)(p) + 1) = ((uint64_t)(x) >> 40) & 0xFF, *((uint8_t*)(p) + 2) = ((uint64_t)(x) >> 32) & 0xFF, *((uint8_t*)(p) + 3) = ((uint64_t)(x) >> 24) & 0xFF, *((uint8_t*)(p) + 4) = ((uint64_t)(x) >> 16) & 0xFF, *((uint8_t*)(p) + 5) = ((uint64_t)(x) >> 8) & 0xFF, *((uint8_t*)(p) + 6) = (uint64_t)(x) & 0xFF) : (((s) == 8) ? ENCODE_BE_LWORD(p, x) : (void)(x)))))))))


#define DECODE_LE_WORD(p) (((uint16_t)*((const uint8_t*)(p)) & 0x00FF) | (((uint16_t)*((const uint8_t*)(p) + 1) << 8) & 0xFF00))
#define DECODE_LE_TBYTE(p) (((uint32_t)*((const uint8_t*)(p)) & 0x0000FF) | (((uint32_t)*((const uint8_t*)(p) + 1) << 8) & 0x00FF00) | (((uint32_t)*((const uint8_t*)(p) + 2) << 16) & 0xFF0000))
#define DECODE_LE_DWORD(p) (((uint32_t)*((const uint8_t*)(p)) & 0x000000FF) | (((uint32_t)*((const uint8_t*)(p) + 1) << 8) & 0x0000FF00) | (((uint32_t)*((const uint8_t*)(p) + 2) << 16) & 0x00FF0000) | (((uint32_t)*((const uint8_t*)(p) + 3) << 24) & 0xFF000000))
#define DECODE_LE_LWORD(p) (((uint64_t)*((const uint8_t*)(p)) & 0x00000000000000FF) | (((uint64_t)*((const uint8_t*)(p) + 1) << 8) & 0x000000000000FF00) | (((uint64_t)*((const uint8_t*)(p) + 2) << 16) & 0x0000000000FF0000) | (((uint64_t)*((const uint8_t*)(p) + 3) << 24) & 0x00000000FF000000) | (((uint64_t)*((const uint8_t*)(p) + 4) << 32) & 0x000000FF00000000) | (((uint64_t)*((const uint8_t*)(p) + 5) << 40) & 0x0000FF0000000000) | (((uint64_t)*((const uint8_t*)(p) + 6) << 48) & 0x00FF000000000000) | (((uint64_t)*((const uint8_t*)(p) + 7) << 56) & 0xFF00000000000000))

#define ENCODE_LE_WORD(p, w) (*((uint8_t*)(p)) = (uint8_t)((uint16_t)(w) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define ENCODE_LE_TBYTE(p, t) (*((uint8_t*)(p)) = (uint8_t)((uint32_t)(t) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint32_t)(t) >> 8) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)(((uint32_t)(t) >> 16) & 0xFF))
#define ENCODE_LE_DWORD(p, d) (*((uint8_t*)(p)) = (uint8_t)((uint32_t)(d) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint32_t)(d) >> 8) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)(((uint32_t)(d) >> 16) & 0xFF), *((uint8_t*)(p) + 3) = (uint8_t)(((uint32_t)(d) >> 24) & 0xFF))
#define ENCODE_LE_LWORD(p, l) (*((uint8_t*)(p)) = (uint8_t)((uint64_t)(l) & 0xFF), *((uint8_t*)(p) + 1) = (uint8_t)(((uint64_t)(l) >> 8) & 0xFF), *((uint8_t*)(p) + 2) = (uint8_t)(((uint64_t)(l) >> 16) & 0xFF), *((uint8_t*)(p) + 3) = (uint8_t)(((uint64_t)(l) >> 24) & 0xFF), *((uint8_t*)(p) + 4) = (uint8_t)(((uint64_t)(l) >> 32) & 0xFF), *((uint8_t*)(p) + 5) = (uint8_t)(((uint64_t)(l) >> 40) & 0xFF), *((uint8_t*)(p) + 6) = (uint8_t)(((uint64_t)(l) >> 48) & 0xFF), *((uint8_t*)(p) + 7) = (uint8_t)(((uint64_t)(l) >> 56) & 0xFF))


#define DECODE_WORD DECODE_BE_WORD
#define DECODE_TBYTE DECODE_BE_TBYTE
#define DECODE_DWORD DECODE_BE_DWORD
#define DECODE_LWORD DECODE_BE_LWORD

#define DECODE_DATA DECODE_BE_DATA

#define ENCODE_WORD ENCODE_BE_WORD
#define ENCODE_TBYTE ENCODE_BE_TBYTE
#define ENCODE_DWORD ENCODE_BE_DWORD
#define ENCODE_LWORD ENCODE_BE_LWORD

#define ENCODE_DATA ENCODE_BE_DATA


typedef union
{
	uint32_t m;
	
	int32_t s;
	uint32_t u;
	float f;
	
} Union32;

typedef union
{
	uint64_t m;

	int64_t s;
	uint64_t u;
	double d;
	
} Union64;


#endif
