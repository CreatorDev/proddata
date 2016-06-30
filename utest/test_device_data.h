/**
 * @file
 * Testsuite for DeviceData
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

#include <cxxtest/TestSuite.h>
#include <glog/logging.h>
#include <vector>
#include "flash_access_mock.h"
#include "device_data.h"

using ::testing::_;
using ::testing::Return;

class DeviceDataTestSuite : public CxxTest::TestSuite {
 private:
  std::unique_ptr<FlashAccess> flash_access;
  MockFlashAccess *flash_mock;
  DeviceData *device_data;

 public:
  DeviceDataTestSuite() {
    google::InitGoogleLogging("DeviceData utest");
  }

  ~DeviceDataTestSuite() {
    google::ShutdownGoogleLogging();
  }

  void setUp() {
    std::unique_ptr<FlashAccess> flash_access(new MockFlashAccess);
    flash_mock = static_cast<MockFlashAccess *>(flash_access.get());
    device_data = new DeviceData(std::move(flash_access));
  }

  void tearDown() {
    delete device_data;
  }

  void TestWriteVersion1Expected() {
    /* Each Write operation will invoke flash write twice. one for reg0 data and one for reg1 data*/
    std::vector<uint8_t> data(39, 0x01);

    EXPECT_CALL(*flash_mock, Write(_, _)).Times(2);
    device_data->Write(data);
  }

  void TestWriteDataSizeError() {
    std::vector<uint8_t> data(8, 0x01);
    TS_ASSERT_THROWS_EQUALS(device_data->Write(data),
                            std::exception &e, e.what(), "Data size error");
  }

  void TestWriteFieldExpected() {
    unsigned char old_reg1_buf[] = {0x5d, 0x15, 0x01, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                    0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                    0x28, 0x29};
    std::vector<uint8_t> old_reg1_data(old_reg1_buf, old_reg1_buf +
                                       sizeof(old_reg1_buf)/sizeof(old_reg1_buf[0]));
    std::vector<uint8_t> old_dcxo_value(1, 0x11);


    /* new_reg1_buf has updated dcxo value '0x1A' instead of '0x11', also the crc is changed */
    unsigned char new_reg1_buf[] = {0x78, 0xb3, 0x01, 0x1A, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                    0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                   0x28, 0x29};
    std::vector<uint8_t> new_reg1_data(new_reg1_buf, new_reg1_buf +
                                       sizeof(new_reg1_buf)/sizeof(new_reg1_buf[0]));
    std::vector<uint8_t> new_dcxo_value(1, 0x1A);

    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);

    /* read dcxo value */
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(old_reg1_data));

    TS_ASSERT_EQUALS(device_data->ReadField("DCXO"), old_dcxo_value);

    /* read new dcxo value */
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(new_reg1_data));
    TS_ASSERT_EQUALS(device_data->ReadField("DCXO"), new_dcxo_value);

    /* write new dcxo value */
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(old_reg1_data));
    EXPECT_CALL(*flash_mock, Write(new_reg1_data, _)).Times(1);
    device_data->WriteField("DCXO", new_dcxo_value);
  }

  void TestWriteFieldDataSizeError() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    std::vector<uint8_t> data(3, 0x11);
    TS_ASSERT_THROWS_EQUALS(device_data->WriteField("DCXO", data),
                            std::exception &e, e.what(), "Invalid field size");
  }

  void TestWriteFieldInvalidField() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    std::vector<uint8_t> val(1, 0x00);
    TS_ASSERT_THROWS_EQUALS(device_data->WriteField("IP", val),
                            std::exception &e, e.what(), "Invalid data field: IP");
  }

  void WriteVersionThrowsError() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    std::vector<uint8_t> val(1, 0x00);
    TS_ASSERT_THROWS_EQUALS(device_data->WriteField("VERSION_REG0", val),
                            std::exception &e, e.what(), "Cannot modify register version");
  }

  void TestWriteFieldInvalidReg0Version() {
    std::vector<uint8_t> reg0_version_(1, 0x00);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    std::vector<uint8_t> dcxo_value(1, 0x1A);
    TS_ASSERT_THROWS_EQUALS(device_data->WriteField("DCXO", dcxo_value),
                            std::exception &e, e.what(), "No valid reg version");
  }

  void TestWriteFieldInvalidReg1Version() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x00);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    std::vector<uint8_t> dcxo_value(1, 0x1A);
    TS_ASSERT_THROWS_EQUALS(device_data->WriteField("DCXO", dcxo_value),
                            std::exception &e, e.what(), "No valid reg version");
  }

  void TestReadExpected() {
    /* Read data with valid CRC (note: 1st 2 bytes are CRC) */
    /* Each Read operation will invoke flash read twice. one for reg0 data and one for reg1 data */
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);

    unsigned char buf[] = {0x76, 0xAA, 0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    std::vector<uint8_t> data(buf, buf + 11);

    EXPECT_CALL(*flash_mock, Read(_, _)).Times(4)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(data))
        .WillOnce(Return(data));

    device_data->Read();
  }

  void TestReadVersionFailed() {
    std::vector<uint8_t> reg0_version_(1, 0x00);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    TS_ASSERT_THROWS_EQUALS(device_data->Read(),
                            std::exception &e, e.what(), "No valid reg version");
  }

  void TestReadCRCFailure() {
    /* Read data with wrong CRC (note: 1st 2 byets are CRC) */
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    unsigned char buf[] = {0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    std::vector<uint8_t> data(buf, buf + 11);

    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(data));

    TS_ASSERT_THROWS_EQUALS(device_data->Read(),
                            std::exception &e, e.what(), "Data corrupted:CRC failed");
  }

  void TestReadField() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);

    unsigned char reg0_buf[] = {0x7e, 0x6e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11,
                               0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x33,
                               0x33, 0x33, 0x33, 0x33, 0x33, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                               0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    unsigned char reg1_buf[] = {0x5d, 0x15, 0x01, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29};

    std::vector<uint8_t> reg0_data(reg0_buf, reg0_buf + 39);
    std::vector<uint8_t> reg1_data(reg1_buf, reg1_buf + 22);

    /* reg0 data field */
    std::vector<uint8_t> mac0(6, 0x00);

    /* reg1 data field */
    std::vector<uint8_t> DCXO(1, 0x11);

    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(reg0_data));

    TS_ASSERT_EQUALS(device_data->ReadField("MAC_0"), mac0);

    EXPECT_CALL(*flash_mock, Read(_, _)).Times(3)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_))
        .WillOnce(Return(reg1_data));

    TS_ASSERT_EQUALS(device_data->ReadField("DCXO"), DCXO);
  }

  void TestReadFieldInvalidField() {
    std::vector<uint8_t> reg0_version_(1, 0x01);
    std::vector<uint8_t> reg1_version_(1, 0x01);
    EXPECT_CALL(*flash_mock, Read(_, _)).Times(2)
        .WillOnce(Return(reg0_version_))
        .WillOnce(Return(reg1_version_));

    TS_ASSERT_THROWS_EQUALS(device_data->ReadField("IP"),
                            std::exception &e, e.what(), "Invalid data field: IP");
  }

  void TestReadSerial() {
    std::vector<uint8_t> serial(8, 0x11);
    EXPECT_CALL(*flash_mock, ReadSerial()).Times(1).WillOnce(Return(serial));
    TS_ASSERT_EQUALS(device_data->ReadField("SERIAL"), serial);
  }
};
