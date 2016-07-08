#include <config/SaturationChangerConfig.h>
#include "ProcessMonitor.h"
#include <WinProcessFinder.h>
#include "NvidiaSaturationController.h"
#include "AmdSaturationController.h"

namespace SaturationChanger {

ProcessMonitor::ProcessMonitor(Configuration* config) : stop(false), config(config) {
#ifdef PLATFORM_LINUX
	processFinder = new LinuxProcessFinder;
#endif
#ifdef PLATFORM_WINDOWS
	processFinder = std::make_unique<WinProcessFinder>();
#endif

    if(config->vendor() == AMD) {
        controller = std::make_unique<AmdSaturationController>(config->display_id());
    }
    else if (config->vendor() == NVIDIA) {
        controller = std::make_unique<NvidiaSaturationController>(config->display_id());
    }
    else {
        throw std::runtime_error("Only AMD and NVIDIA are support");
    }
}

ProcessMonitor::~ProcessMonitor() {
    stop = true;

    if (controller != nullptr)
        controller->setDesktopSaturation(*config);

    if (monitoringThread->joinable())
        monitoringThread->join();    
}

void ProcessMonitor::init() {
    monitoringThread = std::make_unique<std::thread>([this] () {
        std::chrono::milliseconds sleep_duration(2000);

        while(!this->stop) {
            if(processFinder->isProcessRunning(config->process_name())) {
                controller->setGameSaturation(*config);
            }
            else {
                controller->setDesktopSaturation(*config);
            }
                
            std::this_thread::sleep_for(sleep_duration);
        }
    });
}

}
