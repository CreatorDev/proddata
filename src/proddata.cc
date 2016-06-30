/**
 * @file
 * Proddata class
 *
 * @author Imagination Technologies
 *
 * @copyright <b>Copyright 2015 by Imagination Technologies Limited and/or its affiliated group companies.</b>
 *      All rights reserved.  No part of this software, either
 *      material or conceptual may be copied or distributed,
 *      transmitted, transcribed, stored in a retrieval system
 *      or translated into any human or computer language in any
 *      form by any means, electronic, mechanical, manual or
 *      other-wise, or disclosed to the third parties without the
 *      express written permission of Imagination Technologies
 *      Limited, Home Park Estate, Kings Langley, Hertfordshire,
 *      WD4 8LZ, U.K.
 */

#include "proddata.h"
#include <glog/logging.h>
#include "device_data.h"
#include "flash_access.h"

/**
 * Format string containing hexadecimal symbols
 * @note string should only have symbols 0-9 A-F
 * @return vector containing raw data
 *
 */

std::vector<uint8_t> FormatString(const std::string &data) {
  int size = data.size();
  std::vector<uint8_t> buf(size/2);
  const char *src = data.c_str();
  int j = 0;
  for (int i = 0; i < size; i += 2) {
    sscanf(src + i, "%02hhx", &buf[j++]);
  }

  return buf;
}

Proddata::Proddata(std::unique_ptr<FlashAccess> flash_access) {
  DLOG(INFO) << "Initialising Proddata";
  device_data_ = std::unique_ptr<DeviceData>(new DeviceData(std::move(flash_access)));
}

Proddata::~Proddata() {
  LOG(INFO) << "Deinitialising Proddata";
}

void Proddata::Write(const std::string &data) {
  LOG(INFO) << "Writing reg0 data and reg1 data";
  if (data.size() % 2 != 0) {
    LOG(ERROR) << "Invalid data given";
    throw std::runtime_error("Invalid data given");
  }

  std::vector<uint8_t> buf = FormatString(data);
  device_data_->Write(buf);
}

void Proddata::WriteField(const std::string &name, const std::string &data) {
  if (data.size() % 2 != 0) {
    LOG(ERROR) << "Invalid data given";
    throw std::runtime_error("Invalid data given");
  }
  std::vector<uint8_t> buf = FormatString(data);
  device_data_->WriteField(name, buf);
}

std::vector<uint8_t> Proddata::Read() {
  DLOG(INFO) << "Reading reg0 data and reg1 data";
  return device_data_->Read();
}

std::vector<uint8_t> Proddata::ReadField(const std::string &name) {
  DLOG(INFO) << "Reading data";
  return device_data_->ReadField(name);
}
