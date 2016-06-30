/**
 * @file
 * Proddata tool main entry point
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

#include <glog/logging.h>
#include <string>
#include <iomanip>
#include <iostream>
#include "proddata.h"
#include "flash_access.h"
#include "userotp_access.h"

static void PrintData(const std::vector<uint8_t> &data) {
  int size = data.size();
  for (int i = 0; i < size; i++) {
    std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[i]);
  }
  std::cout << std::endl;
}

static void usage() {
  std::string mesg = "Usage: proddata write <data>             Write complete calibration data \n"
                     "       proddata write <field> <value>    Write single data field only \n"
                     "       proddata read                     Read calibration data \n"
                     "       proddata read <field>             Read data field \n";
  std::cerr << mesg;
}

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  if (argc < 2) {
    usage();
    return -1;
  }

  try {
    // TODO(Sagar): Make FlashAccess implemetation and harcoded device name configurable
    std::unique_ptr<FlashAccess> flash_access(new UserOTPAccess("/dev/mtd1"));
    Proddata proddata(std::move(flash_access));

    if (!strcmp(argv[1], "write")) {
      if (argv[2] == NULL) {
        std::cerr << "Specify data to be written to OTP" << std::endl;
        return -1;
      } else if (argv[3] == NULL) {
        proddata.Write(argv[2]);
      } else {
        proddata.WriteField(argv[2], argv[3]);
      }
    } else if (!strcmp(argv[1], "read")) {
      std::vector<uint8_t> data;
      if (argv[2] == NULL) {
        data = proddata.Read();
      } else {
        data = proddata.ReadField(argv[2]);
      }
      PrintData(data);
    } else {
      std::cerr << "Invalid command" << std::endl;
      usage();
      return -1;
    }
  } catch (std::runtime_error &e) {
    google::ShutdownGoogleLogging();
    return -1;
  }

  google::ShutdownGoogleLogging();
  return 0;
}
