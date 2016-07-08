#pragma once

#include <string>
#include <config/SaturationChangerConfig.h>

namespace SaturationChanger {

class ProcessFinder {

public:
    virtual ~ProcessFinder() {}
    virtual bool isProcessRunning(std::string process_name) = 0;
};

}