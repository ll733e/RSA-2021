#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

void print_mpz(MPZ *a)
{
	SINT32 i;
	for (i = a->len - 1; i >= 0; i--)
	{
		printf("%08x", a->dat[i]);
	}
	printf("\n\n");
}

void Gen_BigNum_File(SINT8* filename, SINT32 bytes, SINT32 num) {
	FILE* fp;
	SINT32 err, i, j;

	if ((err = fopen_s(&fp, filename, "w"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++) {
		for (j = 0; j < bytes; j++)
		{
			fprintf(fp, "%02x", rand() & 0xff);
		}
		fprintf(fp, "\n\n");
	}
	fclose(fp);
}

void Gen_BigNum_odd_File(SINT8* filename, SINT32 bytes, SINT32 num) {
	FILE* fp;
	SINT32 err, i, j;

	if ((err = fopen_s(&fp, filename, "w"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++) {
		for (j = 0; j < bytes-1; j++)
		{
			fprintf(fp, "%02x", rand() & 0xff);
		}
		fprintf(fp, "%02x", (rand() & 0xff)|1);
		fprintf(fp, "\n\n");
	}
	fclose(fp);
}

void Read_MPZ_File(SINT8* filename, MPZ* a, SINT32 num) {
	FILE* fp;
	SINT32 err, i, j, k, pos, bytes;
	SINT8 buf[2048], x;

	if ((err = fopen_s(&fp, filename, "r"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++) {
		a[i].sign = 0;
		fgets(buf, 2048, fp);
		bytes=strlen(buf) / 2;
		a[i].len = (bytes + 3) / 4;
		pos = bytes * 2 - 1;
		
		for (j = 0; j < a[i].len; j++)
		{
			a[i].dat[j] = 0;
			for (k = 0; k < 8; k++)
			{
				x = buf[pos--];
				if (x >= 'A' && x <= 'F')x = x - 'A' + 10;
				else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
				else if (x >= '0' && x <= '9')x = x - '0';
				a[i].dat[j] = (x<<28)^(a[i].dat[j]>>4);
				if (pos == -1)break;
			}
		}
		for (; k < 7; k++)
			a[i].dat[a[i].len-1] >>= 4;

		for (k = a[i].len - 1; k >= 0; k--)
		{
			if (a[i].dat[k] != 0)
				break;
		}
		a[i].len = k + 1;
		fgets(buf, 2048, fp);

	}

	fclose(fp);
}

void Print_MPZ(MPZ* a) {  //sign = 0 : non-negative
	SINT32 i;

	for (i = a->len - 1; i >= 0; i--)
	{
		printf("%08x", a->dat[i]);
	}
	printf("\n\n");
}

void Write_MPZ_File(SINT8* filename, MPZ* a, SINT32 num) {
	FILE* fp;
	SINT32 err, i, j, k, pos, bytes;
	SINT8 buf[2048], x;

	if ((err = fopen_s(&fp, filename, "w"))) {
		printf("error at opening file\n");
	}
	for (i = 0; i < num; i++)
	{
		for (k = a[i].len - 1; k >= 0; k--)
			fprintf(fp, "%08x", a[i].dat[k]);
		fprintf(fp, "\n\n");
	}
	
	fclose(fp);
}

void Copy_BigNum_File(SINT8* filename1, SINT8* filename2, SINT32 bytes, SINT32 num) {
	FILE *fp1, *fp2;
	SINT32 err, i, j, k, pos;
	SINT8 buf[2048], x;
	MPZ a;

	if ((err = fopen_s(&fp1, filename1, "r"))) {
		printf("error at opening file\n");
	}

	if ((err = fopen_s(&fp2, filename2, "w"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++) {
		a.sign = 0;
		fgets(buf, 2048, fp1);
		bytes = strlen(buf) / 2;
		a.len = (bytes + 3) / 4;
		pos = bytes * 2 - 1;

		for (j = 0; j < a.len; j++)
		{
			a.dat[j] = 0;
			for (k = 0; k < 8; k++)
			{
				x = buf[pos--];
				if (x >= 'A' && x <= 'F')x = x - 'A' + 10;
				else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
				else if (x >= '0' && x <= '9')x = x - '0';
				a.dat[j] = (x << 28) ^ (a.dat[j] >> 4);
				if (pos == -1)break;
			}
		}
		for (; k < 8; k++)
			a.dat[j] >>= 4;

		for (k = a.len - 1; k >= 0; k--)
		{
			if (a.dat[k] != 0)
				break;
		}
		a.len = k + 1;
		fgets(buf, 2048, fp1);

		for (k = a.len - 1; k >= 0; k--)
			fprintf(fp2, "%08x", a.dat[k]);
		fprintf(fp2, "\n\n");

	}
	
	fclose(fp1);
	fclose(fp2);
}

SINT32 Compare_MPZ(MPZ *a, MPZ *b) // a>=b return 1, else return 0.
{
	SINT32 i;
	if ((a->sign == 0) && (b->sign == 1)) return 1;
	else if ((a->sign == 1) && (b->sign == 0)) return 0;
	else if ((a->sign == 0) && (b->sign == 0))
	{
		if (a->len > b->len)return 1;
		else if(a->len < b->len)return 0;
		else {
			for (i = a->len - 1; i >= 0; i--)
			{
				if (a->dat[i] > b->dat[i]) return 1;
				else if (a->dat[i] < b->dat[i]) return 0;
			}
			return 1;
		}
	}
	else
	{
		if (a->len > b->len)return 0;
		else if (a->len < b->len)return 1;
		else {
			for (i = a->len - 1; i >= 0; i--)
			{
				if (a->dat[i] > b->dat[i]) return 0;
				else if (a->dat[i] < b->dat[i]) return 1;
			}
			return 1;
		}
	}
}

void Swap_MPZ(MPZ *a, MPZ *b)
{
	MPZ tmp;
	SINT32 i;

	tmp.sign = a->sign;
	tmp.len = a->len;
	for (i = 0; i < a->len; i++)
	{
		tmp.dat[i] = a->dat[i];
	}

	a->sign = b->sign;
	a->len = b->len;
	for (i = 0; i < b->len; i++)
	{
		a->dat[i] = b->dat[i];
	}

	b->sign = tmp.sign;
	b->len = tmp.len;
	for (i = 0; i < tmp.len; i++)
	{
		b->dat[i] = tmp.dat[i];
	}
}

Sort_MPZ_File(SINT8* filename1, SINT8* filename2, SINT32 num) {

	MPZ *a;
	SINT32 i, j;
	a = (MPZ*)malloc(sizeof(MPZ)*num);
	Read_MPZ_File(filename1, a, num);
	
	for(i=num-1;i>=0;i--)
		for (j = 0; j < i; j++)
		{
			if (Compare_MPZ(&a[j], &a[j + 1]))Swap_MPZ(&a[j], &a[j + 1]);
		}

	Write_MPZ_File(filename2, a, num);

	free(a);
}

void MPZ_UADD(MPZ *c, MPZ *a, MPZ *b) //a>=0, b>=0
{
	SINT32 carry, i;
	UINT32 temp;

	if (a->len >= b->len) {
		carry = 0;
		for (i = 0; i < b->len; i++)
		{
			if ((temp = a->dat[i] + carry) < carry) //carry = 1 , temp = 0;
				temp = b->dat[i];     //temp = a[i]+b[i]+carry
			else if ((temp += b->dat[i]) < b->dat[i]) carry = 1;
			else carry = 0;
			c->dat[i] = temp;
		}
		for (i = b->len; i < a->len; i++)
		{
			if ((temp = a->dat[i] + carry) >= carry) //carry = 1 , temp = 0;
				carry = 0;    //temp = a[i]+b[i]+carry
			c->dat[i] = temp;
		}
		if (carry == 1)
		{
			c->dat[a->len] = 1;
			c->len = a->len + 1;
		}
		else c->len = a->len;	
	}
	else
	{
		carry = 0;
		for (i = 0; i < a->len; i++)
		{
			if ((temp = b->dat[i] + carry) < carry) //carry = 1 , temp = 0;
				temp = a->dat[i];     //temp = a[i]+b[i]+carry
			else if ((temp += a->dat[i]) < a->dat[i]) carry = 1;
			else carry = 0;
			c->dat[i] = temp;
		}
		for (i = a->len; i < b->len; i++)
		{
			if ((temp = b->dat[i] + carry) >= carry) //carry = 1 , temp = 0;
				carry = 0;    //temp = a[i]+b[i]+carry
			c->dat[i] = temp;
		}
		if (carry == 1)
		{
			c->dat[b->len] = 1;
			c->len = b->len + 1;
		}
		else c->len = b->len;
	}
}

void MPZ_USUB(MPZ* c, MPZ* a, MPZ* b) //a>=0, b>=0
{
	SINT32 borrow, i;
	UINT32 temp;

	if (Compare_MPZ(a, b) == 1) {
		c->sign = 0;
		borrow = 0;
		for (i = 0; i < b->len; i++) //ci = ai-bi-borrow=(ai-borrow)-bi
		{
			if (a->dat[i] >= borrow)
			{
				temp = a->dat[i] - borrow;
				if ((c->dat[i] = temp - b->dat[i]) > temp)
					borrow = 1;
				else
					borrow = 0;
			}
			else
				c->dat[i] = 0xFFFFFFFF - b->dat[i];
		}

		for (i = b->len; i < a->len; i++) //ci=ai-borrow
		{
			if (a->dat[i] >= borrow)
			{
				c->dat[i] = a->dat[i] - borrow;
				borrow = 0;
			}
			else
				c->dat[i] = 0xFFFFFFFF;
		}

		c->len = a->len;
		for (i = a->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
	}
	else  //c=a-b , b>a ==> c=a-b=-(b-a)
	{
		c->sign = 1;
		borrow = 0;
		for (i = 0; i < a->len; i++) //ci = bi-ai-borrow=(bi-borrow)-ai
		{
			if (b->dat[i] >= borrow)
			{
				temp = b->dat[i] - borrow;
				if ((c->dat[i] = temp - a->dat[i]) > temp)
					borrow = 1;
				else
					borrow = 0;
			}
			else
				c->dat[i] = 0xFFFFFFFF - a->dat[i];
		}

		for (i = a->len; i < b->len; i++) //ci=bi-borrow
		{
			if (b->dat[i] >= borrow)
			{
				c->dat[i] = b->dat[i] - borrow;
				borrow = 0;
			}
			else
				c->dat[i] = 0xFFFFFFFF;
		}

		c->len = b->len;
		for (i = b->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
	}
}

void MPZ_UMUL(MPZ* c, MPZ* a, MPZ* b) //a>=0, b>=0
{
	SINT32 i, j;
	UINT64 UV;
	UINT32 U;

	for (i = 0; i< b->len; i++)
		c->dat[i] = 0;

	for (i = 0; i < a->len; i++)
	{
		U = 0;
		for (j = 0; j < b->len; j++)
		{
			UV = c->dat[i + j] + (UINT64)a->dat[i] * b->dat[j] + U;
			U = UV >> 32;
			c->dat[i+j] = UV & 0xfffffffff;
		}
		c->dat[i + b->len] = U;
	}
	c->len = a->len + b->len;
	for (i = c->len - 1; i >= 0; i--)
	{
		if (c->dat[i] == 0)
			c->len--;
		else
			break;
	}

}

void COPY_MPZ(MPZ* r, MPZ* a)
{
	SINT32 i;
	for (i = 0; i < a->len; i++)
		r->dat[i] = a->dat[i];
	r->len = a->len;
	r->sign = a->sign;
}

void MPZ_BIT_SHIFT(MPZ* r, MPZ* a, SINT32 shift) //if shift>=0, left shift. Else, right shift, -32<shift<32
{
	SINT32 i;
	
	if (shift == 0)
	{
		COPY_MPZ(r, a);
	}
	else if (shift > 0)
	{
		r->sign = a->sign;
		r->len = a->len + 1;
		r->dat[a->len] = a->dat[a->len - 1] >> (32 - shift);
		for (i = a->len - 1; i >= 1; i--)
		{
			r->dat[i] = (a->dat[i] << shift) | (a->dat[i - 1] >> (32 - shift));
		}
		r->dat[0] = a->dat[0] << shift;
		if (r->dat[a->len] == 0)
			r->len = a->len;
	}
	else
	{
		r->sign = a->sign;
		r->len = a->len;
		for (i = 0; i < a->len-1; i++)
		{
			r->dat[i] = (a->dat[i] >> (-shift)) | (a->dat[i + 1] << (32 + shift));
		}
		r->dat[a->len - 1] = a->dat[a->len - 1] >> (-shift);
		if (r->dat[a->len - 1] == 0)
			r->len = a->len - 1;
		if (r->len == 0)
			r->sign = 0;
	}
}

void MPZ_WORD_SHIFT(MPZ* r, MPZ* a, SINT32 shift) //if shift>=0, left shift. Else, right shift
{
	SINT32 i;

	if (shift == 0)
	{
		COPY_MPZ(r, a);
	}
	else if (shift > 0)
	{
		for (i = a->len - 1; i >= 0; i--)
			r->dat[i + shift] = a->dat[i];
		for (i = shift - 1; i >= 0; i--)
			r->dat[i] = 0;
		r->sign = a->sign;
		r->len = a->len + shift;
	}
	else
	{
		for (i = -shift; i < a->len; i++)
			r->dat[i + shift] = a->dat[i];
		r->sign = a->sign;
		r->len = a->len + shift;
		if (r->len <= 0)
		{
			r->len = 0;
			r->sign = 0;
		}
	}
}

void MUL_WORD_MPZ(MPZ* r, UINT32 t, MPZ* a)
{
	SINT32 i;
	UINT64 UV;
	UINT32 U;

	r->sign = a->sign;
	r->len = a->len;

	U = 0;
	for (i = 0; i < a->len; i++)
	{
		UV = (UINT64)a->dat[i] * t + U;
		U = UV >> 32;
		r->dat[i] = UV & 0xffffffff;
	}
	if (U != 0)
	{
		r->dat[a->len] = U;
		r->len = a->len + 1;
	}

}

void MPZ_ADD(MPZ* c, MPZ* a, MPZ* b)
{
	if (a->sign == 0 && b->sign == 0)
	{
		MPZ_UADD(c, a, b);
		c->sign = 0;
	}
	else if (a->sign == 1 && b->sign == 1) //c = -((-a)+(-b))
	{
		MPZ_UADD(c, a, b);
		c->sign = 1;
	}
	else if (a->sign == 0 && b->sign == 1) //c = -((-a)+(-b))
	{
		MPZ_USUB(c, a, b); //a>=0, b>=0 ==> c
	}else //c=a+b = b-(-a)
	{
		MPZ_USUB(c, b, a);
	}

}

void MPZ_SUB(MPZ* c, MPZ* a, MPZ* b)
{
	if (a->sign == 0 && b->sign == 0)
	{
		MPZ_USUB(c, a, b);
	}
	else if (a->sign == 1 && b->sign == 1) //c = a-b=((-b)-(-a))
	{
		MPZ_USUB(c, b, a);
	}
	else if (a->sign == 0 && b->sign == 1) //c = a-b=((-b)+(a))
	{
		MPZ_UADD(c, a, b); //a>=0, b>=0 ==> c
		c->sign = 0;
	}
	else //c = a-b=-((b)+(-a))
	{
		MPZ_UADD(c, b, a);
		c->sign = 1;
	}

}

void MPZ_MUL(MPZ* c, MPZ* a, MPZ* b)
{
	if (a->sign == 0 && b->sign == 0)
	{
		MPZ_UMUL(c, a, b);
		c->sign = 0;
	}
	else if (a->sign == 1 && b->sign == 1) //c = a-b=((-b)-(-a))
	{
		MPZ_UMUL(c, a, b);
		c->sign = 0;
	}
	else if (a->sign == 0 && b->sign == 1) //c = a-b=((-b)+(a))
	{
		MPZ_UMUL(c, a, b);
		c->sign = 1;
		if (c->len == 0)c->sign = 0;
	}
	else //c = a-b=-((b)+(-a))
	{
		MPZ_UMUL(c, a, b);
		c->sign = 1;
		if (c->len == 0)c->sign = 0;
	}

}

void MPZ_UDIV(MPZ* q, MPZ* r, MPZ* a, MPZ* b) //a>=0, b>0
{
	SINT32 i, normbits;
	UINT32 x0, x1, x2;
	UINT64 x_, y_;
	MPZ x, y, tmp, tmp2;

	if (Compare_MPZ(a, b) == 0)
	{
		q->len = 0;
		q->sign = 0;
		COPY_MPZ(r, a);
	}
	else
	{
		//biginning of normalization
		for (i = 31; i >= 0; i--)
		{
			if ((b->dat[b->len - 1] & (1 << i)))
				break;
		}
		normbits = 31 - i;

		MPZ_BIT_SHIFT(&x, a, normbits);
		MPZ_BIT_SHIFT(&y, b, normbits); //end of normalization

		for (i = 0; i <= x.len - y.len; i++)
			q->dat[i] = 0;
		q->len = x.len - y.len + 1;
		MPZ_WORD_SHIFT(&tmp, &y, x.len - y.len);

		if (Compare_MPZ(&x, &tmp) == 1)
		{
			q->dat[x.len - y.len]++;
			MPZ_USUB(&x, &x, &tmp);
		}

		for (i = x.len - 1; i >= y.len; i--)
		{
			if (x.dat[i] == y.dat[y.len - 1])
				q->dat[i - y.len] = 0xFFFFFFFF;
			else
				q->dat[i - y.len] = (((UINT64)x.dat[i] << 32) | x.dat[i - 1]) / y.dat[y.len - 1]; //step3.1

			for(;;)
			{
				x_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 2];
				x0 = x_ & 0xffffffff;
				y_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 1];
				y_ += (x_ >> 32);
				x1 = y_ & 0xffffffff;
				x2 = y_ >> 32;
				if (x2 < x.dat[i]) break;
				else if (x2 == x.dat[i])
				{
					if (x1 < x.dat[i - 1])break;
					if (x1 == x.dat[i - 1])
					{
						if (x0 <= x.dat[i - 2])break;
					}	
				}
				q->dat[i - y.len]--;
			}//step3.2

			MPZ_WORD_SHIFT(&tmp, &y, i - y.len); //tmp = y*b^{i-t-1}
			MUL_WORD_MPZ(&tmp2, q->dat[i - y.len], &tmp);
			MPZ_USUB(&x, &x, &tmp2);
			if (x.sign == 1)
			{
				MPZ_ADD(&x, &x, &tmp);
				q->dat[i - y.len]--;
			}
		}
		MPZ_BIT_SHIFT(r, &x, -normbits);
		q->sign = 0;
		for (i = q->len - 1; i >= 0; i--)
		{
			if (q->dat[i] == 0)
				q->len--;
			else
				break;
		}
	}
}

void MPZ_MODADD(MPZ* r, MPZ* a, MPZ* b, MPZ* n) //r=a+b mod n, 0<=a,b<n
{
	MPZ_ADD(r, a, b);
	if (Compare_MPZ(r, n) == 1)
	{
		MPZ_SUB(r, r, n);
	}
}

void MPZ_MODSUB(MPZ* r, MPZ* a, MPZ* b, MPZ* n) //r=a-b mod n, 0<=a,b<n
{
	MPZ_SUB(r, a, b);
	if (r->sign == 1)
	{
		MPZ_ADD(r, r, n);
	}
}

void MPZ_MODMUL(MPZ* r, MPZ* a, MPZ* b, MPZ* n) //r=a*b mod n, 0<=a,b<n
{
	MPZ x, y;
	MPZ_MUL(&x, a, b);
	MPZ_UDIV(&y, r, &x, n);
}

UINT32 minus_modinv(SINT64 m, SINT64 mod) //return (-m^{-1}) mod (mod)
{
	SINT64 r1, r2, r, q;
	SINT64 t1 = 0, t2 = 1, t;

	if (mod == 1)
		return 0;

	r1 = mod;
	r2 = m;

	while (r1 > 1)
	{
		q = r1 / r2;
		r = r1 % r2;
		t = t1 - q * t2;
		r1 = r2;
		r2 = r;
		t1 = t2;
		t2 = t;
	}
	t1 = (-t1) % mod;
	if (t1 < 0)t1 += mod;
	return (UINT32)t1;
}

void MPZ_modinv(MPZ* result, MPZ* a, MPZ* mod)
{
	MPZ r1, r2, r, q, t1, t2, t, two, temp;

	t1.len = 0;
	t1.dat[0] = 0;
	t1.sign = 0;  //t1=0;
	t2.len = 1;
	t2.dat[0] = 1;
	t2.sign = 0;	//t2=1;

	two.len = 1;
	two.dat[0] = 2;
	two.sign = 0;

	COPY_MPZ(&r1, mod);
	COPY_MPZ(&r2, a);

	while (Compare_MPZ(&r1, &two))  //r1>=2
	{
		MPZ_UDIV(&q, &r, &r1, &r2);
		MPZ_MODMUL(&temp, &q, &t2, mod);
		MPZ_MODSUB(&t, &t1, &temp, mod);
		COPY_MPZ(&r1, &r2);
		COPY_MPZ(&r2, &r);
		COPY_MPZ(&t1, &t2);
		COPY_MPZ(&t2, &t);
	}
	COPY_MPZ(result, &t1);
}

void Mont_MUL(MPZ* r, MPZ* x, MPZ* y, MPZ* mod, UINT32 minv) //xyR-1 mod n
{
	SINT32 i;
	UINT32 u;
	MPZ temp, A;

	A.len = 0;
	A.sign = 0;
	A.dat[0] = 0;

	for (i = 0; i < x->len; i++)
	{
		u = (A.dat[0] + x->dat[i] * y->dat[0]) * minv;
		MUL_WORD_MPZ(&temp, x->dat[i], y);
		MPZ_UADD(&A, &A, &temp);
		MUL_WORD_MPZ(&temp, u, mod);
		MPZ_UADD(&A, &A, &temp);
		MPZ_WORD_SHIFT(&A, &A, -1);
	}
	for (i = x->len; i < mod->len; i++)
	{
		u = A.dat[0] * minv;
		MUL_WORD_MPZ(&temp, u, mod);
		MPZ_UADD(&A, &A, &temp);
		MPZ_WORD_SHIFT(&A, &A, -1);
	}

	if (Compare_MPZ(&A, mod) == 1)
		MPZ_USUB(r, &A, mod);
	else
		COPY_MPZ(r, &A);

	for (i = r->len - 1; i >= 0; i--)
	{
		if (r->dat[i] == 0)
			r->len--;
		else
			break;
	}
}


void MPZ_UADD_TEST(SINT8 *rbuf1, SINT8 *rbuf2, SINT8 *wbuf, SINT32 num)
{
	MPZ *a, *b, *c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ)*num);
	b = (MPZ*)malloc(sizeof(MPZ)*num);
	c = (MPZ*)malloc(sizeof(MPZ)*num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		MPZ_UADD(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_File(wbuf, c, num);

	free(a);
	free(b);
	free(c);
}

void MPZ_USUB_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		MPZ_USUB(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_File(wbuf, c, num);

	free(a);
	free(b);
	free(c);
}

void MPZ_UMUL_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		MPZ_UMUL(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_File(wbuf, c, num);

	free(a);
	free(b);
	free(c);
}

void MPZ_UDIV_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf1, SINT8* wbuf2, SINT32 num)
{
	MPZ* a, * b, * c, *d;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		MPZ_UDIV(&c[i], &d[i], &a[i], &b[i]);
	}

	Write_MPZ_File(wbuf1, c, num);
	Write_MPZ_File(wbuf2, d, num);

	free(a);
	free(b);
	free(c);
	free(d);
}