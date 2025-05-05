#include "head.h"
#include <iostream>
#include <time.h>
#include <vector>
#include <cmath>
using namespace std;

int num;
int node;

//function
void ini_sol(int* SPr, int* SDr, int* SPv, int* SDv, float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d] , float* p1, float d1[d][p], float* w, float* q1, float* c1){
    float TimeGap;  // Meaningless variable, just an input for cal_obj
    
    //Calculate demand
    float amount[p+d] = {};
    calamount(amount, p1, d1);
    cout << "amount:"<<endl;
    for (int i = 0 ; i < p+d ; i++){
        cout<<amount[i]<<"\t";
    }
    cout << endl;
   
    //record the four lists of the best initial solution;
    int SPrbest[c+p+v-1] = {};
    int SDrbest[c+d+v-1] = {};
    int SPvbest[v] = {};
    int SDvbest[v] = {};

    // Iteration index
    int x = 1;

    //Cost of the best initial solution
    float BestCost = 0;

    while (x<=c) {
        int count0 = 0; // dummy zero record
        int CurrentNode = 0; // current node (CD or node)
        int CurrentCD = 0; // current cross dock
        int CurrentV = 0; //current vehicle
        int CurrentLoad = 0; //current vehicle load
        int CDCapacity = 0; //current CD's remain capacity
        bool Cm[p+d] = {}; //remain node
        
        int option = 1; //option = 1: generate pickup route; option = 2: generate delivery route
        while(option <= 2){
            if (option == 1 ) {
                cout <<"--------------"<<"pickup: "<<"--------------"<<endl;
                for (int i = p ; i < p+d ; i++){
                    Cm[i] = 1;  //delivery => 1
                }
            }else{
                cout <<"--------------"<<"delivery: "<<"--------------"<<endl;
                for (int i = p ; i < p+d ; i++){
                    Cm[i] = 0; //delevery => 0
                }
            }
            bool Dm[c] = {}; //remain depot : if the CD is used, Dm = 1
            bool Vm[v] = {}; //remain vehicle
            vector<int> Sr; //set of routes
            vector<int> Sv; //vehicle priority list
            CurrentNode = x; 
            CurrentCD = CurrentNode; //assign first cross dock to first node
            Dm[CurrentCD - 1] = 1 ;  //Remove CurrentCD from Dm
            CDCapacity = w[CurrentCD - 1];
            Sr.push_back(CurrentNode);

            //randomly choose a vehicle to be the first one
            do {
                CurrentV = rand()%(v) + 1;
                cout <<"first v: " << CurrentV <<endl;
            }while (Vm[CurrentV - 1] == 1);
            Vm[CurrentV - 1] = 1;
            CurrentLoad = q1[CurrentV - 1];
            Sv.push_back(CurrentV);
            
            //select the next node
            while(checkCm(Cm) < p+d){
		//choose the closet node to be the current node
                cout << "Find the closest to " << CurrentNode << endl;
                CurrentNode = closest(CurrentNode, Cm, e1, option);
                cout << CurrentNode << " is the closest" << endl;

		// case 1 : capacity of vehicle and CD are both feasible
                if (CurrentLoad >= amount[CurrentNode-c-1] && CDCapacity >= amount[CurrentNode-c-1]){ //2 capacity okay
                    cout << "----------remain----------"<<endl;
		    //Add the current node to the route
                    Sr.push_back(CurrentNode);
                    CurrentLoad -= amount[CurrentNode-c-1];
                    CDCapacity -= amount[CurrentNode-c-1];
                    Cm[CurrentNode - c - 1] = 1;
                    cout <<CurrentNode<<" is add to the route"<<endl;

		// case 2 : capacity of vehicle is not feasible but CD is feasible => need to change vehicle
                }else if (CurrentLoad < amount[CurrentNode-c-1] && CDCapacity >= amount[CurrentNode-c-1]){ //change vehicle
                    cout << "----------change v----------"<<endl;
		//check if there is available vehicle		
                    if(checkV(Vm) >= v){
                        cout <<"no available vehicle"<<endl<<endl;
                        goto finish;
                    }
		//change the vehicle
                    changeV(Sr, Vm, &CurrentV, x, &CurrentNode, &count0, &CurrentLoad, Sv, q1);
                    cout << "vehicle is change to "<<CurrentV<<endl;
		//add the vehicle to the vehicle list, and update the load
                    Sv.push_back(CurrentV);
                    Sr.push_back(0);
                    CurrentNode = CurrentCD;
                    cout <<"start from "<<CurrentNode<<endl;
                }
		// case 3 : need to change vehicle and crossdock
                else{ 
                    cout<<"----------change v & CD----------"<<endl;
                    if(checkV(Vm) >= v || checkCD(Dm) >= c){
                        cout <<"no available vehicle or CD"<<endl<<endl;
                        goto finish;
                    }
		//add the vehicle to the vehicle list, and update the load
                    changeV(Sr, Vm, &CurrentV, x, &CurrentNode, &count0, &CurrentLoad, Sv, q1);
                    cout << "vehicle is change to "<<CurrentV<<endl;
                    Sv.push_back(CurrentV);
                    changeCD(Sr, Dm, &CurrentCD, &CDCapacity, e1, w);
		//current node became the new CD
                    CurrentNode = CurrentCD;
		//add new CD to the route, update the capacity
                    Sr.push_back(CurrentNode);
                    cout << "Current node(CD): "<<CurrentNode<<" is added to the route"<<endl;
                }
            }
            
            //General Route
            for (int i = 0 ; i < c ; i++){
                if (Dm[i] == 0) Sr.push_back(i+1);
            }
            for (int i = count0 ; i < dummy ; i++){
                Sr.push_back(0);
            }
            for (int i = 0 ; i < v ; i++){
                if (Vm[i] == 0) Sv.push_back(i+1);
            }
            
            //Pickup and Delivery Route
            if (option == 1){
                for (int i = 0 ; i < c+p+v-1 ; i++){
                    SPr[i] = Sr[i];
                }
                for (int i = 0 ; i < v ; i++){
                    SPv[i] = Sv[i];
                }
            }else if (option == 2){
                for (int i = 0 ; i < c+d+v-1 ; i++){
                    SDr[i] = Sr[i];
                }
                for (int i = 0 ; i < v ; i++){
                    SDv[i] = Sv[i];
                }
            }
            option++;
        }//until option > 2
        
	// print the generated solution
        cout << "[ "<<x <<" ]"<<" initial sol: "<<endl;
        cout << "pickup route: " <<endl;
        for (int i = 0 ; i < c+p+v-1 ; i++){
            cout<<SPr[i] <<"\t";
        }
        cout <<endl;
        cout << "pickup vehicle: " <<endl;
        for (int i = 0 ; i < v ; i++){
            cout<<SPv[i] <<"\t";
        }
        cout <<endl;
        cout << "delivery route: " <<endl;
        for (int i = 0 ; i < c+d+v-1 ; i++){
            cout<<SDr[i] <<"\t";
        }
        cout <<endl;
        cout << "delivery vehicle: " <<endl;
        for (int i = 0 ; i < v ; i++){
            cout<<SDv[i] <<"\t";
        }
        cout <<endl<<endl;
        
        //Obj function
        if (x == 1)
        {
            recordBest(SPrbest, SDrbest, SPvbest, SDvbest, SPr, SDr, SPv, SDv);
            float CurrentObj = cal_obj(SPr, SDr, SPv, SDv, e1, t1 ,p1, d1, w, q1, c1, &TimeGap);
            if(CurrentObj < 0) BestCost = infin;
            else BestCost = CurrentObj;
        }
        else if(cal_obj(SPr, SDr, SPv, SDv, e1, t1 ,p1, d1, w, q1, c1, &TimeGap)<BestCost){
            recordBest(SPrbest, SDrbest, SPvbest, SDvbest, SPr, SDr, SPv, SDv);
        }
        
        finish:
        x++;
    }//until x>|D|=c
    bestsol(SPrbest, SDrbest, SPvbest, SDvbest, SPr,  SDr, SPv, SDv);
}

void bestsol(int* SPrbest, int* SDrbest, int* SPvbest, int* SDvbest, int* SPr,  int* SDr, int* SPv, int* SDv){
// put the best initial solution in the array
    for (int i = 0 ; i < c+p+v-1 ; i++){
        SPr[i] = SPrbest[i];
    }
    for (int i = 0 ; i < c+d+v-1 ; i++){
        SDr[i] = SDrbest[i];
    }
    for (int i = 0 ; i < v ; i++){
        SPv[i] = SPvbest[i];
        SDv[i] = SDvbest[i];
    }
    
}

void recordBest(int* SPrbest, int* SDrbest, int* SPvbest, int* SDvbest, int* SPr,  int* SDr, int* SPv, int* SDv){
//record the best intial solution
    for (int i = 0 ; i < c+p+v-1 ; i++){
        SPrbest[i] = SPr[i];
    }
    for (int i = 0 ; i < c+d+v-1 ; i++){
        SDrbest[i] = SDr[i];
    }
    for (int i = 0 ; i < v ; i++){
        SPvbest[i] = SPv[i];
        SDvbest[i] = SDv[i];
    }
    
}

void calamount(float amount[p+d], float p1[p], float d1[d][p]){
//calculate the total demand of a demand customer
    for (int i = 0; i<p ; i++){
        amount[i] = p1[i];
    }
    for (int i = p; i< p+d ; i++){
        int sum = 0;
        for (int j = 0; j<p ; j++){
            sum += d1[i-p][j];
        }
        amount[i] = sum;
    }
}

int checkV(bool Vm[v]){
//check if there is an available vehicle to use
    int sum = 0;
    for (int j = 0 ; j < v ; j++){
        sum += Vm[j];
    }
    return sum;
}

void changeV(vector<int> Sr, bool Vm[v], int *CurrentV, int i, int *CurrentNode, int *count0, int *CurrentLoad, vector<int> Sv, float q1[v]){
//choose a random vehicle to use   
 int choice = 0;
    do {
        choice = rand()%(v) + 1;
    }while (Vm[choice - 1] == 1);
    *CurrentV = choice;
    Vm[*CurrentV - 1] = 1;
    *CurrentLoad = q1[*CurrentV - 1];
}

int checkCD(bool Dm[c]){
//check if there is an available CD to use
    int sum = 0;
    for (int j = 0 ; j < c ; j++){
        sum += Dm[j];
    }
    return sum;
}

void changeCD(vector<int> Sr,bool Dm[c], int *CurrentCD, int *CDCapacity, float e1[c+p+d][c+p+d], float w[c]){
//choose the closest CD to the current CD to use   
    int best = 0;
    for (int i = 0 ; i < c ; i++){
        if (Dm[i] == 0 ) best = i;
    }
    for (int i = 0 ; i < c ; i++){
        if (Dm[i] == 1 ) continue;;
        if (e1[*CurrentCD-1][i] < e1[*CurrentCD-1][best]){
            best = i;
        }
    }
    *CurrentCD = best + 1;
    Sr.push_back(*CurrentCD);
    Dm[*CurrentCD-1] = 1 ;
    *CDCapacity = w[*CurrentCD-1];
}

int checkCm(bool Cm[p+d]){
    int sum = 0;
    for (int i = 0 ; i < p+d ; i++){
        sum += Cm[i];
    }
    return sum;
}

int closest (int CurrentNode, bool Cm[p+d], float e1[c+p+d][c+p+d], int option){
//choose the closet node to the current one
    int best = 0;
    //pickup
    if (option == 1 ){
        for (int i = c ; i < c+p ; i++){
            if (Cm[i-c]==0) best = i;
        }
        for (int i = c ; i < c+p ; i++){
            if (Cm[i-c]==1) continue;
            if (e1[CurrentNode-1][i] < e1[CurrentNode-1][best]) best = i;
        }

    //delivery
    }else if (option == 2 ){
        for (int i = c+p ; i < c+p+d ; i++){
            if (Cm[i-c]==0) best = i;
        }
        for (int i = c+p ; i < c+p+d ; i++){
            if (Cm[i-c]==1) continue;
            if (e1[CurrentNode-1][i] < e1[CurrentNode-1][best]) best = i;
        }
    }
    return best+1;
}

float cal_obj(int* SPr, int* SDr, int* SPv, int* SDv, float e1[c+p+d][c+p+d], float t1[c+p+d][c+p+d] , float* p1, float d1[d][p], float* w, float* q1, float* c1, float* TimeGap){
//Calculate the objectvie value of solution by reading the four lists
    
    const int infin = 99999;
    const float infeasible = -1;
    //There are 4 cases of infeasibility. The illustration will be next to the "infeasible" code.

    /*Initialization*/
    float TotalCost = 0;                    // Objective value if feasible
    int PickupNodeToCD[p] = {0};            // Assignment of pickup nodes to CD
    float ProductNeedCD[c][p] = {0};          // Amount of Products needed by CD from the pickup nodes
    float FinishPickup[p] = {0};              // Time of finishing delivery process at CD
    float StartTimeCD[p] = {0};              // Starting time of the cross-docking activity at CD
    float FinishDelivery[p] = {0};           // Time of finishing delivery process at CD

    /*Pickup*/
    float MaxTime = -infin, MinTime = infin;                   // For calculation of time gap
    int top1 = 0, top2 = 0;                                    // Top of stack SPr(SDr), SPv(SDv)
    float CurrentLoad = 0;                                     // Cargoes loaded on current vehicle
    float CurrentTime = 0;                                     // Time of returning to CD
    float TotalPickupCD = 0;                                   // Total pickup of CurrentCD
    int Origin = SPr[top1]; top1 ++;                           // Origin
    int Destination;                                           // Destination
    int Vehicle = SPv[top2]; top2 ++;                          // Vehicle
    int CurrentCD = Origin;                                    // CurrentCD
    int CM = 0;                                                // Number of routed pickup nodes
    
    // Infeasibility case 1: the first node in a list is not a cross-dock.
    if(Origin > c || Origin == 0) return infeasible;

    while(CM < p)  //The reading process stops when all pickup nodes are read.
    {
        Destination = SPr[top1]; top1 ++;
        if(Destination <= c) // Situations of closing the route, 0 or cross-dock
        {
            // close the route
            TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
            CurrentTime += t1[Origin-1][CurrentCD-1];
            FinishPickup[CurrentCD-1] = max(FinishPickup[CurrentCD-1],CurrentTime);
            if(CurrentTime != 0 && CurrentTime < MinTime) MinTime = CurrentTime;
            if(CurrentTime != 0 && CurrentTime > MaxTime) MaxTime = CurrentTime;

            //Initialization for next route
            if(CurrentLoad != 0) //Vehicle is changed only if the current vehicle is used.
            {
                if(top2 < v)
                {
                    Vehicle = SPv[top2]; top2 ++;
                }
                else return infeasible;  //Infeasibility case 2: run out of vehicles
            }
            CurrentLoad = 0; CurrentTime = 0;
            if(Destination != 0)  //Situation 3 of closing the route: reach 0.
            {
                CurrentCD = Destination;
                Origin = CurrentCD;
                TotalPickupCD = 0;
            }
            else                 //Situation 1 of closing the route: reach cross-dock
            {
                Origin = CurrentCD;
            }
        }
        else
        {
            CurrentLoad += p1[Destination-c-1];
            TotalPickupCD += p1[Destination-c-1];

            if(TotalPickupCD > w[CurrentCD-1]) return infeasible; //Infeasibility case 3: exceeding the capacity of a cross-dock
            else if(CurrentLoad <= q1[Vehicle-1])
            {
                TotalCost += c1[Vehicle-1] * e1[Origin-1][Destination-1];
                CurrentTime += t1[Origin-1][Destination-1];
                PickupNodeToCD[Destination-c-1] = CurrentCD;
                CM ++;
                Origin = Destination;
            }
            else //Situation 2 of closing the route: exceeding vehicle capacity
            {
                // close the route
                TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
                CurrentTime += t1[Origin-1][CurrentCD-1];
                FinishPickup[CurrentCD-1] = max(FinishPickup[CurrentCD-1],CurrentTime);
                if(CurrentTime != 0 && CurrentTime < MinTime) MinTime = CurrentTime;
                if(CurrentTime != 0 && CurrentTime > MaxTime) MaxTime = CurrentTime;

                if(CurrentLoad != 0) //Vehicle is changed only if the current vehicle is used.
                {
                    if(top2 < v)
                    {
                        Vehicle = SPv[top2]; top2 ++;
                    }
                    else return infeasible; //Infeasibility case 2: run out of vehicles
                }
                CurrentLoad = 0; CurrentTime = 0;
                Origin = CurrentCD;
                TotalCost += c1[Vehicle-1] * e1[Origin-1][Destination-1];
                CurrentTime += t1[Origin-1][Destination-1];
                PickupNodeToCD[Destination-c-1] = CurrentCD;
                CM ++;
                Origin = Destination;
            }
        }
    }
    // Close the last one route
    TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
    CurrentTime += t1[Origin-1][CurrentCD-1];
    FinishPickup[CurrentCD-1] = max(FinishPickup[CurrentCD-1],CurrentTime);
    if(CurrentTime < MinTime) MinTime = CurrentTime;
    if(CurrentTime > MaxTime) MaxTime = CurrentTime;

    /*Return Time Gap Penalty*/
    *TimeGap = MaxTime-MinTime;
    TotalCost += alpha * (MaxTime-MinTime);

    /*Delivery*/
    top1 = 0; top2 = 0;
    CurrentLoad = 0;
    CurrentTime = 0;
    int TotalDeliveryCD = 0; // Total delivery of CurrentCD
    Origin = SDr[top1]; top1 ++;
    Vehicle = SDv[top2]; top2 ++;
    CurrentCD = Origin;
    CM = 0;

    // Infeasibility case 1: the first node in a list is not a cross-dock.
    if(Origin > c || Origin == 0) return infeasible;

    while(CM < d)
    {
        Destination = SDr[top1]; top1 ++;
        if(Destination <= c) // Situations of closing the route, 0 or cross-dock
        {
            //Close the route
            TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
            CurrentTime += t1[Origin-1][CurrentCD-1];
            FinishDelivery[CurrentCD-1] = max(FinishDelivery[CurrentCD-1],CurrentTime);

            //Initialization for next route
            if(CurrentLoad != 0) //Vehicle is changed only if the current vehicle is used.
            {
                if(top2 < v)
                {
                    Vehicle = SDv[top2]; top2 ++;
                }
                else return infeasible; //Infeasibility case 2: run out of vehicles
            }
            CurrentLoad = 0; CurrentTime = 0;
            if(Destination != 0)  //Situation 3 of closing the route: reach 0.
            {
                CurrentCD = Destination;
                Origin = CurrentCD;
                TotalDeliveryCD = 0;
            }
            else                  //Situation 1 of closing the route: reach cross-dock
            {
                Origin = CurrentCD;
            }
        }
        else
        {
            for (int PickupNode = 0; PickupNode < p; PickupNode ++)
            {
                int Demand = d1[Destination-c-p-1][PickupNode];
                ProductNeedCD[CurrentCD-1][PickupNode] += Demand;
                CurrentLoad += Demand;
                TotalDeliveryCD += Demand;
            }

            if(TotalDeliveryCD > w[CurrentCD-1]) return infeasible; //Infeasibility case 3: exceeding the capacity of a cross-dock
            else if(CurrentLoad <= q1[Vehicle-1])
            {
                TotalCost += c1[Vehicle-1] * e1[Origin-1][Destination-1];
                CurrentTime += t1[Origin-1][Destination-1];
                CM ++;
                Origin = Destination;
            }
            else //Situation 2 of closing the route: exceeding vehicle capacity
            {
                // Close the route
                TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
                CurrentTime += t1[Origin-1][CurrentCD-1];
                FinishDelivery[CurrentCD-1] = max(FinishDelivery[CurrentCD-1],CurrentTime);
                
                if(CurrentLoad != 0) //Vehicle is changed only if the current vehicle is used.
                {
                    if(top2 < v)
                    {
                        Vehicle = SDv[top2]; top2 ++;
                    }
                    else return infeasible; //Infeasibility case 2: run out of vehicles
                }
                CurrentLoad = 0; CurrentTime = 0;
                Origin = CurrentCD;
                TotalCost += c1[Vehicle-1] * e1[Origin-1][Destination-1];
                CurrentTime += t1[Origin-1][Destination-1];
                CM ++;
                Origin = Destination;
            }
        }
    }
    // Close the last one route
    TotalCost += c1[Vehicle-1] * e1[Origin-1][CurrentCD-1];
    CurrentTime += t1[Origin-1][CurrentCD-1];
    FinishDelivery[CurrentCD-1] = max(FinishDelivery[CurrentCD-1],CurrentTime);
    
    /*Transfer*/
    //Calculation of total amount of transfer between cross-docks
    float Transfer[c][c] = {0};
    for(int PickupNode = 0; PickupNode < p; PickupNode ++)
    {
        int CDofProduct = PickupNodeToCD[PickupNode];
        for(int CD = 0; CD < c; CD++)
        {
            Transfer[CDofProduct-1][CD] += ProductNeedCD[CD][PickupNode];
        }
    }
    //Calculation of transfer cost
    for(int CD1 = 0; CD1 < c; CD1++)
    {
        StartTimeCD[CD1] = FinishPickup[CD1];
        for(int CD2 = 0; CD2 < c; CD2++)
        {
            if (Transfer[CD2][CD1] != 0)
            {
                TotalCost += e1[CD2][CD1] * c2 * ceil(Transfer[CD2][CD1]/q2);
                StartTimeCD[CD1] = max(StartTimeCD[CD1], FinishPickup[CD2] + t1[CD2][CD1]);
            }
        }
        //Infeasibility case 4: total activity time exceeds the time horizon
        if(StartTimeCD[CD1] + Tcd + FinishDelivery[CD1] > Tmax) return infeasible;
    }
    
    //To this point, the solution must be feasible, so return the objective value
    return TotalCost;
}



