/**
 ********************************************************************
 *
 * @copyright (c) 2023 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI’s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */
#include "image_processor_publisher.h"

#include <sys/time.h>

#include <fstream>
#include <sstream>

#include "logger.h"
#include "util_misc.h"

#include <dlfcn.h>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <csignal>

using namespace std;
using namespace edge_sdk;

namespace edge_app {

volatile sig_atomic_t flag = 1;
void handler(int signum)
{
    flag = 0;
}

int32_t ImageProcessorPublisher::Init() {

    const string SO_FILE_PATH = "/usr/local/lib/libros_shared_object_library.so";
    std::signal(SIGINT, handler); 
    handle = dlopen(SO_FILE_PATH.c_str(), RTLD_NOW);
    if (!handle) {
        ERROR("Error loading shared object: %s", dlerror());
        return 1;
    }

    intptr_t (*create)() = (intptr_t (*)())dlsym(handle, "create");
    if (!create) {
        ERROR("Error loading symbol create: %s", dlerror());
        return 1;
    }

    // Create the instance of the ROS2 node
    instance = (*create)();

    return 0;
}

void ImageProcessorPublisher::Process(const std::shared_ptr<Image> image) {
    
    void (*spin_some)(intptr_t) = (void (*)(intptr_t))dlsym(handle, "spin_some");
    if (!spin_some) {
        ERROR("Error loading symbol spin_some: %s", dlerror());
        return;
    }

    // Function to publish the sensor image (assuming it takes an image and an instance)
    void (*talk)(intptr_t, const cv::Mat&) = (void (*)(intptr_t, const cv::Mat&))dlsym(handle, "talk");
    if (!talk) {
        ERROR("Error loading symbol talk: %s", dlerror());
        return;
    }

    void (*destroy)(intptr_t) = (void (*)(intptr_t))dlsym(handle, "destroy");
    if (!destroy) {
        ERROR("Error loading symbol destroy: %s", dlerror());
        return;
    }

    auto do_publish = [&] {
        Mat& frame = *image;

        // Publish the frame
        (*talk)(instance, frame);
        (*spin_some)(instance);

        if (flag == 0)
        {
            // Clean up
            (*destroy)(instance);
            dlclose(handle);
            handle = nullptr;
        }
    };
    do_publish();

}

}  // namespace edge_app
