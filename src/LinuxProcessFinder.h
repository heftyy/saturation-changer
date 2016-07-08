#pragma once

#include "ProcessFinder.h"

#ifdef PLATFORM_LINUX

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

namespace SaturationChanger {

class LinuxProcessFinder : public ProcessFinder {

public:
    virtual bool isProcessRunning(std::string process_name) override;

};

}

#endif