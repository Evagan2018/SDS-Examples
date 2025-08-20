# SDS Application on STMicroelectronics B-U585I-IOT02A board with SDSIO using USB Interface

This application is an example of using **Edge Impulse Continuous Motion Recognition ML algorithm** on a physical board.
It enables recording and playback of real-world data streams using physical hardware or just playback using simulation models (Arm Virtual Hardware - FVP).
The real-world data streams are captured in SDS data files.

There are two SDS I/O interfaces available to the application:
- **USB Interface** is used for SDS file I/O on hardware.
- **VSI Interface** is used for SDS file I/O on AVH.

## Projects

- **`DataTest.cproject.yml`**: Verifies SDSIO interface on hardware.
- **`AlgorithmTest.cproject.yml`**: Verifies a EI Continuous Motion Recognition algorithm with recording (on hardware only) and playback of SDS data files.

## Build Targets

- **`DebugRec`**: Debug version of application used for recording of input data and algorithm output data.
- **`DebugPlay`**: Debug version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of algorithm.
- **`ReleaseRec`**: Release version of application used for recording of input data and algorithm output data.
- **`ReleasePlay`**: Release version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of algorithm.

> Note: Only difference between `Debug` and `Release` targets is compiler optimization level and debug information.

For more details, refer to the [SDS Template Application](https://arm-software.github.io/SDS-Framework/main/template.html).

## DataTest Project

**Input Data** : generated ramps simulating accelerometer data
**Algorithm** : simple checksum algorithm

### Configuration

It is possible to configure the input data bandwidth by editing `SDS_ALGO_TEST_BANDWIDTH` define in the `sds_algorithm_config.h` file.
Default bandwidth is configured to `100000U` which means approximately 100000 bytes per second.

## AlgorithmTest Project

**Input Data** : board accelerometer data
**Algorithm** : Edge Impulse Continuous Motion Recognition ML algorithm

### Usage on Hardware

#### Recording

For **recording** test, follow the steps below:

1. Connect USB cable between Host and development board
2. Start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server):
   ```bash
   python sdsio-server.py usb
   ```
3. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `STM32U585AIIx`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugRec` to record SDS data files.
4. Build and run the application on the hardware.
5. Press a blue button (B3 USER) to start recording.
6. Press a blue button (B3 USER) again to stop recording.

**Results**

Results should be two generated files: `DataInput.0.sds` and `DataOutput.0.sds` in the folder where SDSIO-Server was started.

`DataInput.0.sds` contains recorded input data.

`DataOutput.0.sds` contains recorded output data (results of the algorithm).

> Note: [SDS-Check](https://arm-software.github.io/SDS-Framework/main/utilities.html#sds-check) utility can be used for SDS file verification.

#### Playback

For **playback** test, follow the steps below:

1. Connect USB cable between Host and development board
2. Start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server):
   ```bash
   python sdsio-server.py usb
   ```
3. Open `SDS.csolution.yml` in Visual Studio Code:
   - Select **Target Type**: `STM32U585AIIx`
   - Choose **Project Name**: `DataTest` and **Build Type**: `DebugPlay` to playback/record SDS data files.
4. Build and run the application on the hardware.
5. Press a blue button (B3 USER) to start playback and recording.
6. Wait for playback to finish, it will finish automatically when all data from `DataInput.1.sds` SDS file was played back.

**Results**

Result should be one generated file `DataInput.1.sds` in the folder where SDSIO-Server was started.

`DataOutput.1.sds` contains recorded output data of the algorithm using input data played from `DataInput.0.sds` file.

To verify that the SDS component usage on hardware is reliable the two recordings of algorithm output data generated during recording and playback: 
`DataOutput.0.sds` and `DataOutput.1.sds` should be binary identical.
