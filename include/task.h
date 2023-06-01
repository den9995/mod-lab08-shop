#ifndef lab08_TASK_H_
#define lab08_TASK_H_

#include <utility>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>

typedef struct {
    int id;
    int cartSize;
	std::chrono::system_clock::time_point spawnTime;
} Client;

class Shop {
 private:
    int maxCheckouts;
    double spawnIntensity;
    int itemTime;
    int averageItems;
    int maxClients;
    int totalClients;
    int servedClients;
    int lostClients;
    int totalWorkTime;
    int sumServingTime;
    int sumIdlingTime;
    int sumQueueLength;
    int sumWaitingTime;
    double avgServingTime;
    double avgIdlingTime;
    double avgQueueLength;
    double avgWaitingTime;
    bool shopShouldWork;
    int spawnPeriod;
    int syncPeriod;
    std::thread spawner;	
    std::vector<std::thread> checkouts;
    std::queue<Client> clients;    
	std::mutex shopMutex;

 public:

    Shop(int maxCheckouts, double spawnIntensity, int itemTime, int averageItems, int maxClients);
    void SpawnClients();
    void OpenCheckout();
    void Start(int workTimeSec);
    void End(int workTimeSec);
    void PrintStatistics();
    void PrintActualValues();
    void PrintTheoreticalValues();    
	
   	double TheoreticalRelativeThroughput();
	double TheoreticalRejectionProbability();
	double TheoreticalAbsoluteThroughput();
	
private:	
    int Factorial(int f);
    double TheoreticalLambda();
    double TheoreticalMu();
    double TheoreticalIdlingProbability();
    
    double ActualLambda();
    double ActualMu();
    double ActualIdlingProbability();
    double ActualRejectionProbability();
	double ActualRelativeThroughput();
	double ActualAbsoluteThroughput();
};

#endif
