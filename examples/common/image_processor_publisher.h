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
#ifndef __IMAGE_PROCESSOR_PUBLISHER_H__
#define __IMAGE_PROCESSOR_PUBLISHER_H__

#include <memory>

#include "image_processor.h"

namespace edge_app {

class ImageProcessorPublisher : public ImageProcessor {
   public:
    ImageProcessorPublisher(const std::string& name) : show_name_(name) {}

    ~ImageProcessorPublisher() override {}

    int32_t Init() override;

    void Process(const std::shared_ptr<Image> image) override;

   private:
    std::string show_name_;

    void* handle = nullptr;
    intptr_t instance = 0;

};

}  // namespace edge_app

#endif

