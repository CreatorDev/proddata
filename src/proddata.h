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

#ifndef PRODDATA_H_
#define PRODDATA_H_

#include <string>
#include <vector>
#include "device_data.h"

/**
 * @brief Class to perform read/write of production data.
 */
class Proddata {
 public:
  /**
   * @brief Constructor
   *
   * Creates an instance of Proddata
   *
   * @param[in] flash_access unique pointer to FlashAccess implementation
   */
  explicit Proddata(std::unique_ptr<FlashAccess> flash_access);
  ~Proddata();

  /**
   * @brief Write production data
   *
   * @param[in] data chunk of data to be written
   */
  void Write(const std::string &data);

  /**
   * @brief Write single data field
   *
   * @param[in] data field value
   */
  void WriteField(const std::string &name, const std::string &data);

  /**
   * @brief Read production data
   *
   * returns vector containing raw data
   */
  std::vector<uint8_t> Read();

  /**
   * @brief Read production data field e.g mac
   *
   * @param[in] data field to be read
   * returns vector containing raw data
   */
  std::vector<uint8_t> ReadField(const std::string& name);

 private:
  std::unique_ptr<DeviceData> device_data_;
};

#endif   // PRODDATA_H_
