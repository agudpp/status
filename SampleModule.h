#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H


#include "Status.h"

class SampleModule
{
public:
    SampleModule();

    Status
    simple_st(void);
    bool
    simple_nm(void);

    Status
    composed(void);

};

#endif // SAMPLEMODULE_H
