/*
 * File Status.h, Status
 * Created on 08/01/2015.
 * Author: Agustin
 *
 */

#ifndef TIMEHELPER_H_
#define TIMEHELPER_H_

#include <sys/time.h>

namespace TimeHelper {

///
/// \brief currentTime
/// \return
///
double
currentTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return (double) tv.tv_sec + 1e-6 * tv.tv_usec;
}

}


#endif // TIMEHELPER_H_
