#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "timing.h"

#include "Dense"

using Eigen::MatrixXd;
using Eigen::Map;

static const int N_ROWS = 1000;
static const int N_COLS = 1000;

/*double R[4][4] = {  12,    6, -4, 1,
                   -51,  167, 24, 2,
                     4,  -68,-41, 4,
                     5,  -70,-40, 3};
double Q[4][4] = { 1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1};
*/

double drand() {
	double s1 = rand()  /  (double) RAND_MAX;
	return s1;
}

double sign(double a) {
	if(a > 0)
		return 1;
	else if (a < 0)
		return -1;
	else
		return 0;
}

int main() {
	double v[N_ROWS];
	double (*R)[N_ROWS] = (double(*)[N_ROWS]) malloc(sizeof(double) * N_COLS * N_ROWS);
	double (*Q)[N_ROWS] = (double(*)[N_ROWS]) malloc(sizeof(double) * N_COLS * N_ROWS);

	for(int i = 0; i < N_ROWS; i++) {
		for(int j = 0; j < N_COLS; j++) {
			R[i][j] = drand();
			Q[i][j] = (i==j) ? 1 : 0;
		}
	}

	double begin = current_time();

	for(int c = 0; c < N_COLS-1; c++) {
		double lcl = 0.0;
		
		double b = 0.0;
		lcl += R[c][c] * R[c][c];
		
		for(int r = c + 1; r < N_ROWS; r++) {
			double tmp = R[c][r] * R[c][r];
			v[r] = R[c][r];
			lcl += tmp;
			b += tmp;
		}
		lcl = sqrt(lcl);
		//printf("lcl = %f\n",lcl);
		double a = R[c][c] + sign(R[c][c]) * lcl;
		v[c] = a;
		b += a * a;
		//printf("b = %f\n", b);
		double two_on_b = 2.0 / b;
		
		double v_t_R[N_COLS];
		double Q_v[N_ROWS];
		
		for(int cc = 0; cc < N_COLS; cc++) {
			double tmp = 0.0;
			for(int rr = c; rr < N_ROWS; rr++) {
				tmp += R[cc][rr] * v[rr];
			}
			v_t_R[cc] = tmp;
			//printf("v_t_R[%d] = %f\n",cc,v_t_R[cc]);
		}
		
		for(int rr = 0; rr < N_ROWS; rr++) {
			double tmp = 0.0;
			for(int cc = c; cc < N_COLS; cc++) {
				tmp += Q[rr][cc] * v[cc];
			}
			Q_v[rr] = tmp;
			//printf("Q_v[%d] = %f\n",rr,Q_v[rr]);
		}

		for(int k = 0; k < N_ROWS; k++) {
			v[k] *= two_on_b;
		}

		for(int cc = 0; cc < N_COLS; cc++) {
			for(int rr = c; rr < N_ROWS; rr++) {
				R[cc][rr] -= v[rr] * v_t_R[cc];
			}
		}
		for(int rr = 0; rr < N_ROWS; rr++) {
			for(int cc = c; cc < N_COLS; cc++) {
				Q[rr][cc] -= Q_v[rr] * v[cc];
			}
		}
	}

	/*MatrixXd qr_m  = Map<MatrixXd>((double*)R, N_ROWS, N_COLS);

        Eigen::HouseholderQR<MatrixXd> hqr(qr_m);
	MatrixXd r = hqr.matrixQR();
	std::cout << r(500,500) << std::endl;	*/

	printf("Elapsed: %f\n", current_time()-begin);
	
	/*for(int rr = 0; rr < N_ROWS; rr++) {
		for(int cc = 0; cc < N_COLS; cc++) {
			printf("%f ",Q[rr][cc]);
		}
		printf("\n");
	}
	for(int rr = 0; rr < N_ROWS; rr++) {
		for(int cc = 0; cc < N_COLS; cc++) {
			printf("%f ",R[cc][rr]);
		}
		printf("\n");
	}*/
	return 0;
}
