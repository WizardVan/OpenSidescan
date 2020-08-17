/*
 * Copyright 2020 © Centre Interdisciplinaire de développement en Cartographie des Océans (CIDCO), Tous droits réservés
 */

/* 
 * File:   MonitorTest.hpp
 * Author: Jordan McManus
 */

#ifndef MONITORTEST_HPP
#define MONITORTEST_HPP

#include "catch.hpp"
#include <Eigen/Dense>
#include "../src/OpenSidescan/sidescan/sidescanfile.h"
#include "../src/OpenSidescan/sidescan/sidescanimager.h"
#include "../src/OpenSidescan/detector/roidetector.h"
#include "../src/thirdParty/MBES-lib/src/datagrams/DatagramParserFactory.hpp"
#include "../src/thirdParty/MBES-lib/src/datagrams/DatagramParser.hpp"
#include <string>
#include <map>
#include <vector>
#include <experimental/filesystem>
#include <atomic>
#include "../src/OpenSidescan/utilities/FileLockUtils.h"
#include "../src/OpenSidescan/utilities/SideScanFileProcessor.hpp"
#include "../src/OpenSidescan/utilities/DirectoryMonitor.hpp"
#include <string>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>

#if defined(_WIN32)

#include <windows.h>
#include <stdio.h>

#else
#if defined(__linux) || defined(__linux__) || defined(linux)


#include <fcntl.h>
#include <sys/file.h>   // For flock structure

#endif
#endif



class SideScanFileProcessorForTest : public SideScanFileProcessor {
    //Sub class this to connect with OpenSideScan
public:

    void processFile(SidescanFile * f) {
        //In OpenSidescan, add file to project
        std::cout << "Processing sidescan file: " << f->getFilename() << std::endl;
        REQUIRE(false);
    }

    void reportProgress(std::string progress) {
        //In OpenSidescan, give feedback to user
        std::cout << progress << std::endl;
    }

};



TEST_CASE("Test file lock with monitor") {

    if (fork() == 0) {
        //child process
        std::string file = "test/data/lockTest/s4.xtf";
        int fd = open(file.c_str(), O_RDWR | O_NOATIME);

        struct flock flockStructForTest;
        memset(&flockStructForTest, 0, sizeof (flockStructForTest));

        flockStructForTest.l_type = F_WRLCK;
        if (fcntl(fd, F_SETLKW, &flockStructForTest) == -1) // F_SETLKW waits until lock obtained
        {
            //file is already locked
            REQUIRE(false);
        }

        sleep(10);

        flockStructForTest.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &flockStructForTest); // Release the lock
        close(fd);

        REQUIRE(true);

    } else {
        //parent process
        //let child aquire lock
        sleep(5);

        std::string path = "test/data/lockTest/";

        Eigen::Vector3d leverArm;
        leverArm << 0.0, 0.0, 0.0;

        // setup region of interest detector
        int fastThreshold = 300;
        int fastType = cv::FastFeatureDetector::TYPE_9_16;
        bool fastNonMaxSuppression = false;
        double dbscanEpsilon = 50;
        int dbscanMinimumPoints = 20;
        int mserDelta = 6;
        int mserMinimumArea = 320;
        int mserMaximumArea = 15000;
        bool mergeOverlappingObjects = true;

        RoiDetector * roiDetector = new RoiDetector(
                fastThreshold,
                fastType,
                fastNonMaxSuppression,
                dbscanEpsilon,
                dbscanMinimumPoints,
                mserDelta,
                mserMinimumArea,
                mserMaximumArea,
                mergeOverlappingObjects);

        SideScanFileProcessorForTest * processor = new SideScanFileProcessorForTest();
        DirectoryMonitor *monitor = new DirectoryMonitor(roiDetector, processor, leverArm);

        monitor->monitor(path);

        delete processor;
        delete monitor;

        REQUIRE(true);
    }
}

#endif /* MONITORTEST_HPP */

