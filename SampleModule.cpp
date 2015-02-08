#include "SampleModule.h"

SampleModule::SampleModule()
{
}

Status
SampleModule::simple_st(void)
{
    return true;
}

bool
SampleModule::simple_nm(void)
{
    return true;
}

Status
SampleModule::composed(void)
{
    return Status(StatusCode::STC_INTERNAL_ERROR,
                  StatusSeverity::STS_CRITICAL,
                  "This is an example of how we can return not only the error and "
                  "the severity but also the description of it");
}
