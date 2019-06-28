#ifndef PACKAGEDELIVERYFUNCTIONS_H_INCLUDED
#define PACKAGEDELIVERYFUNCTIONS_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

int C1 = 1, C2 = 1, Common = 1500;
int inf = 9999999;
double TotalCost24w=0,TotalWaitTime24w=0,TotalTransportCost24w=0;

struct City
{
	int cityindex;
	int incomenum;
	int outcomenum;
	double outweight;
	int outamount;
	double totalweight;
	double totaltranscost;
	double avrgweight;
	double avrgtranscost;
	int planecount;
	int traincount;
	int shipcount;
	int truckcount;
	double maxtoolpro;
	bool ifhub;
};
struct CostRecord
{
	double waittime;
	double trancost;
	double totalcost;
};
struct TransRecord
{
	int valid;
	int departurecityindex;
	int arrivalcityindex;
	double delay;
	double speed;
	double departuretime;
	double arrivaltime;
	double costunit;
	double distance;
	char type[6];
	double cost;
	double waittime;
	double trancost;
};
vector<TransRecord> trans[658][658];
int distancematrix[660][660] = { 0 };
City city[660];


CostRecord CalCost(double OrderTime, double ArrivalTime, double weight, int amount, double distance, double costunit, double price)
{
	CostRecord record;
	double TimeDelay = ArrivalTime - OrderTime;
	//cout<<"Cost: "<<costunit<<" "<<TimeDelay<<" "<<weight*amount*distance*costunit/50.0<<endl;
	double moneycost = weight * amount * distance * costunit / 50.0;
	double TotalCost = C1 * TimeDelay * amount + C2  * moneycost;
	record.totalcost = TotalCost;
	record.trancost = weight * amount * distance * costunit / 50.0;
	record.waittime = TimeDelay;
	return record;
}

double timechange(char s[10])
{
	int num0, num1, num2, num3, num4, num5;
	if (s[1] == ':')
	{
		num0 = 0;
		num1 = s[0] - '0';
		num2 = s[2] - '0';
		num3 = s[3] - '0';
		num4 = s[5] - '0';
		num5 = s[6] - '0';
	}
	else
	{
		num0 = s[0] - '0';
		num1 = s[1] - '0';
		num2 = s[3] - '0';
		num3 = s[4] - '0';
		num4 = s[6] - '0';
		num5 = s[7] - '0';
	}
	return (10 * num0 + num1) * 60 + (10 * num2 + num3) + (10 * num4 + num5) / 60.0;
}

void arrivetime(double arrivaltime)
{
    int day,hour,minute;
    ofstream out("Result1.txt", ios::out | ios::app);
    day=arrivaltime/1440;
    hour=(arrivaltime-day*1440)/60;
    minute=arrivaltime-day*1440-hour*60;
    out<<"Arrive time: next "<<day<<" day "<<hour<<":"<<minute<<endl;
    out.close();
}

bool init()
{
	int rowcount = 0;
	int departurecity, arrivalcity;
	double speed, dis;
	double delay, costunit;
	char dtime[10];

	for (int i = 0; i < 660; i++)
	{
		city[i].cityindex = i;
		city[i].ifhub = 0;
		city[i].incomenum = 0;
		city[i].outcomenum = 0;
		city[i].outweight = 0;
	    city[i].outamount=0;
	    city[i].totalweight=0;
	    city[i].totaltranscost=0;
	    city[i].avrgweight=0;
	    city[i].avrgtranscost=0;
	    city[i].planecount=0;
	    city[i].traincount=0;
	    city[i].shipcount=0;
	    city[i].truckcount=0;
	    city[i].maxtoolpro=0;
	};

	ifstream in("DistanceMatrix.txt");
	if (!in) { cout << "cannot open file\n"; return false; }
	for (int i = 0; i < 656; i++)
		for (int j = 0; j < 656; j++)
		{
			in >> dis;
			distancematrix[i][j] = dis;
		}

	//cout<<distancematrix[1][0]<<" "<<distancematrix[654][613]<<" "<<distancematrix[613][592]<<" "<<distancematrix[592][9]<<endl;

	TransRecord tmp;
	ifstream fin("Trans.txt");
	if (!fin) { cout << "cannot open file\n"; return false; }

	while (fin >> departurecity)
	{
		fin >> arrivalcity >> delay >> speed >> costunit >> dtime;
		tmp.valid = 0;
		tmp.departurecityindex = departurecity;
		tmp.arrivalcityindex = arrivalcity;
		tmp.delay = delay;
		tmp.departuretime = timechange(dtime);
		tmp.distance = distancematrix[departurecity - 1][arrivalcity - 1];
		tmp.arrivaltime = tmp.departuretime + tmp.delay + tmp.distance / speed * 60;
		tmp.costunit = costunit;
		tmp.speed = speed;

		if (rowcount < 9948) strcpy(tmp.type, "plane");
		else if (rowcount < 13039) strcpy(tmp.type, "ship");
		else if (rowcount < 54419) strcpy(tmp.type, "train");
		else strcpy(tmp.type, "truck");
		rowcount++;

		trans[departurecity][arrivalcity].push_back(tmp);
	}
	return true;
}

int iterationcount=0;

TransRecord privateSearchForBestTrans(int startcityindex, int destcityindex, double ordertime, double weight, int amount, double price)
{
    TransRecord record;
	record.valid = 0;
	int departurecity, arrivalcity;
	double tmpcost = inf, speed;
	double arrivaltime, transtime, costunit, departuretime, distance;

	for (int i = 0; i < trans[startcityindex][destcityindex].size(); i++)
	{
		distance = trans[startcityindex][destcityindex][i].distance;
		departurecity = startcityindex;
		arrivalcity = trans[startcityindex][destcityindex][i].arrivalcityindex;
		speed = trans[startcityindex][destcityindex][i].speed;
		transtime = trans[startcityindex][destcityindex][i].delay + distance / speed * 60;
		costunit = trans[startcityindex][destcityindex][i].costunit;
		departuretime = trans[startcityindex][destcityindex][i].departuretime;
		arrivaltime = trans[startcityindex][destcityindex][i].arrivaltime;

		if (departuretime > ordertime&&CalCost(ordertime, arrivaltime, weight, amount, distance, costunit, price).totalcost < tmpcost)
		{
			tmpcost = CalCost(ordertime, arrivaltime, weight, amount, distance, costunit, price).totalcost;
			record.trancost = CalCost(ordertime, arrivaltime, weight, amount, distance, costunit, price).trancost;
			record.waittime = CalCost(ordertime, arrivaltime, weight, amount, distance, costunit, price).waittime;
			record.valid = 1;
			record.arrivaltime = arrivaltime;
			record.cost = tmpcost;
			record.departuretime = departuretime;
			record.arrivaltime = arrivaltime;
			record.distance = distance;
			strcpy(record.type, trans[startcityindex][destcityindex][i].type);
			record.costunit = costunit;
		}
	}
	return record;
}

TransRecord SearchForBestTrans(int startcityindex, int destcityindex, double ordertime, double weight, int amount, double price)
{
    TransRecord tmp1,tmp2;
    tmp1=privateSearchForBestTrans(startcityindex, destcityindex, ordertime, weight, amount, price);
    tmp2=privateSearchForBestTrans(startcityindex, destcityindex, ordertime-1440, weight, amount, price);
    if(tmp1.valid==1) return tmp1;
    return tmp2;
}

void printpath(int sNo, int dNo, int prev[], char type[][6])
{
	ofstream out("Result1.txt", ios::out | ios::app);
	if (sNo == dNo)
	{
		//cout << sNo;
		out << "Initial city:" << sNo;
		return;
	}
	printpath(sNo, prev[dNo], prev, type);
	//cout << " " << type[dNo] << " " << dNo;//显示到达此处的交通工具
	out << " " << "transport through" << " " << type[dNo] << " " << "arrive at city" << " " << dNo;
	out.close();
}

int PathForOrder(int start, int dest, double ordertime, double weight, double price, int amount)
{
	int mycount = 0;
	CostRecord cost[657];
	int known[657] = { 0 }, prev[657] = { 0 }, orderupdate[657];
	for (int i = 0; i < 657; i++)
	{
		cost[i].totalcost = inf; cost[i].waittime = 0; cost[i].trancost = 0;
	}
	cost[start].totalcost = 0;
	prev[start] = start;
	int tmp;
	for (int i = 0; i < 657; i++) orderupdate[i] = ordertime;
	char typerecord[657][6];

	while (!known[dest] && mycount++ < 700)
	{
		int mincost = inf;
		TransRecord tmprecord;

		//find the minist with known[i]=0
		for (int i = 0; i < 657; i++)
		{
			if (known[i] == 0)
			{
				if (cost[i].totalcost < mincost)
				{
					tmp = i;
					mincost = cost[i].totalcost;
				}
			}
		}
		known[tmp] = 1;

		//update
		for (int i = 1; i < 657; i++)
		{
			if (i == tmp) continue;
			tmprecord = SearchForBestTrans(tmp, i, orderupdate[tmp], weight, amount, price);
			if (known[i] == 0 && cost[tmp].totalcost + tmprecord.cost < cost[i].totalcost&&tmprecord.valid==1)
			{
				cost[i].totalcost = cost[tmp].totalcost + tmprecord.cost;
				cost[i].trancost = cost[tmp].trancost + tmprecord.trancost;
				cost[i].waittime = cost[tmp].waittime + tmprecord.waittime;
				prev[i] = tmp;
				orderupdate[i] = tmprecord.arrivaltime;
				strcpy(typerecord[i], tmprecord.type);
			}
		}
	}

	if (mycount >= 700)
	{
		//cout << "no path!check the following day" << endl;
		return 1;
	}

	//sum up the times of in and out each city
	for (int i = dest; i != start; i = prev[i])
	{
		if (i == dest)
        {
            city[i].incomenum++;
			if(!strcmp(typerecord[i],"plane"))
               {
                  city[prev[i]].planecount+=amount;
                  city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*2.0/50;
               }
            if(!strcmp(typerecord[i],"ship"))
               {
                   city[prev[i]].shipcount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.45/50;
               }
            if(!strcmp(typerecord[i],"train"))
               {
                   city[prev[i]].traincount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.90/50;
               }
            if(!strcmp(typerecord[i],"truck"))
               {
                   city[prev[i]].truckcount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.35/50;
                   //cout<<"test"<<endl;
               }
        }

		else
        {
			city[i].incomenum++;
			city[i].outcomenum++;
			city[i].outweight+=weight*amount;
			city[i].outamount+=amount;
			if(!strcmp(typerecord[i],"plane"))
               {
                  city[prev[i]].planecount+=amount;
                  city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*2.0/50;
               }
            if(!strcmp(typerecord[i],"ship"))
               {
                   city[prev[i]].shipcount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.45/50;
               }
            if(!strcmp(typerecord[i],"train"))
               {
                   city[prev[i]].traincount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.90/50;
               }
            if(!strcmp(typerecord[i],"truck"))
               {
                   city[prev[i]].truckcount+=amount;
                   city[prev[i]].totaltranscost+=distancematrix[i][prev[i]]*weight*amount*0.35/50;
               }
		}
	};
	city[start].outcomenum++;
    city[start].outweight+=weight*amount;
    city[start].outamount+=amount;

    TotalCost24w+=cost[dest].totalcost;
    TotalTransportCost24w+=cost[dest].trancost;
    TotalWaitTime24w+=cost[dest].waittime;

    arrivetime(ordertime+cost[dest].waittime);
	//cout << "totalcost: " << cost[dest].totalcost << " trancost: " << cost[dest].trancost << " waittime: " << cost[dest].waittime << endl;
	ofstream out("Result1.txt", ios::out | ios::app);
	//cout << "totalcost: " << cost[dest].totalcost << " trancost: " << cost[dest].trancost << " waittime: " << cost[dest].waittime << endl;
	out << "calculatecost: " << cost[dest].totalcost << " trancost: " << cost[dest].trancost << " waittime: " << cost[dest].waittime << endl;
	printpath(start, dest, prev, typerecord); //cout << endl << endl;
	out<<endl<<endl;
	out.close();
	return 0;
}

void choose_hub(int hubnum)
{
	int min_outcome = 1000000;
	int min_one;
	int num = 0;
	int* index = new int[hubnum];

	for(int i=0;i<656;i++)
    {
        if(city[i].outamount>=1&&city[i].outweight>=1)
        {
           city[i].avrgtranscost=city[i].totaltranscost/city[i].outweight;
           city[i].avrgweight=city[i].outweight/city[i].outamount;
           city[i].maxtoolpro=max(max(city[i].planecount,city[i].shipcount),max(city[i].traincount,city[i].truckcount))/(city[i].outamount+0.0);
        }
    }

    ofstream out1("240000order-outamount.txt");
    if(!out1) {cout<<"create file fail";return;}
    for(int i=0;i<656;i++) out1<<city[i].outamount<<" ";
    out1.close();

    ofstream out2("240000order-avrgweight.txt");
    if(!out2) {cout<<"create file fail";return;}
    for(int i=0;i<656;i++) out2<<city[i].avrgweight<<" ";
    out2.close();

    ofstream out3("240000order-avrgtranscost.txt");
    if(!out3) {cout<<"create file fail";return;}
    for(int i=0;i<656;i++) out3<<city[i].avrgtranscost<<" ";
    out3.close();

    ofstream out4("240000order-maxtoolpro.txt");
    if(!out4) {cout<<"create file fail";return;}
    for(int i=0;i<656;i++) out4<<city[i].maxtoolpro<<" ";
    out4.close();

	for (int i = 0; i < 660; i++)
	{
		if (num == hubnum) {
			for (int j = 0; j < hubnum; j++) {
				if (city[index[j]].outcomenum < min_outcome) {
					min_one = j;
					min_outcome = city[index[j]].outcomenum;
				}
			};
			if (city[i].outcomenum > min_outcome) {
				index[min_one] = i;
				min_outcome = city[i].outcomenum;
			}
		}
		else index[num++] = i;
	};

	for (int i = 0; i < hubnum; i++) {
		city[index[i]].ifhub = 1;
		//cout << "hub" << i << "->" << index[i] << " outcomenum: "<<city[index[i]].outcomenum<<endl;
	};
}

#endif // PACKAGEDELIVERYFUNCTIONS_H_INCLUDED
