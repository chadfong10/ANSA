#include <iostream>
#include "head.h"
#include <time.h>
#include <string>
#include <math.h>
#include <fstream>
using namespace std;

//random_engine
random_device rd;
mt19937 generator(rd());
uniform_int_distribution<int> distribution(0, 3); //randomly select one of four lists
uniform_real_distribution<float> distribution1(0.0, 1.0); //probabilities of three moves
uniform_int_distribution<int> random_SPR(0, c + p + v - 2); //randomly select one element from SPr list 
uniform_int_distribution<int> random_SDR(0, c + d + v - 2); //randomly select one element from SDr list 
uniform_int_distribution<int> random_SV(0, v - 1); //randomly select one element from  SPv or SDv list

void Move_swap(int x[], int y, int y1);
void Move_insert(int x[], int y, int y1);
void Move_reverse(int x[], int y, int y1);
void show(int SPr[c + p + v - 1], int SDr[c + d + v - 1], int SPv[v], int SDv[v]);
void calculate_avg( vector<float> result_objvalue, vector<float>  result_objcost,\
                     vector<float>  result_CPU, float* result_avgvalue, float* result_avgCPU);
void opendata(float xy[c+p+d][2], float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d],\
                float* p1, float d1[d][p], float* w, float* q1, float* c1);

fstream fin, file, route, result;
int iteration = 0;

int main()
{
    float e1[c + p + d][c + p + d] = {0}; // Travel distance
    float t1[c + p + d][c + p + d] = {0};         // Travel time
    float p1[p] = {0};                    // Supply of pickup nodes
    float d1[d][p] = {0};                 // Demand of delivery nodes from pickup nodes
    float w[c] = {0};                     // Capacity of CD
    float q1[v] = {0};                    // Capacity of vehicles
    float c1[v] = {0};                    // Cost per distance for vehicles
    float xy[c+p+d][2] = {0};             // Coordinates of nodes

    int SPr[c + p + v - 1] = {}; //Pickup routes
    int SDr[c + d + v - 1] = {}; //Delivery routes
    int SPv[v] = {}; //Pickup vehicle priority list
    int SDv[v] = {}; //Delivery vehicle priority list

    //Current solution
    int SPr_X[c + p + v - 1] = {};
    int SDr_X[c + d + v - 1] = {};
    int SPv_X[v] = {};
    int SDv_X[v] = {};

    //New solution
    int SPr_Y[c + p + v - 1] = {};
    int SDr_Y[c + d + v - 1] = {};
    int SPv_Y[v] = {};
    int SDv_Y[v] = {};

    int method;
    int instance;
    float runtime = (float)clock() / CLOCKS_PER_SEC;
    float currenttime = (float)clock() / CLOCKS_PER_SEC;
    result.open("result/30N/result.txt", ios::out | ios::trunc);
    result << "ins" << "\t"
    << "Avg.Value" <<"\t"
    << "Best Value" <<"\t"
    << "Best Cost" <<"\t"
    << "CPU time" <<"\t"<<endl;

    cout << "choose one of method"<<endl;
    cout << "[1] processing specified instance"<<endl;
    cout << "[2] processing all instances"<<endl;
    cin >> method;//選擇執行單一instance或者全部instances

    if (method==1){
        cout << "choose which instance to be processed(1~30)"<<endl;
        cin >> instance;
        num = instance;
    }
    else{
        num = 1;
        instance = 30;
    }

    while(num <= instance){
        
        result << num << "\t";
        
        //Loading the parameters
        opendata(xy, e1, t1, p1, d1, w, q1, c1);

        //Files for recording the results
        file.open("result/30N/output_"+to_string(num)+".txt", ios::out | ios::trunc);
        route.open("result/30N/recordroute_"+to_string(num)+".txt", ios::out | ios::trunc);
        
        vector<float> result_objvalue;
        vector<float>  result_objcost;
        vector<float>  result_CPU;
        float result_avgvalue = 0;
        float result_avgCPU = 0;
        float result_bestcost = infin;
        float result_bestvalue = infin;
        
        iteration = 1;
        while(iteration <= 10){  // Every instance is solved 10 times.
            
            file << "=============iteration "<<iteration<<"============="<<endl;
            route << "=============iteration "<<iteration<<"============="<<endl;
            ini_sol(SPr, SDr, SPv, SDv, e1, t1, p1, d1, w, q1, c1);
            
            /*ANSA parameters and variables*/
            float T_0 = 3; //起始溫度
            float T_f = 0.01; //最低溫度
            float T = 0.0; //目前溫度 
            float rate = 0.9f; //降溫速度
            float K = 1.0; //波茲曼常數
            int I = 600*(c+p+d); //每個溫度下的迭代次數
            float R_t[3] = {0.33333f, 0.33333f, 0.33333f}; //初始化選擇機率
            int random_list = 0;
            float random_move = 0;
            int *X_best[4] = {SPr, SDr, SPv, SDv}; //目前最佳解
            int *X[4] = {SPr_X, SDr_X, SPv_X, SDv_X}; //改善解
            int *Y[4] = {SPr_Y, SDr_Y, SPv_Y, SDv_Y}; //鄰域解
            int arr_size[4] = {c + p + v - 1, c + d + v - 1, v, v};
            int f_index = 0;
            int l_index = 0;
            int select_move = 0;
            float delta = 0.0;
            float r = 0.0;
            float obj_X = 0;
            float obj_Y = 0;
            float F_best = 0;
            float F_worst = 1;
            float TimeGap = 0;
            float TimeGap_best = 0;
            float O_bar[3] = {0};
            float O = 0;
            
            /*ANSA*/
            //初始化
            obj_X = infin;//先將目前的目標值設為極大值
            F_best = obj_X;
            TimeGap_best = TimeGap;
            T = T_0;
            
            //將目前解設定為起始解
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < arr_size[j]; k++)
                {
                    X[j][k] = X_best[j][k];
                }
            }
            
           //外迴圈
            while (T > T_f)//是否到最低溫度
            {
                float O_t[3] = {0};//累積目標值
                int N_t[3] = {0};//選擇次數
                O = 0;
                
                for (int i = 0; i < I; i++)
                {
                    random_list = distribution(generator);
                    random_move = distribution1(generator);
                    
                     //依照選到的list，從中隨機產生不大於串列長度的兩個不相同的數字
                    if (arr_size[random_list] == v)
                    {
                        do
                        {
                            f_index = random_SV(generator);
                            l_index = random_SV(generator);
                        } while (f_index == l_index);
                    }
                    else if(arr_size[random_list] == c + p + v - 1)
                    {
                        do
                        {
                            f_index = random_SPR(generator);
                            l_index = random_SPR(generator);
                        } while (f_index == l_index);
                    }
                    else
                    {
                        do
                        {
                            f_index = random_SDR(generator);
                            l_index = random_SDR(generator);
                        } while (f_index == l_index);
                    }
                    
                    //初始化鄰域解為上次迭代的解
                    for (int j = 0; j < 4; j++)
                    {
                        for (int k = 0; k < arr_size[j]; k++)
                        {
                            Y[j][k] = X[j][k];
                        }
                    }
                    
                    //依據各move的選擇機率選擇move
                    if (random_move <= R_t[0])
                    {
                        Move_swap(Y[random_list], f_index, l_index);
                        select_move = 0;
                    }
                    else if (random_move > R_t[0] && random_move <= R_t[0] + R_t[1])
                    {
                        Move_reverse(Y[random_list], f_index, l_index);
                        select_move = 1;
                    }
                    else
                    {
                        Move_insert(Y[random_list], f_index, l_index);
                        select_move = 2;
                    }
                    
                    N_t[select_move]++;
                    
                    //計算鄰域解的目標值
                    obj_Y = cal_obj(SPr_Y, SDr_Y, SPv_Y, SDv_Y, e1, t1, p1, d1, w, q1, c1, &TimeGap);
                    
                    if (obj_Y >= 0)//判斷鄰域解是否可行。不可行解目標值為-1;可行解則是會大於0
                    {
                        O_t[select_move] = O_t[select_move] + obj_Y;
                        delta = obj_Y - obj_X;
                        
                        if (delta <= 0)//若鄰域解比目前的解好則更新
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                for (int k = 0; k < arr_size[j]; k++)
                                {
                                    X[j][k] = Y[j][k];
                                }
                            }
                            obj_X = obj_Y;
                        }
                        else
                        {
                            r = distribution1(generator);
                            if (r < exp((float)(-delta) / (K * T)))//依據波茲曼函數來決定是否接受比較差的解
                            {
                                for (int j = 0; j < 4; j++)
                                {
                                    for (int k = 0; k < arr_size[j]; k++)
                                    {
                                        X[j][k] = Y[j][k];
                                    }
                                }
                                obj_X = obj_Y;
                            }
                        }
                        
                        if (obj_X < F_best)//判斷是否為目前最佳的解
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                for (int k = 0; k < arr_size[j]; k++)
                                {
                                    X_best[j][k] = X[j][k];
                                }
                            }
                            route << "--------"
                            << "T:" << T << ",I:" << i + 1 << "--------" << endl;
                            show(SPr, SDr, SPv, SDv);
                            route << "obj_Y:" << obj_Y << endl;
                            route << "obj_X:" << obj_X << endl;
                            route << "obj_best:" << F_best << endl;
                            route << "obj_worst:" << F_worst << endl;
                            route << "---------------------------" << endl << endl;
                            F_best = obj_X;
                            TimeGap_best = TimeGap;

                        }
                        
                        if (obj_Y > F_worst)//判斷是否為目前最差的解
                        {
                            F_worst = obj_Y;
                        }
                    }
                    else //若出現不可行解則累積目標值改成加上目前最差的目標值
                    {
                        O_t[select_move] = O_t[select_move] + F_worst;
                        continue;
                    }
                }
                for (int j = 0; j < 3; j++)
                {
                    if (N_t[j] == 0) //若沒被選擇過則累積目標值設為目前最大的目標值
                    {
                        O_t[j] = F_worst;
                        N_t[j] = 1;
                    }
                    O_bar[j] = (float)(N_t[j]) / O_t[j];
                    O += O_bar[j];
                }
                
                for (int j = 0; j < 3; j++)
                {
                    R_t[j] = O_bar[j] / O;
                }
                T = rate * T;//降溫
            }
            
            route << endl
            << "------Optimal solution------" << endl;
            show(SPr, SDr, SPv, SDv);
            route << endl
            << "objective value= " << F_best << endl
            << "objective time gap= " << TimeGap_best << endl
            << "objective cost= " << F_best - alpha * TimeGap_best << endl;
            file << endl
            << F_best << endl
            << TimeGap_best << endl
            << F_best - alpha * TimeGap_best << endl;
            
            runtime = ((float)clock() / CLOCKS_PER_SEC) - currenttime;
            currenttime = ((float)clock() / CLOCKS_PER_SEC);
            route << "----------------------------" << endl;
            file << "Time: " << runtime<< "s" << endl;
            route << "Time: " << runtime << "s" << endl;

            //record value
            result_objvalue.push_back(F_best);
            result_objcost.push_back(F_best - alpha * TimeGap_best);
            result_CPU.push_back(runtime);
            if (F_best <= result_bestvalue) result_bestvalue = F_best;
            if ((F_best - alpha * TimeGap_best) <= result_bestcost) result_bestcost = F_best - alpha * TimeGap_best;
            
            iteration ++ ;
        }
        
        //calculate average
        calculate_avg( result_objvalue,result_objcost,result_CPU, &result_avgvalue, &result_avgCPU);
        result << result_avgvalue << "\t"
        << result_bestvalue <<"\t"
        << result_bestcost <<"\t"
        << result_avgCPU <<endl;
        
        file.close();
        route.close();
        num ++;
    }
    result.close();
    return 0;
}

void Move_swap(int x[], int y, int y1)
{
    int z = 0;
    z = x[y];
    x[y] = x[y1];
    x[y1] = z;
}

void Move_insert(int x[], int y, int y1)
{
    
    if (y < y1)
    {
        for (int i = y; i < y1 - 1; i++)
        {
            Move_swap(x, i, i + 1);
        }
    }
    else
    {
        for (int i = y; i > y1; i--)
        {
            Move_swap(x, i, i - 1);
        }
    }
}

void Move_reverse(int x[], int y, int y1)
{
    
    int z = (abs(y - y1) - 1) / 2;
    
    if (y < y1)
    {
        for (int i = 1; i <= z; i++)
        {
            Move_swap(x, y + i, y1 - i);
        }
    }
    else
    {
        for (int i = 1; i <= z; i++)
        {
            Move_swap(x, y1 + i, y - i);
        }
    }
}

void show(int SPr[c + p + v - 1], int SDr[c + d + v - 1], int SPv[v], int SDv[v])
{
    // Write to the file
    route << "SPr:";
    for (int j = 0; j < c + p + v - 1; j++)
    {
        route << SPr[j] << " ";
    }
    route << endl
    << "SDr:";
    for (int j = 0; j < c + d + v - 1; j++)
    {
        route << SDr[j] << " ";
    }
    route << endl
    << "SPv:";
    for (int j = 0; j < v; j++)
    {
        route << SPv[j] << " ";
    }
    route << endl
    << "SDv:";
    for (int j = 0; j < v; j++)
    {
        route << SDv[j] << " ";
    }
    route << endl;
}

void calculate_avg( vector<float> result_objvalue, vector<float>  result_objcost, vector<float>  result_CPU, float* result_avgvalue, float* result_avgCPU){
    for (int i = 0 ; i < iteration-1 ; i++){
        *result_avgvalue += result_objvalue[i];
        *result_avgCPU += result_CPU[i];
    }
    *result_avgvalue /= iteration-1;
    *result_avgCPU /= iteration-1;
}

void opendata(float xy[c+p+d][2], float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d],\
                float* p1, float d1[d][p], float* w, float* q1, float* c1)
{
//Loading the parameters

    fin.open("30N/data"+to_string(num)+".txt", ios::in);
    if (!fin)
    {
        system("pause");
        exit(1);
    }
    cout << "xy data:" << endl;
    for (int i = 0 ; i < c+p+d ; i++){
        for (int j = 0 ; j < 2 ; j++){
            fin >> xy[i][j];
            cout << xy[i][j] <<"\t";
        }
        cout <<endl;
    }

    cout << "e1: "<<endl;
    for (int i = 0 ; i < c+p+d ; i++){
        for (int j = 0 ; j < c+p+d; j++){
            e1[i][j] = sqrt(pow(abs(xy[i][0]-xy[j][0]),2)+pow(abs(xy[i][1]-xy[j][1]),2));
            t1[i][j] = e1[i][j]/2.4;
            cout << e1[i][j] << "\t";
        }
        cout <<endl;
    }
    
    cout << "t1: "<<endl;
    for (int i = 0 ; i < c+p+d ; i++){
        for (int j = 0 ; j < c+p+d; j++){
            cout << t1[i][j]<<"\t";
        }
        cout <<endl;
    }

    cout << "p1 data:" << endl;
    for (int i = 0 ; i < p ; i++){
        fin >> p1[i];
        cout << p1[i] <<"\t";
    }
    cout <<endl;

    cout << "d1 data:" << endl;
    for (int i = 0 ; i < d ; i++){
        for (int j = 0 ; j < p ; j++){
            fin >> d1[i][j];
            cout << d1[i][j] <<"\t";
        }
        cout <<endl;
    }

    cout << "w data:" << endl;
    for (int i = 0 ; i < c ; i++){
        fin >> w[i];
        cout << w[i] <<"\t";
    }
    cout <<endl;

    cout << "q1 data:" << endl;
    for (int i = 0 ; i < v ; i++){
        fin >> q1[i];
        cout << q1[i] <<"\t";
    }
    cout <<endl;

    cout << "c1 data:" << endl;
    for (int i = 0 ; i < v ; i++){
        fin >> c1[i];
        cout << c1[i] <<"\t";
    }
    cout <<endl;
    fin.close();
}
