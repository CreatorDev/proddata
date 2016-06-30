/**
 * @file
 * Abstract class for flash access
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

#include "flash_access.h"
#include <glog/logging.h>
#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdexcept>

FlashAccess::FlashAccess(const std::string &device_name) {
  fd_ = open(device_name.c_str(), O_RDWR);
  if (fd_ < 0) {
    DLOG(ERROR) << "Can't open device: " << strerror(errno);
    throw std::runtime_error("FlashAccess Initialization failed");
  }
}

FlashAccess::~FlashAccess() {
  close(fd_);
}

std::vector<uint8_t> FlashAccess::ReadSerial() {
  DLOG(INFO) << "Reading serial number";
  const int size = 8;
  std::vector<uint8_t> buf(size);
  int val = MTD_OTP_FACTORY;
  if (ioctl(fd_, OTPSELECT, &val) < 0) {
    DLOG(ERROR) << "ioctl failed and returned error: " << strerror(errno);
    throw std::runtime_error("Factory OTP access failed");
  }

  if (lseek(fd_, 0, SEEK_SET) < 0) {
    DLOG(ERROR) << "read serial: lseek failed: " << strerror(errno);
    throw std::runtime_error("read serial: lseek failed");
  }

  int ret = read(fd_, buf.data(), buf.size());
  if (ret < 0) {
    DLOG(ERROR) << "read serial num failed:" << strerror(errno);
    throw std::runtime_error("read serial num failed");
  }
  return buf;
}

