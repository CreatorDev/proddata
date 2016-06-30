/**
 * @file
 * MTDAccess class
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

#ifndef MTDACCESS__H_
#define MTDACCESS__H_

#include <mtd/mtd-user.h>
#include <string>
#include <vector>
#include "flash_access.h"

/**
 * @brief MTDAccess class to perfrom read/write on mtd device
 */
class MTDAccess final: public FlashAccess {
 public:
  /**
   * @brief Constructor
   *
   * Creates an instance of MTDAccess
   *
   */
  explicit MTDAccess(const std::string &device_name);
  ~MTDAccess();

  void Write(const std::vector<uint8_t> &buf, const int offset);
  std::vector<uint8_t> Read(const int size, const int offset);

 private:
  mtd_info_t mtd_info_;
};

#endif  // MTDACCESS_H_
