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

#ifndef FLASHACCESS_H_
#define FLASHACCESS_H_

#include <string>
#include <vector>

/**
 * @brief Abstract class for flash access
 */
class FlashAccess {
 public:
  explicit FlashAccess(const std::string &device_name);
  virtual ~FlashAccess();

  /**
   * @brief Write data to flash
   *
   * @param[in] buf chunk of data to be written
   * @param[in] offset device offset
   */
  virtual void Write(const std::vector<uint8_t> &buf, const int offset) = 0;

  /**
   * @brief Read data from flash storage

   * @param[in] size size of data to be read
   * @param[in] offset device offset
   * @returns vector containing read data
   *
   */
  virtual std::vector<uint8_t> Read(const int size, const int offset) = 0;

  /**
   * @brief Read serial number
   *
   * returns vector containing serial number
   */
  virtual std::vector<uint8_t> ReadSerial();

 protected:
  int fd_;
};

#endif  // FLASHACCESS_H_
