#include <config/SaturationChangerConfig.h>
#include "ProcessMonitor.h"
#include <WinProcessFinder.h>
#include "NvidiaSaturationController.h"
#include "AmdSaturationController.h"

namespace SaturationChanger {

ProcessMonitor::ProcessMonitor(Configuration* config) : stopMonitoring(false), config(config) {
#ifdef PLATFORM_LINUX
	processFinder = new LinuxProcessFinder;
#endif
#ifdef PLATFORM_WINDOWS
	processFinder = std::make_unique<WinProcessFinder>();
#endif
}

ProcessMonitor::~ProcessMonitor() {
    stop();
}

void ProcessMonitor::start() {
    std::cout << "started process monitor" << std::endl;

    createController();

    stopMonitoring = false;

    monitoringThread = std::make_unique<std::thread>([this] () {
        std::chrono::milliseconds sleep_duration(2000);

        while(!stopMonitoring) {
            std::cout << "searching for process..." << std::endl;

            if(processFinder->isProcessRunning(config->process_name())) {
                std::cout << "found the game running..." << std::endl;
                controller->setGameSaturation(*config);
            }
            else {
                controller->setDesktopSaturation(*config);
            }
                
            std::this_thread::sleep_for(sleep_duration);
        }
    });
}

void ProcessMonitor::stop() {
    std::cout << "stopping process monitor" << std::endl;

    stopMonitoring = true;

    if (controller != nullptr) {        
        controller->setDesktopSaturation(*config);
        controller.release();
    }

    if (monitoringThread->joinable()) {        
        monitoringThread->join();
        monitoringThread.release();
    }
}

void ProcessMonitor::restart() {
    stop();
    start();
}

void ProcessMonitor::createController() {
    if (config->vendor() == AMD) {
        controller = std::make_unique<AmdSaturationController>(config->display_id());
    }
    else if (config->vendor() == NVIDIA) {
        controller = std::make_unique<NvidiaSaturationController>(config->display_id());
    }
    else {
        throw std::runtime_error("Only AMD and NVIDIA are support");
    }
}

}
