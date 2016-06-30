/**
 * @file
 *  MTDAccess class
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

#include "mtd_access.h"
#include <glog/logging.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdexcept>
#include "vector_operations.h"

MTDAccess::MTDAccess(const std::string &device_name) : FlashAccess(device_name) {
  DLOG(INFO) << "Initialising MTDAccess";
  if (ioctl(fd_, MEMGETINFO, &mtd_info_) < 0) {
    DLOG(ERROR) << "ioctl failed and returned error: " << strerror(errno);
    close(fd_);
    throw std::runtime_error("MTDAccess Initialization failed");
  }
}

MTDAccess::~MTDAccess() {
  DLOG(INFO) << "Deinitialising MTDAccess";
}

void MTDAccess::Write(const std::vector<uint8_t> &buf, const int offset) {
  int sector_size = mtd_info_.erasesize;

  /* read sector (note : it is assumed that all the device data will be on 1st sector) */
  std::vector<uint8_t> read_buf = this->Read(sector_size, 0);

  /* erase sector */
  erase_info_t ei;
  ei.length = mtd_info_.erasesize;
  ei.start = 0;
  if (ioctl(fd_, MEMERASE, &ei) < 0) {
    DLOG(ERROR) << "ioctl failed and returned error: " << strerror(errno);
    throw std::runtime_error("mtd write: ioctl failed");
  }
  /* modify sector */
  vector_operations::replace(&read_buf, buf, offset);

  /* write sector */
  if (lseek(fd_, 0, SEEK_SET) < 0) {
    DLOG(ERROR) << "mtd write: lseek failed:" << strerror(errno);
    throw std::runtime_error("mtd write: lseek failed");
  }

  int ret = write(fd_, read_buf.data(), sector_size);
  if (ret < 0) {
    DLOG(ERROR) << "mtd write failed:" << strerror(errno);
    throw std::runtime_error("mtd write failed");
  }
}

std::vector<uint8_t> MTDAccess::Read(const int size, const int offset) {
  std::vector<uint8_t> buf(size);
  if (lseek(fd_, offset, SEEK_SET) < 0) {
    DLOG(ERROR) << "mtd read: lseek failed:" << strerror(errno);
    throw std::runtime_error("mtd read: lseek failed");
  }

  int ret = read(fd_, buf.data(), buf.size());
  if (ret < 0) {
    DLOG(ERROR) << "mtd read failed:" << strerror(errno);
    throw std::runtime_error("mtd read failed");
  }
  return buf;
}

