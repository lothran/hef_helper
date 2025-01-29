#include "hailo/hailort_common.hpp"
#include "hailo/hailort_defaults.hpp"

#include "hailo/hef.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#define RGB_FEATURES_SIZE (3)
#define RGBA_FEATURES_SIZE (4)
#define GRAY8_FEATURES_SIZE (1)
#define YUY2_FEATURES_SIZE (2)
#define NV12_FEATURES_SIZE (3)
#define NV21_FEATURES_SIZE (3)
#define I420_FEATURES_SIZE (3)
static const char *
gst_hailonet_get_format_string(const hailo_vstream_info_t &input) {
  switch (input.format.order) {
  case HAILO_FORMAT_ORDER_RGB4:
  case HAILO_FORMAT_ORDER_NHWC:
    if (input.shape.features == RGBA_FEATURES_SIZE) {
      return "RGBA";
    }
    if (input.shape.features == GRAY8_FEATURES_SIZE) {
      return "GRAY8";
    }
    /* Fallthrough */
  case HAILO_FORMAT_ORDER_NHCW:
  case HAILO_FORMAT_ORDER_FCR:
  case HAILO_FORMAT_ORDER_F8CR:
    if (input.shape.features == GRAY8_FEATURES_SIZE) {
      return "GRAY8";
    }
    return "RGB";
  case HAILO_FORMAT_ORDER_YUY2:
    return "YUY2";
  case HAILO_FORMAT_ORDER_NV12:
    return "NV12";
  case HAILO_FORMAT_ORDER_NV21:
    return "NV21";
  case HAILO_FORMAT_ORDER_I420:
    return "I420";
  default:
    std::cerr << "unsupported network input";
    exit(1);
  }
}
static uint32_t get_height_by_order(uint32_t original_height,
                                    hailo_format_order_t order) {
  switch (order) {
  case HAILO_FORMAT_ORDER_NV12:
  case HAILO_FORMAT_ORDER_NV21:
    return original_height * 2;
  default:
    break;
  }
  return original_height;
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    exit(-1);
  }
  auto hef = hailort::Hef::create(argv[1]);

  auto inputs = hef->get_input_vstream_infos();
  auto doc = rapidjson::Document(rapidjson::kObjectType);
  rapidjson::Value json_inputs(rapidjson::kArrayType);
  if (inputs.status() != HAILO_SUCCESS) {
    std::cerr << "failed to get hef properties";
    return 1;
  }
  if (inputs->size() != 1) {
    std::cerr << "only models with one input stream are supported\n";
    return 1;
  }
  auto input = inputs.value()[0];
  doc.AddMember("pixel_format",
                rapidjson::Value(gst_hailonet_get_format_string(input),
                                 doc.GetAllocator()),
                doc.GetAllocator());
  doc.AddMember("width", input.shape.width, doc.GetAllocator());
  doc.AddMember("height",
                get_height_by_order(input.shape.height, input.format.order),
                doc.GetAllocator());
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(buf);

  doc.Accept(wr);
  std::cout << buf.GetString() << std::endl;

  return 0;
}
