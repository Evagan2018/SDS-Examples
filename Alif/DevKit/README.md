# SDS Application on Alif DevKit board with SDSIO using Ethernet Interface

This application provides a test framework for DSP and ML algorithms. It enables recording and playback of real-world data streams using physical hardware
or simulation models (Arm Virtual Hardware - FVP) to an user algorithm under test. The real-world data streams are captured in SDS data files.

There are two SDS I/O interfaces available to the application:
- **Ethernet Interface** is used for SDS file I/O on hardware.
- **VSI Interface** is used for SDS file I/O on AVH.

## Projects

- **`DataTest.cproject.yml`**: Verifies SDSIO interface on hardware.
- **`AlgorithmTest.cproject.yml`**: Verifies a user algorithm with recording and playback of SDS data files.

## Build Targets

- **`DebugRec`**: Debug version of application used for recording of generated input data and results of simple checksum algorithm output data.
- **`DebugPlay`**: Debug version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of simple checksum algorithm.
- **`ReleaseRec`**: Release version of application used for recording of generated input data and results of simple checksum algorithm output data.
- **`ReleasePlay`**: Release version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of simple checksum algorithm.

> Note: Only difference between `Debug` and `Release` targets is compiler optimization level and debug information.

For more details, refer to the [SDS Template Application](https://arm-software.github.io/SDS-Framework/main/template.html).

## DataTest Project

### Configuration

It is possible to configure the input data bandwidth by editing `SDS_ALGO_TEST_BANDWIDTH` define in the `sds_algorithm_config.h` file.
Default bandwidth is configured to `100000U` which means approximately 100000 bytes per second.

### Usage on Hardware

#### Recording

For **recording** test, follow the steps below:

1. Connect development board to the local network with Ethernet cable
2. Start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server):
   ```bash
   python sdsio-server.py socket
   ```
3. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `AE722F80F55D5LS`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugRec` to record SDS data files.
4. Open `./sdsio/network/RTE/SDS/sdsio_config_socket.h` file and edit `SDSIO_SOCKET_SERVER_IP` value to one reported by SDSIO-Server
   For more details see [documentation](./sdsio/network/README.md)
5. Build and run the application on the hardware.
6. Press a joystick (SW2) to start recording.
7. Press a joystick (SW2) again to stop recording.

**Results**

Results should be two generated files: `DataInput.0.sds` and `DataOutput.0.sds` in the folder where SDSIO-Server was started.

`DataInput.0.sds` contains recorded generated test data representing input data to a simple checksum algorithm.

`DataOutput.0.sds` contains recorded output data of a simple checksum algorithm.

> Note: [SDS-Check](https://arm-software.github.io/SDS-Framework/main/utilities.html#sds-check) utility can be used for SDS file verification.

#### Playback

For **playback** test, follow the steps below:

1. Connect development board to the local network with Ethernet cable
2. Start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server):
   ```bash
   python sdsio-server.py socket
   ```
3. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `AE722F80F55D5LS`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugPlay` to playback/record SDS data files.
4. Open `./sdsio/network/RTE/SDS/sdsio_config_socket.h` file and edit `SDSIO_SOCKET_SERVER_IP` value to one reported by SDSIO-Server
   For more details see [documentation](./sdsio/network/README.md)
5. Build and run the application on the hardware.
6. Press a joystick (SW2) to start playback and recording.
7. Wait for playback to finish, it will finish automatically when all data from `DataInput.1.sds` SDS file was played back.

**Results**

Result should be one generated file `DataInput.1.sds` in the folder where SDSIO-Server was started.

`DataOutput.1.sds` contains recorded output data of simple checksum algorithm using input data played from `DataInput.0.sds` file.

To verify that the SDS component usage on hardware is reliable the two recordings of algorithm output data generated during recording and playback: 
`DataOutput.0.sds` and `DataOutput.1.sds` should be binary identical.


### Usage on Simulator

#### Recording

For **recording** test, follow the steps below:

1. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `AVH-SSE-300`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugRec` to record SDS data files.
2. Build and run the application on the simulator
3. Activate/deactivate a virtual button (vioButton0) to start recording.
4. Activate/deactivate a virtual button (vioButton0) again to stop recording.

**Results**

Results should be two generated files: `DataInput.0.sds` and `DataOutput.0.sds` in the folder where SDSIO-Server was started.

`DataInput.0.sds` contains recorded generated test data representing input data to a simple checksum algorithm.

`DataOutput.0.sds` contains recorded output data of a simple checksum algorithm.

#### Playback

For **playback** test, follow the steps below:

1. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `AVH-SSE-300`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugPlay` to playback/record SDS data files.
2. Build and run the application on the simulator.
3. Activate/deactivate a virtual button (vioButton0) to start playback and recording.
4. Wait for playback to finish, it will finish automatically when all data from `DataInput.1.sds` SDS file was played back.

**Results**

Result should be one generated file `DataInput.1.sds` in the folder where SDSIO-Server was started.

`DataOutput.1.sds` contains recorded output data of simple checksum algorithm using input data played from `DataInput.0.sds` file.

To verify that the SDS component usage on hardware is reliable the two recordings of algorithm output data generated during recording and playback: 
`DataOutput.0.sds` and `DataOutput.1.sds` should be binary identical.
