#!/bin/sh
#
# Copyright 2015 by Imagination Technologies Limited and/or its affiliated group companies.
#
# All rights reserved.  No part of this software, either
# material or conceptual may be copied or distributed,
# transmitted, transcribed, stored in a retrieval system
# or translated into any human or computer language in any
# form by any means, electronic, mechanical, manual or
# other-wise, or disclosed to the third parties without the
# express written permission of Imagination Technologies
# Limited, Home Park Estate, Kings Langley, Hertfordshire,
# WD4 8LZ, U.K.

# Script to write data to OTP and cross check it by reading from OTP using Proddata tool


proddata=/usr/bin/proddata

MAC_0="000000000000"
MAC_1="111111111111"
MAC_2="222222222222"
MAC_3="333333333333"
MAC_4="444444444444"
MAC_5="555555555555"


mac_version="01"
wifi_version="01"
mac_data_write="000000000000111111111111222222222222333333333333444444444444555555555555"
wifi_data_write="11121314151617181920212223242526272829"
data_payload=$mac_version$mac_data_write$wifi_version$wifi_data_write

write_otp()
{
    $proddata write $data_payload
}

check_otp()
{

    # mac data check
    mac0=$($proddata read MAC_0)
    mac1=$($proddata read MAC_1)
    mac2=$($proddata read MAC_2)
    mac3=$($proddata read MAC_3)
    mac4=$($proddata read MAC_4)
    mac5=$($proddata read MAC_5)

    if [ $mac0 == $MAC_0 ] && [ $mac1 == $MAC_1 ] && [ $mac2 == $MAC_2 ]  && [ $mac3 == $MAC_3 ]  && [ $mac4 == $MAC_4 ]  && [ $mac5 == $MAC_5 ]
    then
        echo "mac data check successful"
    else
        echo "mac data check failed"
    fi

    # wifi data check
    a=$($proddata read DCXO)
    b=$($proddata read DSSS_TX_POWER_2.4)
    c=$($proddata read OFDM_TX_POWER_2.4_MCS7)
    d=$($proddata read OFDM_TX_POWER_5B1_MCS5)
    e=$($proddata read OFDM_TX_POWER_5B2_MCS5)
    f=$($proddata read OFDM_TX_POWER_5B3_MCS5)
    g=$($proddata read OFDM_TX_POWER_5B4_MCS5)
    h=$($proddata read OFDM_TX_POWER_5B1_MCS7)
    i=$($proddata read OFDM_TX_POWER_5B2_MCS7)
    j=$($proddata read OFDM_TX_POWER_5B3_MCS7)
    k=$($proddata read OFDM_TX_POWER_5B4_MCS7)
    l=$($proddata read OFDM_TX_POWER_5B1_MCS8)
    m=$($proddata read OFDM_TX_POWER_5B2_MCS8)
    n=$($proddata read OFDM_TX_POWER_5B3_MCS8)
    o=$($proddata read OFDM_TX_POWER_5B4_MCS8)
    p=$($proddata read OFDM_TX_POWER_5B1_MCS9)
    q=$($proddata read OFDM_TX_POWER_5B2_MCS9)
    r=$($proddata read OFDM_TX_POWER_5B3_MCS9)
    s=$($proddata read OFDM_TX_POWER_5B4_MCS9)

    wifi_data_read=$a$b$c$d$e$f$g$h$i$j$k$l$m$n$o$p$q$r$s
    if [ $wifi_data_write == $wifi_data_read ]
    then
        echo "wifi data check successful"
    else
        echo "wifi data check failed"
    fi

   # whole payload check
    payload=$($proddata read)

    if [ $data_payload == $payload ]
    then
        echo "data payload check successful"
    else
        echo "data payload check failed"
    fi
}


write_otp
check_otp
