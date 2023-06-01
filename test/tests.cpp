// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include "task.h"


TEST(ShopTest, RejectionProbabilityTest) {
    Shop shop = Shop(4,3,100,5,12);
    double rejectionProbability = shop.TheoreticalRejectionProbability();
    EXPECT_EQ(2.6103752090097983e-16, rejectionProbability);
}

TEST(ShopTest, RelativeThroughputTest) {
	Shop shop = Shop(4,3,100,5,12);        
    double relativeThroughput = shop.TheoreticalRelativeThroughput();
    EXPECT_EQ(0.99999999999999978, relativeThroughput);
}

TEST(ShopTest, AbsoluteThroughputTest) {
	Shop shop = Shop(4,3,100,5,12);
    double absoluteThroughput = shop.TheoreticalAbsoluteThroughput();
    EXPECT_EQ(2.9999999999999991, absoluteThroughput);
}