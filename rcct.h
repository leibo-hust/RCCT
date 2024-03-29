#include <stdio.h>
#include <stdlib.h>

//c[1][n] = a[1][n] logic c[1][n]
void cim_bitmap(int opcode, int* a, int* b, int* c, int n);

// a[1][M] * B[M][N] = c[1][N];
void cim_mvm(int M, int N, float alpha, float beta, float* a, float* B, float* c);

// A[M][N] * B[N][K] = C[M][K];
void cim_mmm(int M, int N, int K, float alpha, float beta, float* A, float* B, float* C);
