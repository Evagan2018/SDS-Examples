# Copyright (c) 2022 Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# SDS-View

import argparse
import struct
import sys

import matplotlib.pyplot as plt
import numpy as np
import yaml


class RecordManager:
    def __init__(self):
        self.HEADER_SIZE    = 8
        self.TIMESTAMP_SIZE = 4
        self.data = bytearray()

    # Flush data buffer
    def flush(self):
        self.data = bytearray()

    # Private function for retrieving data from record
    def __getRecord(self, file):
        record = bytearray(file.read(self.HEADER_SIZE))
        if len(record) == self.HEADER_SIZE:
            timestamp = struct.unpack("I", record[:self.TIMESTAMP_SIZE])[0]
            data_size = struct.unpack("I", record[self.TIMESTAMP_SIZE:])[0]
            self.data.extend(bytearray(file.read(data_size)))
            return True
        else:
            return False

    # Extract all data from recording file
    def getData(self, file):
        record = True
        while record:
            record = self.__getRecord(file)
        return self.data


# Convert C style data type to Python style
def getDataType(data_type):
    if   data_type == "int16_t":
        d_type = "h"
    elif data_type == "uint16_t":
        d_type = "H"
    elif data_type == "int32_t":
        d_type = "i"
    elif data_type == "uint32_t":
        d_type = "I"
    elif data_type == "float":
        d_type = "f"
    elif data_type == "double":
        d_type = "d"
    else:
        print(f"Unknown data type: {data_type}\n")
        d_type = "I"

    return d_type

# Open SDS data file in read mode
def openFile(file_name):
    try:
        if ".yml" in file_name:
            file = open(file_name, "r")
        else:
            file = open(file_name, "rb")
        return file
    except Exception as e:
        print(f"Error in openFile({file_name}): {e}")
        sys.exit(1)

# Close file
def closeFile(file_name):
    try:
        file_name.close()
    except Exception as e:
        print(f"Error in closeFile({file_name}): {e}")
        sys.exit(1)

# Create new figure and plot content
def plotData(all_data, data_desc, freq, title, view3D):
    dim = {}
    desc_n = 0
    desc_n_max = len(data_desc)

    # Create a new figure for each .sds file
    fig = plt.figure()
    for desc in data_desc:
        # Extract parameters from description in YAML file
        if "unit" in desc:
            unit = desc["unit"]
        else:
            unit = "raw"

        if "scale" in desc:
            scale = desc["scale"]
        else:
            scale = 1

        if "offset" in desc:
            offset = desc["offset"]
        else:
            offset = 0

        if "type" in desc:
            d_type = getDataType(desc["type"])
        else:
            sys.exit(1)

        # Calculate number of bytes needed for decoding the data in .sds file
        d_byte = struct.calcsize(d_type)
        # Disunite raw data into a list of data points according to the number of bytes needed for each data point
        tmp_data = [all_data[i:(i + d_byte)] for i in range(0, len(all_data), d_byte)]
        # Keep only every n-th data point
        tmp_data = tmp_data[desc_n::desc_n_max]
        # Decode retrieved data points
        data = struct.unpack(f"{int(len(tmp_data))}{d_type}", b''.join(tmp_data))
        # Scale and offset data points
        scaled_data = [((x * scale) + offset) for x in data]

        # Generate timestamps using number of data points and sampling frequency
        t = np.arange(0, len(data) / freq, 1 / freq)
        if len(t) > len(data):
            # Truncate timestamps to match the number of data points
            t = t[0:len(data)]
        plt.plot(t, scaled_data, label=desc["value"])

        # Store data points in a dictionary for later use when there are 3 axes described
        if view3D and (desc_n_max == 3):
            dim[desc_n] = scaled_data

        # Increment description number
        desc_n += 1

    plt.grid(linestyle=":")
    plt.title(title)
    plt.xlabel("seconds")
    plt.ylabel(unit)
    plt.legend()

    # Create a 3D view when there are 3 axes available
    if view3D and (desc_n_max == 3):
        fig3d = plt.figure()
        ax3d = fig3d.add_subplot(projection="3d")
        ax3d.plot(dim[0], dim[1], dim[2])
        ax3d.set_title(f"{title} - 3D")
        ax3d.set_xlabel(f"{data_desc[0]['value']} [{data_desc[0]['unit']}]")
        ax3d.set_ylabel(f"{data_desc[1]['value']} [{data_desc[1]['unit']}]")
        ax3d.set_zlabel(f"{data_desc[2]['value']} [{data_desc[2]['unit']}]")
