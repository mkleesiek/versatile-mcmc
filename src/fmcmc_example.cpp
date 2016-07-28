/*
 * fmcmc.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include "fmcmc/logger.h"

LOG_DEFINE("fmcmc.example")

using namespace std;
using namespace fmcmc;

int main(int argc, char* argv[]){

    for (auto i : {0,1,2,3,4,5} )
        LOG(Info, i);

    return 0;
}

