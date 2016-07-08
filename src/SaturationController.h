#pragma once

namespace SaturationChanger {

class SaturationController {
public:
    virtual void setGameSaturation(const Configuration& conf) = 0;
    virtual void setDesktopSaturation(const Configuration& conf) = 0;

};

}