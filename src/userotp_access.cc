/**
 * @file
 *  UserOTPAccess class
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

#include "userotp_access.h"
#include <glog/logging.h>
#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdexcept>

UserOTPAccess::UserOTPAccess(const std::string &device_name) : FlashAccess(device_name) {
  DLOG(INFO) << "Initialising UserOTPAccess";
}

UserOTPAccess::~UserOTPAccess() {
  DLOG(INFO) << "Deinitialising UserOTPAccess";
}

void UserOTPAccess::Write(const std::vector<uint8_t> &buf, const int offset) {
  SelectUserOTP();

  if (lseek(fd_, offset, SEEK_SET) < 0) {
    DLOG(ERROR) << "user otp write: lseek failed: " << strerror(errno);
    throw std::runtime_error("user otp write: lseek failed");
  }

  int ret = write(fd_, buf.data(), buf.size());
  if (ret < 0) {
    DLOG(ERROR) << "user otp write failed: " << strerror(errno);
    throw std::runtime_error("user otp write failed");
  }
}

std::vector<uint8_t> UserOTPAccess::Read(const int size, const int offset) {
  std::vector<uint8_t> buf(size);
  SelectUserOTP();

  if (lseek(fd_, offset, SEEK_SET) < 0) {
    DLOG(ERROR) << "user otp read: lseek failed: " << strerror(errno);
    throw std::runtime_error("user otp read: lseek failed");
  }

  int ret = read(fd_, buf.data(), buf.size());
  if (ret < 0) {
    DLOG(ERROR) << "user otp read failed:" << strerror(errno);
    throw std::runtime_error("user otp read failed");
  }
  return buf;
}

void UserOTPAccess::SelectUserOTP() {
  int val = MTD_OTP_USER;
  if (ioctl(fd_, OTPSELECT, &val) < 0) {
    DLOG(ERROR) << "ioctl failed and returned error: " << strerror(errno);
    throw std::runtime_error("UserOTPAccess: ioctl failed");
  }
}

