/**
 * @file
 * FlashAccess Mock
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

#ifndef FLASHACCESS_MOCK_H
#define FLASHACCESS_MOCK_H

#include <gmock/gmock.h>
#include <vector>
#include "flash_access.h"

/**
 * @brief Mock FlashAccess
 */

class MockFlashAccess : public FlashAccess {
 public:
  MockFlashAccess() : FlashAccess("/dev/null") {}
  /**
   * @brief mock method for Write method of FlashAccess
   */
  MOCK_METHOD2(Write, void(const std::vector<uint8_t>&, const int));

  /**
   * @brief mock method for Read method of FlashAccess
   */
  MOCK_METHOD2(Read, std::vector<uint8_t>(const int, const int));

  /**
   * @brief mock method for ReadSerial method of FlashAccess
   */
  MOCK_METHOD0(ReadSerial, std::vector<uint8_t>());
};
#endif
