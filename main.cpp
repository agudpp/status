#include <iostream>
#include <string>

#include "StatusHandler.h"
#include "TimeHelper.h"
#include "SampleModule.h"


#define NUM_SAMPLES 9999999

/// we want to check:
/// 1) difference in times between simple return values.
/// 2) Using strings (descriptions)
/// 3) Nested methods
/// 4) Checking for errors (comparing if it fails or not).
///





int
main(void)
{
    // init the handler
    static StatusHandler<std::string> statusHandler;
    // set the global handler to be used by all the Status instances
    Status::s_shandler = &statusHandler;

    SampleModule sm;
    int dummy = 0;
    double normalTotalTime = 0.,
           statusSimpleTime = 0.,
           statusComplexTime = 0.;

    // we will check the time from now
    double t1 = TimeHelper::currentTime();

    for (unsigned int i = 0; i < NUM_SAMPLES; ++i) {
        const bool ret = sm.simple_nm();
        if (!ret) {
            dummy++;
        } else {
            dummy--;
        }
    }

    double t2 = TimeHelper::currentTime();
    normalTotalTime = t2 - t1;

    // now check the status method
    t1 = TimeHelper::currentTime();

    for (unsigned int i = 0; i < NUM_SAMPLES; ++i) {
        const Status ret = sm.simple_st();
        if (!ret) {
            dummy++;
        } else {
            dummy--;
        }
    }
    t2 = TimeHelper::currentTime();
    statusSimpleTime = t2 - t1;

    // now take the time using the description as well
    t1 = TimeHelper::currentTime();

    for (unsigned int i = 0; i < NUM_SAMPLES; ++i) {
        const Status ret = sm.composed();
        if (!ret) {
            dummy++;
        } else {
            dummy--;
        }
    }
    t2 = TimeHelper::currentTime();
    statusComplexTime = t2 - t1;

    // now print the statistics
    std::cout << "We run\n"
              << "\tNum Samples per test: " << NUM_SAMPLES << "\n"
              << "\tTotal time normal call (bool): " << normalTotalTime << "\n"
              << "\tSimple Status return time: " << statusSimpleTime << "\n"
              << "\tComplex Status return time: " << statusComplexTime << "\n";

    return 0;
}

