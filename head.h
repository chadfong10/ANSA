#ifndef head_h
#define head_h

#include <vector>
#include <random>
using namespace std;

const int c = 5, p = 7, d = 23, v = 9;      //Number of nodes in each set
const float alpha = 2.4;                    //Penalty of Return Time Gap
const float q2 = 70;                        //Capacity of transfer vehicle
const float c2 = 1;                         //Cost per distance for transfer vehicle
const int infin = 99999;
const float infeasible = -1;
const int Tmax = 960;                       //Time horizon
const int Tcd = 60;                         //Cross-docking activity time

//initial & cal obj:
const int dummy = v-1;
extern int num ;
extern int node ;

/*two main functions*/
// Generation of initial solution
void ini_sol(int* SPr, int* SDr, int* SPv, int* SDv, float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d], float* p1, float d1[d][p], float* w, float* q1, float* c1);
// Calculation of objective value
float cal_obj(int* SPr, int* SDr, int* SPv, int* SDv, float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d], float* p1, float d1[d][p], float* w, float* q1, float* c1, float* TimeGap);

//Sub-functions of ini_sol
int closest (int v, bool Cm[p+d], float e1[c+p+d][c+p+d], int option);
int checkCm(bool Cm[p+d]);
void calamount(float* amount, float* p1, float d1[d][p]);
void changeCD(vector<int> Sr, bool Dm[c], int* CurrentCD, int *CurrentLoad, float e1[c+p+d][c+p+d], float* w);
void changeV(vector<int> Sr, bool Vm[v], int *CurrentV, int i, int *CurrentNode, int *count0, int *CurrentLoad, vector<int> Sv, float* q1);
void recordBest(int* SPrbest, int* SDrbest, int* SPvbest, int* SDvbest, int* SPr,  int* SDr, int* SPv, int* SDv);
void bestsol(int* SPrbest, int* SDrbest, int* SPvbest, int* SDvbest, int* SPr,  int* SDr, int* SPv, int* SDv);
int checkCD(bool Dm[c]);
int checkV(bool Vm[v]);

#endif /* header_hpp */
