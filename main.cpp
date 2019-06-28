#include <iostream>
#include <fstream>
#include "PackageDeliveryFunctions.h"
using namespace std;
int times = 100;

struct OrderRecord
{
	int start;
	int dest;
	int comindex;
	int amount;
	int emergency;
	double time;
};

int main()
{
	init();

	double com[125] = { 0 };
	double a, price[125] = { 0 };
	char s[25];
	ifstream fin("Commodities.txt");
	if (!fin) { cout << "cannot open file\n"; return false; }
	for (int i = 1; i <= 123; i++)
	{
		fin >> a >> price[i] >> s >> com[i];
		//cout<<a<<" "<<price<<" "<<s<<" "<<com[i]<<endl;
	}

	ifstream in("Orders.txt");
	if (!in) { cout << "cannot open file\n"; return false; }
	char time[10];
	OrderRecord Orders;

	for (int i = 0; i < times; i++)
    {
        cout<<i<<endl;
        in >> Orders.start >> Orders.dest >> time >> Orders.comindex >> Orders.amount >> Orders.emergency;
        Orders.time=timechange(time);
        PathForOrder(Orders.start, Orders.dest, Orders.time, com[Orders.comindex], price[Orders.comindex], Orders.amount); //依次输入初始城市，目标城市，换算成分钟的时间，单个重量，数量
    }
    ofstream out("Result1Evaluate.txt");
    out<<"Average Calculate Cost: "<<TotalCost24w/(times+0.0)<<endl<<"Average Transport Cost: "<<TotalTransportCost24w/(times+0.0)<<endl<<"Average Wait Time: "<<TotalWaitTime24w/(times+0.0);
    out.close();
	//choose_hub(25);
	return 0;
}
