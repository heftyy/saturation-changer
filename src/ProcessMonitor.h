﻿#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include "ProcessFinder.h"
#include <Configuration.pb.h>
#include "SaturationController.h"

namespace SaturationChanger {
	
class ProcessMonitor {
public:
    explicit ProcessMonitor(Configuration* config);
    ~ProcessMonitor();

    void start();
    void stop();
    void restart();

private:
	std::unique_ptr<ProcessFinder> processFinder;
    std::unique_ptr<std::thread> monitoringThread;
    std::unique_ptr<SaturationController> controller;
    std::atomic<bool> stopMonitoring;
    Configuration* config;

    void createController();
};

}
