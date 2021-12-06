#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "typedef.h"

int main()
{
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	SINT64 elapsed;
	SINT32 i;
	double duringtime;
	MPZ* a;
	RSA_Private_Key SK;
	RSA_Public_Key PK;
	
	MPZ message, cipher;

	a = (MPZ*)malloc(sizeof(MPZ) * 1000);

	QueryPerformanceFrequency(&Frequency);
	
	srand(time(NULL));

	QueryPerformanceCounter(&BeginTime);

	//Gen_BigNum_File("rand_A.txt", 128, 1000);
	//Gen_BigNum_File("rand_B.txt", 64, 1000);
	//Gen_BigNum_File("input.txt", 256, 50);
	//Gen_BigNum_File("exp.txt", 256, 50);
	//Gen_BigNum_odd_File("mod.txt", 256, 50);

	/*Read_MPZ_File("rand_A.txt", a, 1000);
	for (i = 0; i < 1000; i++)
	{
		Print_MPZ(&a[i]);
	}*/

	//Copy_BigNum_File("rand_A.txt", "Copy_rand_B.txt", 128, 1000);

	//Sort_MPZ_File("rand_A.txt", "Sorted_A.txt", 1000);

	//MPZ_UADD_TEST("rand_A.txt", "rand_B.txt", "rand_C.txt", 1000);
	//MPZ_USUB_TEST("rand_A.txt", "rand_B.txt", "rand_D.txt", 1000);
	//MPZ_UMUL_TEST("rand_A.txt", "rand_B.txt", "rand_E.txt", 1000);
	//MPZ_UMUL_TEST("rand_B.txt", "rand_A.txt", "rand_F.txt", 1000);
	//MPZ_UDIV_TEST("rand_A.txt", "rand_B.txt", "q.txt", "r.txt", 1000);
	//LtoR_Exp_TEST("input.txt", "exp.txt", "mod.txt", "output1.txt", 50);
	//LtoR_4ary_Exp_TEST("input.txt", "exp.txt", "mod.txt", "output3.txt", 50);
	//RtoL_Exp_TEST("input.txt", "exp.txt", "mod.txt", "output2.txt", 50);
	//LtoR_Mont_Exp_TEST("input.txt", "exp.txt", "mod.txt", "output4.txt", 50);
	RSA_Key_Gen(&SK, &PK);

	LOAD_MPZ(&message, "31345f342443245235");
	printf("============ message ============\n");
	print_mpz(&message);
	RSA_Encrypt(&cipher, &message, &PK);
	printf("\n============ cipher ============\n");
	print_mpz(&cipher);
	RSA_CRT_Decrypt(&message, &cipher, &SK);
	printf("\n============ message ============\n");
	print_mpz(&message);

	//Export_der_RSA_PubKey("PubKey.der", &PK);
	//Export_der_RSA_PrivateKey("PriKey.der", &SK);
	DER_to_PEM("PriKey.der");

	QueryPerformanceCounter(&EndTime);

	elapsed = EndTime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;

	printf("elapsed time: %lf\n", duringtime);

	free(a);

	return 0;

}