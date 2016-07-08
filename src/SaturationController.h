#pragma once

#include <Configuration.pb.h>

namespace SaturationChanger {

class SaturationController {
public:
	virtual ~SaturationController() {}
    virtual void setGameSaturation(const Configuration& conf) = 0;
    virtual void setDesktopSaturation(const Configuration& conf) = 0;

};

}