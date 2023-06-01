
#include "task.h"
#include <thread> 

int main() {
	int workTimeSec = 5;
	int maxCheckouts = 3 ;
	double spawnIntensity = 3;
	int itemTimeMs = 100;
	int averageItems = 10;
	int maxClients = 12;
    Shop shop = Shop(maxCheckouts, spawnIntensity, itemTimeMs, averageItems, maxClients);
    shop.Start(workTimeSec);
	
    shop.PrintStatistics();
    shop.PrintActualValues();
    shop.PrintTheoreticalValues();
    return 0;
}