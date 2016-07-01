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

#include "device_data.h"
#include <glog/logging.h>
#include <regex>

extern "C" {
#include "lib_crc.h"
}

#include "vector_operations.h"

static const int kCRCSize = 2;
static const int versionSize = 1;
static const int regVersionOffset[] = {2, 258};

static std::vector<uint8_t> CalculateDataCRC(const std::vector<uint8_t> &data) {
  int16_t crc_16 = 0;
  const unsigned char *ptr = data.data();
  int size = data.size();
  while (size) {
    crc_16 = update_crc_16(crc_16, *ptr);
    ptr++;
    size--;
  }
  /* convert int16_t crc to crc vector */
  char crc_0 = (crc_16 >> 8) & 0xff;
  char crc_1 = crc_16 & 0xff;

  std::vector<uint8_t> crc;

  crc.push_back(crc_0);
  crc.push_back(crc_1);

  return crc;
}

static void AddDataCRC(std::vector<uint8_t> *data) {
  std::vector<uint8_t> crc = CalculateDataCRC(*data);
  data->insert(data->begin(), crc.begin(), crc.end());
}

static void CheckDataCRC(const std::vector<uint8_t> &data) {
  /* ignore first 2 bytes which stores crc */
  std::vector<uint8_t> data_without_crc(data.begin() + kCRCSize, data.end());
  std::vector<uint8_t> crc = CalculateDataCRC(data_without_crc);

  if ((crc[0] != data[0]) || (crc[1] != data[1])) {
    LOG(ERROR) << "Data corrupted:CRC failed";
    throw std::runtime_error("Data corrupted:CRC failed");
  }
}

static void ReplaceDataCRC(std::vector<uint8_t> *data) {
  /* Erase old crc */
  data->erase(data->begin(), data->begin() + kCRCSize);

  AddDataCRC(data);
}

DeviceData::DeviceData(std::unique_ptr<FlashAccess> flash_access) :
    flash_access_(std::move(flash_access)) {
  DLOG(INFO) << "Initialising DeviceData";
  registers_.push_back(register0_);
  registers_.push_back(register1_);
}

DeviceData::~DeviceData() {
  DLOG(INFO) << "Deinitialising DeviceData";
}

void DeviceData::ReadVersionFromOTP() {
  for (int i = register0; i != last; i++) {
    std::vector<uint8_t> version = flash_access_->Read(versionSize, regVersionOffset[i]);
    reg_version_[i] = static_cast<int>(version[0]);
  }
  SelectRegLayout(register0);
  SelectRegLayout(register1);
}

void DeviceData::ReadVersionFromData(const std::vector<uint8_t> &data) {
  reg_version_[0] = static_cast<int>(data[0]);
  SelectRegLayout(register0);
  int reg0_data_size = GetRegisterSize(register0) - kCRCSize;
  int data_size = data.size();
  if (data_size > reg0_data_size) {
    reg_version_[1] = static_cast<int>(data[reg0_data_size]);
    SelectRegLayout(register1);
  } else {
    LOG(ERROR) << "Data size error";
    throw std::runtime_error("Data size error");
  }
}

void DeviceData::SelectRegLayout(RegisterName register_name) {
  const auto it = registers_[register_name].find(reg_version_[register_name]);
  if (it != registers_[register_name].end()) {
    register_data_fields_[register_name] = it->second;
  } else {
    LOG(ERROR) << "No valid reg version";
    throw std::runtime_error("No valid reg version");
  }
}

int DeviceData::GetRegisterSize(RegisterName register_name) {
  int size = 0;
  for (const auto &field : register_data_fields_[register_name]) {
    size = size + field.second.size;
  }

  return size;
}

int DeviceData::GetCRCOffset(RegisterName register_name) {
  if (register_name == register0) {
    return GetDataField(register_name, "CRC_REG0").offset;
  } else {
    return GetDataField(register_name, "CRC_REG1").offset;
  }
}

const DeviceData::DataField DeviceData::GetDataField(RegisterName register_name,
                                                     const std::string &name) {
  for (const auto &field : register_data_fields_[register_name]) {
    if (field.first == name) {
      return field.second;
    }
  }

  std::string error = "Invalid data field: " + name;
  throw std::runtime_error(error);
}

void DeviceData::ParseData(const std::vector<uint8_t> &data, std::vector<uint8_t> *reg0_data,
                           std::vector<uint8_t> *reg1_data) {
  int reg0_data_size = GetRegisterSize(register0) - kCRCSize;
  int reg1_data_size = GetRegisterSize(register1) - kCRCSize;
  int size = data.size();
  if (size != (reg0_data_size + reg1_data_size)) {
    LOG(ERROR) << "Data size error";
    throw std::runtime_error("Data size error");
  }

  reg0_data->assign(data.begin(), data.begin() + reg0_data_size);
  reg1_data->assign(data.begin() + reg0_data_size, data.end());

  /* Add CRC */
  AddDataCRC(reg0_data);
  AddDataCRC(reg1_data);
}

void DeviceData::Write(const std::vector<uint8_t> &data) {
  ReadVersionFromData(data);

  std::vector<uint8_t> reg0_data;
  std::vector<uint8_t> reg1_data;

  ParseData(data, &reg0_data, &reg1_data);

  flash_access_->Write(reg0_data, GetCRCOffset(register0));
  flash_access_->Write(reg1_data, GetCRCOffset(register1));
}

const DeviceData::RegisterName DeviceData::GetRegisterName(const std::string &name) {
  for (int i = register0; i != last; i++) {
    if (register_data_fields_[i].end() != register_data_fields_[i].find(name)) {
      return static_cast<DeviceData::RegisterName>(i);
    }
  }
  LOG(ERROR) << "Invalid data field: " << name;
  throw std::runtime_error("Invalid data field: " + name);
}

void DeviceData::WriteField(const std::string &name, const std::vector<uint8_t> &data) {
  if (std::regex_match(name, std::regex("(VERSION_REG)(.*)"))) {
    LOG(ERROR) << "Cannot modify register version";
    throw std::runtime_error("Cannot modify register version");
  }

  ReadVersionFromOTP();
  DeviceData::RegisterName register_name = GetRegisterName(name);
  DataField field = GetDataField(register_name, name);
  int size = data.size();
  if (size != field.size) {
    LOG(ERROR) << "Invalid field size";
    throw std::runtime_error("Invalid field size");
  }

  std::vector<uint8_t> buf = flash_access_->Read(GetRegisterSize(register_name),
                                                 GetCRCOffset(register_name));

  /* modify register data to update new value of field */
  int data_field_position = field.offset - GetCRCOffset(register_name);
  vector_operations::replace(&buf, data, data_field_position);

  ReplaceDataCRC(&buf);

  flash_access_->Write(buf, GetCRCOffset(register_name));
}

std::vector<uint8_t> DeviceData::Read() {
  ReadVersionFromOTP();
  std::vector<uint8_t> data[2];
  /* read data from 2 registers */
  for (int i = 0; i < 2; i++) {
    DeviceData::RegisterName register_name = static_cast<DeviceData::RegisterName>(i);
    data[i] = flash_access_->Read(GetRegisterSize(register_name), GetCRCOffset(register_name));

    /* check crc */
    CheckDataCRC(data[i]);

    /* remove crc from data */
    data[i].erase(data[i].begin(), data[i].begin() + kCRCSize);
  }

  std::vector<uint8_t> buf = data[0];
  buf.insert(buf.end(), data[1].begin(), data[1].end());

  return buf;
}

std::vector<uint8_t> DeviceData::ReadField(const std::string &name) {
  if (key_serial == name) {
    return flash_access_->ReadSerial();
  }
  ReadVersionFromOTP();
  DeviceData::RegisterName register_name = GetRegisterName(name);
  DataField field = GetDataField(register_name, name);

  /* read complete register into buf to check for crc and version */
  std::vector<uint8_t> buf = flash_access_->Read(GetRegisterSize(register_name),
                                                 GetCRCOffset(register_name));
  CheckDataCRC(buf);

  int data_field_position = field.offset - GetCRCOffset(register_name);

  /* Get data field from buf */
  std::vector<uint8_t> data(buf.begin() + data_field_position,
                            buf.begin() + data_field_position + field.size);

  return data;
}
