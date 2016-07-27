/*
 * random_test.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include "fmcmc/random.h"

#include <gtest/gtest.h>

using namespace fmcmc;

TEST(random_test, singleton_initialization) {
    Random& rand = Random::Instance();
    rand.SetSeed(1);

    ASSERT_EQ(rand.Uniform(0, 100), 42) << "Deterministic random"
            "number generator with unexpected result.";

    ASSERT_DOUBLE_EQ(rand.Uniform(-99.0, +99.0), 43.624248925001041);
}

