#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include "task.h"

Shop::Shop(int maxCheckouts, double spawnIntensity, int itemTime, int averageItems, int maxClients) :
    maxCheckouts (maxCheckouts), spawnIntensity (spawnIntensity),
    itemTime (itemTime), averageItems (averageItems),
    maxClients (maxClients), shopShouldWork (true),
    spawnPeriod ((int)(1 / spawnIntensity * 1000)),
    syncPeriod (50), totalClients (0), servedClients (0),
    lostClients (0), totalWorkTime (0), sumServingTime (0),
    sumIdlingTime (0), sumQueueLength (0), sumWaitingTime (0),
    avgServingTime (0), avgIdlingTime (0),avgQueueLength (0),
    avgWaitingTime (0) {}

void Shop::SpawnClients() {
    while (shopShouldWork) {
        std::this_thread::sleep_for(std::chrono::milliseconds(spawnPeriod));
        Client client;
        client.id = totalClients;
        totalClients++;
        
        if (clients.size() < maxClients) {
			int cartSize = 1 + (std::rand() % (int)(averageItems * 2));
        	client.cartSize = cartSize;
			client.spawnTime = std::chrono::system_clock::now();
            clients.push(client);            
            shopMutex.lock();
            std::cout << "Spawned client " << client.id << " with " << client.cartSize << " items" << std::endl;
            shopMutex.unlock();
        } else {
            shopMutex.lock();
            std::cout << "Client " << client.id << " left" << std::endl;
            shopMutex.unlock();
        }
    }
}

void Shop::OpenCheckout() {
    while (shopShouldWork) {
        shopMutex.lock();
        if (!clients.empty()) {
            Client current = clients.front();
            int clientID = current.id;
			std::chrono::system_clock::time_point spawnTime = current.spawnTime;
			int servingTime = itemTime * current.cartSize;
            std::cout << "Serving client " << current.id << " for " << servingTime << "ms... Clients waiting: " << clients.size() << std::endl;
			clients.pop();
			sumServingTime += servingTime;
            servedClients++;
            shopMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(servingTime));
            shopMutex.lock();
            std::chrono::duration waitingTime = std::chrono::system_clock::now() - spawnTime;
            sumWaitingTime += waitingTime.count();
            sumQueueLength += clients.size();
        } else {
            sumIdlingTime += syncPeriod;
        }
        shopMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(syncPeriod));
    }
}

void Shop::Start(int workTimeSec) {
    for (int i = 0; i < maxCheckouts; i++) {
        checkouts.push_back(std::thread([&] {OpenCheckout();}));
    }
	spawner = std::thread([&]{SpawnClients();});
	std::this_thread::sleep_for(std::chrono::seconds(workTimeSec));
    End(workTimeSec);
}

void Shop::End(int workTimeSec) {
    shopShouldWork = false;
    lostClients = totalClients - servedClients;
    totalWorkTime = workTimeSec * 1000;
    avgQueueLength = (double)(sumQueueLength) / servedClients;
    avgWaitingTime = (double)(sumWaitingTime) / (servedClients * 1000000);
    avgServingTime = (double)(sumServingTime) / (maxCheckouts * 1000);
    avgIdlingTime  = (double)(sumIdlingTime)  / (maxCheckouts * 1000);
	for (int i = 0; i < maxCheckouts; i++) {
        checkouts[i].join();
    }
	spawner.join();
}

void Shop::PrintStatistics() {
    std::cout << std::endl << "---------- STATISTICS ----------" << std::endl;
    std::cout << "Shop worked for " << (double)(totalWorkTime) / 1000 << " sec" << std::endl;
    std::cout << "Served clients: " << servedClients << std::endl;
    std::cout << "Lost clients: " << lostClients << std::endl;
    std::cout << "Average queue length: " << avgQueueLength << std::endl;
    std::cout << "Average waiting time: " << avgWaitingTime << " sec/client" << std::endl;
    std::cout << "Average serving time: " << avgServingTime << " sec/checkout" << std::endl;
    std::cout << "Average idling time: "  << avgIdlingTime  << " sec/checkout" << std::endl;
}

void Shop::PrintActualValues() {
    std::cout << std::endl << "---------- ACTUAL VALUES ----------" << std::endl;
    std::cout << "Actual Lambda: " << ActualLambda() << std::endl;
    std::cout << "Actual Mu: " << ActualMu() << std::endl;
    std::cout << "Actual Idling Probability: " << ActualIdlingProbability() << std::endl;
    std::cout << "Actual Rejection Probability: " << ActualRejectionProbability() << std::endl;
    std::cout << "Actual Relative Throughput: " << ActualRelativeThroughput() << std::endl;
    std::cout << "Actual Absolute Throughput: " << ActualAbsoluteThroughput() << std::endl;
}

void Shop::PrintTheoreticalValues() {
    std::cout << std::endl << "---------- THEORETICAL VALUES ----------" << std::endl;
    std::cout << "Theoretical Lambda: " << TheoreticalLambda() << std::endl;
    std::cout << "Theoretical Mu: " << TheoreticalMu() << std::endl;
    std::cout << "Theoretical Idling Probability: " << TheoreticalIdlingProbability() << std::endl;
    std::cout << "Theoretical Rejection Probability: " << TheoreticalRejectionProbability() << std::endl;
    std::cout << "Theoretical Relative Throughput: " << TheoreticalRelativeThroughput() << std::endl;
    std::cout << "Theoretical Absolute Throughput: " << TheoreticalAbsoluteThroughput() << std::endl;
}

double Shop::TheoreticalRelativeThroughput() {
    return 1.0 - TheoreticalRejectionProbability();
}
double Shop::ActualRelativeThroughput() {
    return 1.0 - ActualRejectionProbability();
}
double Shop::TheoreticalAbsoluteThroughput() {
    return TheoreticalLambda() * TheoreticalRelativeThroughput();
}
double Shop::ActualAbsoluteThroughput() {
    return ActualLambda() * ActualRelativeThroughput();
}
int Shop::Factorial(int f) {
    int result = 1;
    for (int i = 2; i <= f; i++) {
        result *= i;
    }
    return result;
}

double Shop::ActualLambda() {
    return (double)(totalClients * 1000) / totalWorkTime;
}
double Shop::ActualMu() {
    return (double)(servedClients * 1000) / totalWorkTime;
}
double Shop::ActualIdlingProbability() {
    return (double)(sumIdlingTime) / totalWorkTime;
}
double Shop::ActualRejectionProbability() {
    return (double)(lostClients) / totalClients;
}

double Shop::TheoreticalLambda() {
    return spawnIntensity;
}
double Shop::TheoreticalMu() {
    return (double)(maxCheckouts * 1000) / (averageItems * itemTime);
}
double Shop::TheoreticalIdlingProbability() {
    double result = 0;
    double rho = (TheoreticalLambda() / TheoreticalMu());
    int n = maxCheckouts;
    int m = maxClients;
    for (int i = 0; i <= n; i++) {
        result += std::pow(rho, i) / Factorial(i);
    }
    for (int i = 1; i <= m; i++) {
        result += std::pow(rho, n + i) / (pow(n, i) * Factorial(n));
    }
    result = std::pow(result, -1);
    return result;
}
double Shop::TheoreticalRejectionProbability() {
    double rho = (TheoreticalLambda() / TheoreticalMu());
    int n = maxCheckouts;
    int m = maxClients;
    return (std::pow(rho, n + m) * TheoreticalIdlingProbability()) / (std::pow(n, m) * Factorial(n));
}