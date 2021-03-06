/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_CONTRIB_TENSORRT_RESOURCES_TRT_RESOURCES_H_
#define TENSORFLOW_CONTRIB_TENSORRT_RESOURCES_TRT_RESOURCES_H_

#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "tensorflow/contrib/tensorrt/convert/utils.h"
#include "tensorflow/contrib/tensorrt/log/trt_logger.h"
#include "tensorflow/contrib/tensorrt/resources/trt_allocator.h"
#include "tensorflow/contrib/tensorrt/resources/trt_int8_calibrator.h"
#include "tensorflow/core/framework/resource_mgr.h"

#if GOOGLE_CUDA
#if GOOGLE_TENSORRT

#include "tensorrt/include/NvInfer.h"

namespace tensorflow {
namespace tensorrt {

class TRTCalibrationResource : public tensorflow::ResourceBase {
 public:
  ~TRTCalibrationResource() {
    VLOG(0) << "Destroying Calibration Resource " << std::endl << DebugString();
    builder_.reset();
    engine_.reset();
    // We need to manually destroy the builder and engine before the allocator
    // is destroyed.
    allocator_.reset();
  }

  string DebugString() override {
    std::stringstream oss;
    using std::dec;
    using std::endl;
    using std::hex;
    oss << " Calibrator = " << hex << calibrator_.get() << dec << endl
        << " Builder    = " << hex << builder_.get() << dec << endl
        << " Engine     = " << hex << engine_.get() << dec << endl
        << " Logger     = " << hex << &logger_ << dec << endl
        << " Allocator  = " << hex << allocator_.get() << dec << endl
        << " Thread     = " << hex << thr_.get() << dec << endl;
    return oss.str();
  }

  std::unique_ptr<TRTInt8Calibrator> calibrator_;
  TrtUniquePtrType<nvinfer1::IBuilder> builder_;
  TrtUniquePtrType<nvinfer1::ICudaEngine> engine_;
  std::unique_ptr<nvinfer1::IGpuAllocator> allocator_;
  tensorflow::tensorrt::Logger logger_;
  // TODO(sami): Use threadpool threads!
  std::unique_ptr<std::thread> thr_;
};

class TRTWeightStore {
 public:
  TRTWeightStore() {}

  virtual ~TRTWeightStore() { VLOG(1) << "Destroying store" << DebugString(); }

  string DebugString() {
    std::stringstream oss;
    size_t len_bytes = 0;
    for (const auto& v : store_) {
      len_bytes += v.size() * sizeof(uint8_t);
    }
    oss << " Number of entries     = " << store_.size() << std::endl
        << " Total number of bytes = "
        << store_.size() * sizeof(std::vector<uint8_t>) + len_bytes
        << std::endl;
    return oss.str();
  }

  std::list<std::vector<uint8_t>> store_;
};

}  // namespace tensorrt
}  // namespace tensorflow

#endif
#endif
#endif  // TENSORFLOW_CONTRIB_TENSORRT_RESOURCES_TRT_RESOURCES_H_
