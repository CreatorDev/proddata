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

# This script is a wrapper script to wifi calibration scripts.
# Script saves different wifi parameters to a file and later uses
# all parameters collectively to run continuous tx and rx tests.

wifi_param_file=/tmp/wifi_param
rx_statistics_file=/tmp/wifi_rx_statistics
max_txpower=28

usage()
{
cat << EOF

usage: $0 options

OPTIONS:
-h show this message
	Usage: sh $0 [options]

-s set number of spatial streams (1, 2)
-a select antenna to use (1, 2)
-c set channel number (1 to 165)
-b set bandwidth (20,40,60)
-d set data rate index (1 to 9 For MCS, 1 to 54 for Legacy)
-D set DCXO value (00 to FF)
-M set all max powers to maximum value
-o set power offset value (-100 to 100)
-p set tx power (in Hex)
-T start/stop Tx (start, stop)
-R start/stop Rx (start, stop)
-r get Rx statistics
-C reset to the defaults

EOF
}

if [ $# -eq 0 ]; then
	usage
	exit 1
fi

create_wifi_param_file () {
cat << EOF > $wifi_param_file
num_spatial_streams=1
antenna_sel=1
ch_num=7
bw=20
data_rate_index=54
tx_power=20
tx_power_offset=0

EOF
}

if [ ! -e "$wifi_param_file" ]; then
	create_wifi_param_file
fi

update_wifi_parameter () {
	local key=$1
	local value=$2
	sed -i "s/\($key *= *\).*/\1$value/" $wifi_param_file
}

find_wifi_standard () {
	# Warning this is specific to the current 1.2.3 testing spec. Currently the PC
	# test tool does not tell us what modulation to use so we need to work it out
	# with what we know. And we know is that we only use the highest datarate
	# available for the 11a, 11g and 11b and these datarates do not overlap MCS
	# datarates so we are able to work our the modulation from these data rates
	# the frequency band (2.4 or 5).
	local ch_num=$1
	local data_rate_index=$2
	# 2.4G
	if [ "$data_rate_index" -eq 11 ] && [ "$ch_num" -lt 30 ]; then
		standard=11b
	elif [ "$data_rate_index" -eq 54 ] && [ "$ch_num" -lt 30 ]; then
		standard=11g
	# 5G
	elif [ "$data_rate_index" -eq 54 ]; then
		standard=11a
	elif [ "$data_rate_index" -eq 9 ] || [ "$data_rate_index" -eq 8 ]; then
		standard=11ac
	# Both
	elif [ "$data_rate_index" -eq 7 ] || [ "$data_rate_index" -eq 5 ]; then
		standard=11n
	else
		echo "No valid modulation could be determined"
		exit 2
	fi
}

read_wifi_parameter () {
	while IFS="=" read -r key value; do
		case "$key" in
			"num_spatial_streams") num_spatial_streams="$value";;
			"antenna_sel") antenna_sel="$value";;
			"ch_num") ch_num="$value";;
			"bw") bw="$value";;
			"data_rate_index") data_rate_index="$value";;
			"tx_power") tx_power="$value";;
			"tx_power_offset") tx_power_offset="$value";;
		esac
	done < "$wifi_param_file"
}

transmission_test () {
	if [ "stop" = "$1" ]; then
		#TODO
		echo "need to add continuous_transmission_test stop functionality"
	elif [ "start" = "$1" ]; then
		read_wifi_parameter
		find_wifi_standard $ch_num $data_rate_index
		if [ 2 -eq "$num_spatial_streams" ]; then
			 echo /usr/bin/wifi_test/tx_dut_settings.sh $num_spatial_streams $ch_num \
				$standard $bw $data_rate_index
			/usr/bin/wifi_test/tx_dut_settings.sh $num_spatial_streams $ch_num \
				$standard $bw $data_rate_index
			dmesg | tail
		elif [ 1 -eq "$num_spatial_streams" ]; then
			echo /usr/bin/wifi_test/tx_dut_settings.sh $num_spatial_streams $antenna_sel \
				$ch_num $standard $bw $data_rate_index
			/usr/bin/wifi_test/tx_dut_settings.sh $num_spatial_streams $antenna_sel \
				$ch_num $standard $bw $data_rate_index
			dmesg | tail
		fi
		echo /usr/bin/wifi_test/set_pd_offset.sh $antenna_sel $ch_num $tx_power_offset
		/usr/bin/wifi_test/set_pd_offset.sh $antenna_sel $ch_num $tx_power_offset
		echo /usr/bin/wifi_test/set_txpower.sh $tx_power
		/usr/bin/wifi_test/set_txpower.sh $tx_power
		dmesg | tail
	fi
}

receiving_test () {
	if [ "stop" = "$1" ]; then
		#TODO
		echo "need to add receiving_test stop functionality"
	elif [ "start" = "$1" ]; then
		read_wifi_parameter
		echo "clearing stats"
		echo clear_stats=1 > /proc/uccp420/params
		if [ 2 -eq "$num_spatial_streams" ]; then
			echo /usr/bin/wifi_test/rx_dut_settings.sh $num_spatial_streams $ch_num $bw
			/usr/bin/wifi_test/rx_dut_settings.sh $num_spatial_streams $ch_num $bw
			dmesg | tail
		elif [ 1 -eq "$num_spatial_streams" ]; then
			echo /usr/bin/wifi_test/rx_dut_settings.sh $num_spatial_streams $antenna_sel $ch_num $bw
			/usr/bin/wifi_test/rx_dut_settings.sh $num_spatial_streams $antenna_sel $ch_num $bw
			dmesg | tail
		fi
	fi
}

get_rx_statistics () {
	local receive_duration=1
	read_wifi_parameter
	echo /usr/bin/wifi_test/per.sh OFDM $receive_duration
	/usr/bin/wifi_test/per.sh OFDM $receive_duration | tee $rx_statistics_file
}

while getopts "s:a:c:b:d:D:o:p:T:R:CMrh" opt; do
	case $opt in
		s)
			num_spatial_streams=$OPTARG
			update_wifi_parameter "num_spatial_streams" $num_spatial_streams;;
		a)
			antenna_sel=$OPTARG
			update_wifi_parameter "antenna_sel" $antenna_sel;;
		c)
			ch_num=$OPTARG
			update_wifi_parameter "ch_num" $ch_num;;
		b)
			bw=$OPTARG
			update_wifi_parameter "bw" $bw;;
		d)
			data_rate_index=$OPTARG
			update_wifi_parameter "data_rate_index" $data_rate_index;;
		D)
			dcxo=$OPTARG
			echo /usr/bin/wifi_test/set_dcxo.sh $dcxo
			/usr/bin/wifi_test/set_dcxo.sh $dcxo
			dmesg | tail;;
		o)
			tx_power_offset=$OPTARG
			update_wifi_parameter "tx_power_offset" $tx_power_offset;;
		p)
			tx_power=$OPTARG
			update_wifi_parameter "tx_power" $tx_power;;
		T)
			transmission_test $OPTARG;;
		R)
			receiving_test $OPTARG;;
		r)
			get_rx_statistics;;
		C)
			create_wifi_param_file;;
		M)
			echo /usr/bin/wifi_test/set_maxtxpower.sh $max_txpower
			/usr/bin/wifi_test/set_maxtxpower.sh $max_txpower
			dmesg | tail;;
		h)
			usage
			exit 0;;
		\?)
			usage
			exit 1;;
	esac
done
