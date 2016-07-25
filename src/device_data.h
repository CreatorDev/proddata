/**
 * @file
 * DeviceData class
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


#ifndef DEVICEDATA_H_
#define DEVICEDATA_H_

#include <map>
#include <string>
#include <memory>
#include <vector>
#include "flash_access.h"

/**
 * @brief class for maintaining device data layout and performing read/write operations
 */
class DeviceData {
 public:
  /**
   * @brief Constructor
   *
   * Creates an instance of DeviceData
   * @param[in] flash_access unique pointer to FlashAccess implementation
   *
   */
  explicit DeviceData(std::unique_ptr<FlashAccess> flash_access);
  ~DeviceData();

  /**
   * @brief Parse the data into reg0 data and reg1 data and write appropriately
   *
   * @param[in] data  chunk of data to be written
   */
  void Write(const std::vector<uint8_t> &data);

  /**
   * @brief Write single field to OTP e.g MAC_0
   *
   * @param[in] name name of field
   * @param[in] data field value to be written
   */
  void WriteField(const std::string &name, const std::vector<uint8_t> &data);

  /**
   * @brief Read device data from memory
   *
   * returns vector containing raw data
   */
  std::vector<uint8_t> Read();

  /**
   * @brief Read device data field e.g MAC_0
   *
   * @param[in] name name of data field
   * returns vector containing raw data
   */
  std::vector<uint8_t> ReadField(const std::string &name);

  /**
   * @brief struct for storing size and offset for each field
  */
  struct DataField {
    int size;
    int offset;
  };

 private:
  int reg_version_[3];
  const std::string key_serial{"SERIAL"};

  typedef std::map<std::string, struct DataField> Layout;
  typedef std::map<int, Layout> RegisterVersions;

  Layout register_data_fields_[3];

  const Layout register0_v1_data_fields_ {
    {"CRC_REG0", {2, 0}},
    {"VERSION_REG0", {1, 2}},
    {"MAC_0", {6, 3}},
    {"MAC_1", {6, 9}},
    {"MAC_2", {6, 15}},
    {"MAC_3", {6, 21}},
    {"MAC_4", {6, 27}},
    {"MAC_5", {6, 33}},
  };
  const RegisterVersions register0_{
    {1, register0_v1_data_fields_},
  };

  const Layout register1_v0_data_fields_ {
    {"CRC_REG1", {2, 256}},
    {"VERSION_REG1", {1, 258}},
  };
  const Layout register1_v1_data_fields_ {
    {"CRC_REG1", {2, 256}},
    {"VERSION_REG1", {1, 258}},
    {"DCXO", {1, 259}},
  };
  const Layout register1_v2_data_fields_ {
    {"CRC_REG1", {2, 256}},
    {"VERSION_REG1", {1, 258}},
    {"DCXO", {1, 259}},
    {"PD_A1_B24", {1, 260}},
    {"PD_A1_B51", {1, 261}},
    {"PD_A1_B52", {1, 262}},
    {"PD_A1_B53", {1, 263}},
    {"PD_A1_B54", {1, 264}},
    {"PD_A2_B24", {1, 265}},
    {"PD_A2_B51", {1, 266}},
    {"PD_A2_B52", {1, 267}},
    {"PD_A2_B53", {1, 268}},
    {"PD_A2_B54", {1, 269}},
  };
  const RegisterVersions register1_{
    {0, register1_v0_data_fields_},
    {1, register1_v1_data_fields_},
    {2, register1_v2_data_fields_},
  };

  std::vector<RegisterVersions> registers_;

  enum RegisterName {
    register0,
    register1,
    last,
  };

  std::unique_ptr<FlashAccess> flash_access_;

  /**
   * @brief read register0 and register1 version from OTP
   *
   */
  void ReadVersionFromOTP();

  /**
   * @brief Parse data vector to read reg0 version and reg1 version
   *
   * @param[in] data vector containing raw data
   */
  void ReadVersionFromData(const std::vector<uint8_t> &data);

  /**
   * @brief Select register layout to use from its version
   *
   * @param[in] register_name enum specifying which register map to use
   */
  void SelectRegLayout(RegisterName register_name);

  /**
   * @brief Parse data vector to reg0 data and reg1 data and validate CRC
   *
   * @param[in] data vector containing raw data
   * @param[out] reg0_data pointer to vector storing register0 data
   * @param[out] reg1_data pointer to vector storing register1 data
   */
  void ParseData(const std::vector<uint8_t> &data, std::vector<uint8_t> *reg0_data,
                 std::vector<uint8_t> *reg1_data);

  /**
   * @brief Get Sum of size of all data fields in map
   *
   * @param[in] register_name enum specifying which register map to use
   */
  int GetRegisterSize(RegisterName register_name);

  /**
   * @brief Get offset of CRC field from register map
   *
   * @param[in] register_name enum specifying which register map to use
   */
  int GetCRCOffset(RegisterName register_name);

  /**
   * @brief Get DataField structure(size, offset) from corresponding name(key)
   *
   * @param[in] register_name enum specifying which register map to use
   * @param[in] name  name of data field
   */
  const DeviceData::DataField GetDataField(RegisterName register_name, const std::string &name);

  /**
  * @brief Get RegisterName in which the field is defined, from field name
  *
  * @param[in] name  name of data field
  * returns RegisterName enum specifying register map
  */
  const DeviceData::RegisterName GetRegisterName(const std::string &name);
};

#endif  // DEVICEDATA_H_
