#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "typedef.h"
#include <stdint.h>
#include <stdlib.h>


static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
								'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
								'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
								'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
								'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
								'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
								'w', 'x', 'y', 'z', '0', '1', '2', '3',
								'4', '5', '6', '7', '8', '9', '+', '/' };
static char* decoding_table = NULL;
static int mod_table[] = { 0, 2, 1 };

void build_decoding_table() {

	decoding_table = malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char)encoding_table[i]] = i;
}

void base64_encode(UINT8 *encoded_data, UINT8* data, SINT32 input_length, SINT32* output_length) {

	*output_length = 4 * ((input_length + 2) / 3);

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';
}


void base64_decode(UINT8* decoded_data, UINT8* data, SINT32 input_length, SINT32* output_length) {

	if (decoding_table == NULL) build_decoding_table();

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}
}



void LOAD_MPZ(MPZ* a, SINT8* hexdata)
{
	SINT32 i, j, k, pos, bytes;
	SINT8 x;
		
	a->sign = 0;
	bytes = strlen(hexdata) / 2;
	a->len = (bytes + 3) / 4;
	pos = bytes * 2 - 1;

	for (j = 0; j < a->len; j++)
	{
		a->dat[j] = 0;
		for (k = 0; k < 8; k++)
		{
			x = hexdata[pos--];
			if (x >= 'A' && x <= 'F')x = x - 'A' + 10;
			else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
			else if (x >= '0' && x <= '9')x = x - '0';
			a->dat[j] = (x << 28) ^ (a->dat[j] >> 4);
			if (pos == -1)break;
		}
	}
	for (; k < 7; k++)
		a->dat[a->len-1] >>= 4;
	for (k = a->len - 1; k >= 0; k--)
	{
		if (a->dat[k] != 0)
			break;
	}
	a->len = k + 1;
}

void RSA_Key_Gen(RSA_Private_Key* SK, RSA_Public_Key* PK)
{
	MPZ phin, one, pm1, qm1, temp;
	// p, q ����
	LOAD_MPZ(&(SK->p), "977a34505f61feaabdfbe303802d70533a96a5c931e2a30a2eae72adb61bd57307462935db605c422f2e03435671b24a8edf2da7baa0b676d29a6c1467472a89912c616f9ca20596d93246362aa8e8c28ebb7b84834efd42c58205766d11066707090911c64806c0a75217a0195459e63c35a5301ae35454ee121533ec3f90ff");
	LOAD_MPZ(&(SK->q), "b7dd2014ba4c4500e04fb105c96d514bb08a5bdad4d7de4e47307be1933d2fb4368b792aebc89c1322d18c278e79afa1a9076673f4a3488b56548111b0a7cc0161503275915a29cc2e86f6694d785e5757f70ae48704bcc8c22b78a9a2b6f6bdc50df73b8dcbe12e995bfdfc5adcab11932b35cb6c70552d46f2db4152359083");

	MPZ_MUL(&(SK->n), &(SK->p), &(SK->q));
	
	SK->e.len = 1;
	SK->e.sign = 0;
	SK->e.dat[0] = 65537;

	COPY_MPZ(&(PK->n), &(SK->n));
	COPY_MPZ(&(PK->e), &(SK->e));

	one.dat[0] = 1;
	one.len = 1;
	one.sign = 0;

	MPZ_SUB(&pm1, &(SK->p), &one); //pm1 = p-1
	MPZ_SUB(&qm1, &(SK->q), &one); //qm1 = q-1

	MPZ_MUL(&phin, &pm1, &qm1); //phin =(p-1)(q-1)

	MPZ_modinv(&(SK->d), &(SK->e), &phin);
	MPZ_modinv(&(SK->qinvp), &(SK->q), &(SK->p));

	MPZ_UDIV(&temp, &(SK->dmodpm1), &(SK->d), &pm1);
	MPZ_UDIV(&temp, &(SK->dmodqm1), &(SK->d), &qm1);

}

void RSA_Encrypt(MPZ* cipher, MPZ* message, RSA_Public_Key* PK)
{
	//LtoR_Exp(cipher, message, &(PK->e), &(PK->n));
	//print_mpz(cipher);
	//RtoL_Exp(cipher, message, &(PK->e), &(PK->n));
	//print_mpz(cipher);
	//LtoR_4ary_Exp(cipher, message, &(PK->e), &(PK->n));
	//print_mpz(cipher);
	LtoR_Mont_Exp(cipher, message, &(PK->e), &(PK->n));
	//print_mpz(cipher);
	//printf("=====encryption======\n");
}

void RSA_Decrypt(MPZ* message, MPZ* cipher, RSA_Private_Key* SK)
{
	//LtoR_Exp(message, cipher, &(SK->d), &(SK->n));
	//print_mpz(message);
	//RtoL_Exp(message, cipher, &(SK->d), &(SK->n));
	//print_mpz(message);
	//LtoR_4ary_Exp(message, cipher, &(SK->d), &(SK->n));
	//print_mpz(message);
	LtoR_Mont_Exp(message, cipher, &(SK->d), &(SK->n));
	//print_mpz(message);
	//printf("=====decryption======\n");
}

void RSA_CRT_Decrypt(MPZ* message, MPZ* cipher, RSA_Private_Key* SK)
{
	MPZ Cp, Cq, Mp, Mq, temp, temp2;

	MPZ_UDIV(&temp, &Cp, cipher, &(SK->p));
	MPZ_UDIV(&temp, &Cq, cipher, &(SK->q));

	LtoR_Mont_Exp(&Mp, &Cp, &(SK->dmodpm1), &(SK->p));
	LtoR_Mont_Exp(&Mq, &Cq, &(SK->dmodqm1), &(SK->q));
	MPZ_MODSUB(&temp, &Mp, &Mq, &(SK->p));
	MPZ_MODMUL(&temp2, &temp, &(SK->qinvp), &(SK->p));
	MPZ_MUL(&temp, &temp2, &(SK->q));
	MPZ_ADD(message, &temp, &Mq);
}

void MPZ_memcpy(UINT8* buf, MPZ* a)
{
	SINT32 i, j, pos=0;
	for (i = 3; i >= 0; i--)
	{
		if (((a->dat[a->len - 1]) & (0xff << (8 * i))))
			break;
	}
	for (; i >= 0; i--)
	{
		buf[pos++] = ((a->dat[a->len - 1]) >> 8 * i) & 0xff;
	}

	for (j = a->len - 2; j >=0 ; j--)
	{
		for (i=3; i >= 0; i--)
			buf[pos++] = ((a->dat[j]) >> 8 * i) & 0xff;
	}
	
}

void Load_keyparam(UINT8* buf, SINT32* bufsize, MPZ* a)
{
	SINT32 i, size;

	size = a->len * 4;
	for (i = 3; i >= 0; i--)
	{
		if (!((a->dat[a->len - 1]) & (0xff << (8 * i))))
			size--;
		else
			break;
	}
	size++; 

	if (size >= 256)
	{
		buf[0] = 0x02;
		buf[1] = 0x82;
		buf[2] = size >> 8;
		buf[3] = size & 0xff;
		buf[4] = 0x00;
		MPZ_memcpy(buf + 5, a);
		*bufsize = size + 4;
	}
	else
	{
		buf[0] = 0x02;
		buf[1] = size;
		buf[2] = 0x00;
		MPZ_memcpy(buf + 3, a);
		*bufsize = size + 2;
	}
}

void Load_keyparam_v2(UINT8* buf, SINT32* bufsize, MPZ* a)
{
	SINT32 i, size;

	size = a->len * 4;
	for (i = 3; i >= 0; i--)
	{
		if (!((a->dat[a->len - 1]) & (0xff << (8 * i))))
			size--;
		else
			break;
	} //buf�� �ֻ�������Ʈ�� 0���� ä���� �ʴ� ���� ����..

	if (size >= 256)
	{
		buf[0] = 0x02;
		buf[1] = 0x82;
		buf[2] = size >> 8;
		buf[3] = size & 0xff;
		MPZ_memcpy(buf + 4, a);
		*bufsize = size + 4;
	}
	else
	{
		buf[0] = 0x02;
		buf[1] = size;
		MPZ_memcpy(buf + 2, a);
		*bufsize = size + 2;
	}
}

void Export_der_RSA_PubKey(UINT8* filename, RSA_Public_Key* PK)
{
	UINT8 nbuf[512], ebuf[512];
	UINT8 vbuf[3] = { 0x02, 0x01, 0x00 };
	UINT8 header[2] = { 0x30, 0x82 };
	UINT8 temp;
	SINT32 nsize, esize, err, size;
	FILE* fp;

	if ((err = fopen_s(&fp, filename, "wb"))) {
		printf("error at opening file\n");
	}

	Load_keyparam(nbuf, &nsize, &(PK->n));
	Load_keyparam_v2(ebuf, &esize, &(PK->e));

	size = nsize + esize + 3;

	if (size >= 256)
	{
		fwrite(header, sizeof(UINT8), 2, fp);
		temp = (UINT8)(size >> 8);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		temp = (UINT8)(size & 0xff);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		fwrite(vbuf, sizeof(UINT8), 3, fp);
		fwrite(nbuf, sizeof(UINT8), nsize, fp);
		fwrite(ebuf, sizeof(UINT8), esize, fp);
	}
	else
	{
		fwrite(header, sizeof(UINT8), 1, fp);
		temp = (UINT8)(size & 0xff);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		fwrite(vbuf, sizeof(UINT8), 3, fp);
		fwrite(nbuf, sizeof(UINT8), nsize, fp);
		fwrite(ebuf, sizeof(UINT8), esize, fp);
	}
	fclose(fp);
}

void Export_der_RSA_PrivateKey(UINT8* filename, RSA_Private_Key* SK)
{
	UINT8 nbuf[512], ebuf[512], dbuf[512], pbuf[512], qbuf[512], dpbuf[512], dqbuf[512], qinvpbuf[512];
	SINT32 nsize, esize, dsize, psize, qsize, dpsize, dqsize, qinvpsize;
	UINT8 vbuf[3] = { 0x02, 0x01, 0x00 };
	UINT8 header[2] = { 0x30, 0x82 };
	UINT8 temp;
	SINT32 err, size;
	FILE* fp;

	if ((err = fopen_s(&fp, filename, "wb"))) {
		printf("error at opening file\n");
	}

	Load_keyparam(nbuf, &nsize, &(SK->n));
	Load_keyparam_v2(ebuf, &esize, &(SK->e));
	Load_keyparam(dbuf, &dsize, &(SK->d));
	Load_keyparam(pbuf, &psize, &(SK->p));
	Load_keyparam(qbuf, &qsize, &(SK->q));
	Load_keyparam(dpbuf, &dpsize, &(SK->dmodpm1));
	Load_keyparam(dqbuf, &dqsize, &(SK->dmodqm1));
	Load_keyparam(qinvpbuf, &qinvpsize, &(SK->qinvp));

	size = nsize + esize + dsize + psize + qsize + dpsize + dqsize + qinvpsize + 3;

	if (size >= 256)
	{
		fwrite(header, sizeof(UINT8), 2, fp);
		temp = (UINT8)(size >> 8);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		temp = (UINT8)(size & 0xff);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		fwrite(vbuf, sizeof(UINT8), 3, fp);
		fwrite(nbuf, sizeof(UINT8), nsize, fp);
		fwrite(ebuf, sizeof(UINT8), esize, fp);
		fwrite(dbuf, sizeof(UINT8), dsize, fp);
		fwrite(pbuf, sizeof(UINT8), psize, fp);
		fwrite(qbuf, sizeof(UINT8), qsize, fp);
		fwrite(dpbuf, sizeof(UINT8), dpsize, fp);
		fwrite(dqbuf, sizeof(UINT8), dqsize, fp);
		fwrite(qinvpbuf, sizeof(UINT8), qinvpsize, fp);
	}
	else
	{
		fwrite(header, sizeof(UINT8), 1, fp);
		temp = (UINT8)(size & 0xff);
		fwrite(&temp, sizeof(UINT8), 1, fp);
		fwrite(vbuf, sizeof(UINT8), 3, fp);
		fwrite(nbuf, sizeof(UINT8), nsize, fp);
		fwrite(ebuf, sizeof(UINT8), esize, fp);
		fwrite(dbuf, sizeof(UINT8), dsize, fp);
		fwrite(pbuf, sizeof(UINT8), psize, fp);
		fwrite(qbuf, sizeof(UINT8), qsize, fp);
		fwrite(dpbuf, sizeof(UINT8), dpsize, fp);
		fwrite(dqbuf, sizeof(UINT8), dqsize, fp);
		fwrite(qinvpbuf, sizeof(UINT8), qinvpsize, fp);
	}
	fclose(fp);
}

void DER_to_PEM(UINT8* filename)
{
	SINT32 err, size, pemsize, i;
	FILE* fp;
	UINT8 buf[4096], pembuf[4096];

	if ((err = fopen_s(&fp, filename, "rb"))) {
		printf("error at opening file\n");
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread(buf, sizeof(UINT8), size, fp);
	base64_encode(pembuf, buf, size, &pemsize);

	for (i = 0; i < pemsize; i++)
	{
		printf("%c", pembuf[i]);
	}

	fclose(fp);

}