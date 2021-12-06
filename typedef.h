#pragma once

typedef unsigned __int64 UINT64;
typedef __int64 SINT64;

typedef unsigned int UINT32;
typedef int SINT32;

typedef unsigned short UINT16;
typedef short SINT16;

typedef unsigned char UINT8;
typedef char SINT8;

typedef struct {
	SINT8 sign;		//1:negative, 0:positive, zero
	UINT32 dat[256];	//data (max 8192 bits)
	SINT16 len;		//word length
}MPZ;

typedef struct {
	MPZ e;
	MPZ n;
}RSA_Public_Key;

typedef struct {
	MPZ n;
	MPZ e;
	MPZ d;
	MPZ p;
	MPZ q;
	MPZ dmodpm1;
	MPZ dmodqm1;
	MPZ qinvp;
}RSA_Private_Key;


