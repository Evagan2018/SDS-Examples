# SDS Application on STMicroelectronics B-U585I-IOT02A board with SDSIO using USB Interface

This application is an example of using [**Edge Impulse Continuous Motion Recognition ML algorithm**](https://studio.edgeimpulse.com/public/14299/latest) on a physical board.
It enables recording and playback of real-world data streams using physical hardware or just playback using simulation models (Arm Virtual Hardware - FVP).
The real-world data streams are captured in SDS data files.

## Pre-Requisite

To run this example:

- Install [Keil Studio for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack) and run a blinky example on the board to verify tool installation.
- Use `cpackget add ARM::SDS` to add the SDS Framework and [Setup the SDS Python utilities](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server).
- Setup the [Python environment](https://arm-software.github.io/SDS-Framework/main/utilities.html#setup) for running the SDS Utilities.

## STMicroelectronics B-U585I-IOT02A board

The [STMicroelectronics B-U585I-IOT02A](https://www.keil.arm.com/boards/stmicroelectronics-b-u585i-iot02a-revc-c3bc599/) board is based on a Cortex-M33 processor. For using the integrated ST-Link install the [ST-Link USB driver](https://www.st.com/en/development-tools/stsw-link009.html) or your computer and update the firmware.

## Projects

The `SDS.csolution.yml` application is configured for the targets [ST B-U585I-IOT02A board-E7-AIML](https://www.keil.arm.com/boards/alif-semiconductor-appkit-e7-aiml-gen-2-140e28d/guide/) or [AVH-SSE-300](https://github.com/ARM-software/AVH) FVP simulation models. It contains wto projects.

- **`DataTest.cproject.yml`**: Verifies the SDSIO interface on hardware.
- **`AlgorithmTest.cproject.yml`**: Verifies a Motion Recognition ML algorithm with recording and playback of SDS data files.

## Layer Type: Board and Layer Type: SDSIO

The board layer implements the Hardware Abstraction Layer (HAL) layer. Depending on the active target that is selected, a different board layer with I/O interfaces is used:

- `Board/B-U585I-IOT02A/Board.clayer.yml` and `sdsio_usb.clayer.yml` use the **USB Interface** for SDS file I/O on the development board.
- `Board/Corstone-300/Board.clayer.yml` and `sdsio_fvp.clayer.yml` use the **VSI Interface** for SDS file I/O on AVH FVP simulation.

## Layer Type: Edge Impulse layer

This layer contains the ML model that is used in the `AlgorithmTest.cproject.yml`.

## Projects

- **`DataTest.cproject.yml`**: Verifies SDSIO interface on hardware.
- **`AlgorithmTest.cproject.yml`**: Verifies a Continuous Motion Recognition algorithm with recording (on hardware only) and playback of SDS data files.

## Build Targets

- **`DebugRec`**: Debug version of application used for recording of input data and algorithm output data.
- **`DebugPlay`**: Debug version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of algorithm.
- **`ReleaseRec`**: Release version of application used for recording of input data and algorithm output data.
- **`ReleasePlay`**: Release version of application used for verification of SDS component, play back the previously recorded SDS file and verify results of algorithm.

> Note: Only difference between `Debug` and `Release` targets is compiler optimization level and debug information.

For more details, refer to the [SDS Template Application](https://arm-software.github.io/SDS-Framework/main/template.html).

## DataTest Project on ST B-U585I-IOT02A board

The DataTest project allows you to verify the SDS I/O communication and it is recommended to use this project first.  Build and run this project in VS Code using the following steps:

1. Use **Manage Solution Settings** and select as Active Project **DataTest** with Build Type **DebugRec**.
2. **Build solution** creates the executable image.
3. Connect to STLK USB connector of the ST B-U585I-IOT02A board and use the **`...`** menu **Target Information** to validate [ST-Link installation](https://www.st.com/en/development-tools/stsw-link009.html).
4. **Load and Run** to download the application.
5. Use the VS Code **Serial Monitor** to observe the output below.

```txt
SDS initialization failed!
For Network and USB SDSIO Interfaces ensure that SDSIO Server is running and restart the application!
99% idle
99% idle
```

> NOTE
>
> It is possible to configure the input data bandwidth by editing `SDS_ALGO_TEST_BANDWIDTH` define in the `sds_algorithm_config.h` file.
> Default bandwidth is configured to `100000U` which means approximately 100000 bytes per second.

### Recording Test

For executing the **recording** test, follow the steps below:

1. Connect a second USB cable between Host and ST B-U585I-IOT02A board USB-C connector.
2. Open in VS Code a Terminal Window and start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server) with `sdsio-server.py usb`
3. Open the VS Code **Serial Monitor** and Start Monitoring the UART output.
4. **Load and Run** the application on the ST B-U585I-IOT02A board.
5. Press the **User** button on the ST B-U585I-IOT02A board to start recording.
6. Press the **User** button again to stop recording.

**Output of SDSIO Server**

```bash
>sdsio-server.py usb 
Press Ctrl+C to exit.
Starting USB Server...
USB Server running.
Stream opened: DataInput (DataInput.0.sds).
Stream opened: DataOutput (DataOutput.0.sds).
..........
Stream closed: DataInput (DataInput.0.sds).
Stream closed: DataOutput (DataOutput.0.sds).
Stream opened: DataInput (DataInput.1.sds).
Stream opened: DataOutput (DataOutput.1.sds).
```

**Output of Serial Monitor**

```txt
SDS recording started
99% idle
SDS recording stopped
99% idle
SDS recording started
```

Each run records two files: `DataInput.<n>.sds` and `DataOutput.<n>.sds` in the folder where SDSIO-Server was started. `<n>` is a sequential number.

**Check SDS Files**

The [SDS-Check](https://arm-software.github.io/SDS-Framework/main/utilities.html#sds-check) utility verifies SDS files for consistency. For example:

```bash
>sds-check.py -s DataInput.0.sds
File     : DataInput.0.sds
DataSize : 351.400 bytes
Records  : 350
BlockSize: 996 bytes
Interval : 10 ms
DataRate : 99.600 byte/s
Jitter   : 0 ms
Validation passed
```

### Playback Test

For executing the **playback** test, follow the steps below:

1. Use **Manage Solution Settings** and select as Active Project **DataTest** with Build Type **DebugPlay**.
2. **Build solution** creates the executable image.
3. Connect a second USB cable between Host and ST B-U585I-IOT02A board STLK USB connector.
4. Open in VS Code a Terminal Window and start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server) with `sdsio-server.py usb`
5. **Load and Run** the application on the ST B-U585I-IOT02A board hardware.
6. Press a User button on the ST B-U585I-IOT02A board to start playback of `DataInput` and recording of `DataOutput`.
7. Wait for playback to finish, it will finish automatically when all data from `DataInput.0.sds` SDS file was played back.

The stream `DataInput.<n>.sds` is read back and the algorithm processes this data. The stream `DataOutput.<m>.sds` is written whereby `<m>` is the next available file index. 

To verify that the SDS component usage on hardware is reliable the two algorithm output data generated during recording and playback: the files `DataOutput.<n>.sds` and `DataOutput.<m>.sds` should be identical, e.g. `DataOutput.0.sds` and `DataOutput.1.sds` should be binary identical.

**Output of SDSIO Server**

```txt
>sdsio-server usb
SDSIO-Server USB running... Press Ctrl+C to exit.
Stream opened: DataInput (DataInput.0.sds).
Stream opened: DataOutput (DataOutput.2.sds).
...........
Stream closed: DataInput (DataInput.0.sds).
Stream closed: DataOutput (DataOutput.2.sds).
Stream opened: DataInput (DataInput.1.sds).
.
Stream opened: DataOutput (DataOutput.3.sds).
.......
Stream closed: DataInput (DataInput.1.sds).
Stream closed: DataOutput (DataOutput.3.sds).
```

## DataTest Project on AVH-FVP Simulation

The DataTest can be also executed on [AVH-FVP](https://github.com/ARM-software/AVH) simulation models using the steps below.

1. Use **Manage Solution Settings** and select:
     - Active target `AVH-SSE-300`
     - Active Project **DataTest** with Build Type **DebugRec** or **DebugPlay**.
2. **Build solution** to create an executable image.
3. **Load and Run** starts the application on the AVH-FVP simulation.  The output is shown in the Terminal console.

> NOTE
>
> The user button is simulated with the function `simGetSignal` in the file `sds_control.c`.

**Output of DebugRec**

```txt
*  Executing task: FVP_Corstone_SSE-300_Ethos-U55 -f Board/Corstone-300/fvp_config.txt -a out\DataTest\AVH-SSE-300\DebugRec\DataTest.axf  

97% idle
97% idle
SDS recording started
97% idle
  :
SDS recording stopped
 :
Info: /OSCI/SystemC: Simulation stopped by user.
Stream closed: DataOutput (DataOutput.3.sds).
```

**Output of DebugPlay**

```txt
*  Executing task: FVP_Corstone_SSE-300_Ethos-U55 -f Board/Corstone-300/fvp_config.txt -a out\DataTest\AVH-SSE-300\DebugPlay\DataTest.axf  

100% idle
100% idle
SDS playback and recording started
98% idle
  :
SDS playback and recording stopped
99% idle
  :
Info: /OSCI/SystemC: Simulation stopped by user.
```

## AlgorithmTest Project on ST B-U585I-IOT02A board

The AlgorithmTest project includes an EdgeImpulse Motion Recognition ML model that you can verify using the SDS-Framework.  Build and run this project in VS Code using the following steps:

1. Use **Manage Solution Settings** and select Active Target `B-U585-IOT02A board`, Active Project **AlgorithmTest** with Build Type **DebugRec**.
2. **Build solution** creates the executable image.
3. Use a Terminal window and start `sdsio-server usb`.
4. **Load and Run** to download the application.
5. Use the VS Code **Serial Monitor** to observe the output below.
6. On the B-U585-IOT02A board click on the User button to start/stop SDS recording

**Terminal shows sdsio-server output**

```bash
PS C:\SDS-Ex\SDS-Examples> sdsio-server usb
Press Ctrl+C to exit.
Starting USB Server...
USB Server running.
Stream opened: DataInput (DataInput.0.sds).
Stream opened: DataOutput (DataOutput.0.sds).
......
Stream closed: DataInput (DataInput.0.sds).
Stream closed: DataOutput (DataOutput.0.sds).
```


**Serial Monitor shows application output**

```bash
---- Opened the serial port COM5 ----
Predictions (DSP: 19.000000 ms., Classification: 0 ms., Anomaly: 1.000000ms.): 
#Classification results:
    idle: 0.996094
    snake: 0.000000
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: -0.356135
92% idle
96% idle
Predictions (DSP: 18.000000 ms., Classification: 1.000000 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.996094
    snake: 0.000000
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: -0.352785
93% idle
96% idle
96% idle
Predictions (DSP: 18.000000 ms., Classification: 1.000000 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.003906
    snake: 0.996094
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: -0.268574
92% idle
96% idle
Predictions (DSP: 18.000000 ms., Classification: 1.000000 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.000000
    snake: 0.992188
    updown: 0.007812
    wave: 0.000000
Anomaly prediction: 0.010583
92% idle
96% idle
Predictions (DSP: 18.000000 ms., Classification: 1.000000 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.000000
    snake: 0.894531
    updown: 0.101562
    wave: 0.000000
Anomaly prediction: -0.018911
92% idle
96% idle
Predictions (DSP: 18.000000 ms., Classification: 1.000000 ms., Anomaly: 1.000000ms.): 
#Classification results:
    idle: 0.187500
    snake: 0.812500
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: 0.209035
92% idle
96% idle
```
## AlgorithmTest Playback on AVH-FVP Simulation

To playback the recorded SDS data files use in VS Code the following steps:

1. Use **Manage Solution Settings** and select Active Target `AVH-SSE-300`, Active Project **AlgorithmTest** with Build Type **DebugPlay**.
2. **Build solution** creates the executable image.
3. **Load and Run** to runs the application on the simulator and uses the SDS datafiles previous captured.

**Terminal shows FVP simulation output**

```txt
Executing task: FVP_Corstone_SSE-300 -f Board/Corstone-300/fvp_config.txt -a out\AlgorithmTest\AVH-SSE-300\DebugPlay\AlgorithmTest.axf  

100% idle
100% idle
SDS playback and recording started
Predictions (DSP: 3.000000 ms., Classification: 0 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.527344
    snake: 0.472656
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: -0.008272
Predictions (DSP: 3.000000 ms., Classification: 0 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.007812
    snake: 0.992188
    updown: 0.000000
    wave: 0.000000
Anomaly prediction: -0.185833
Predictions (DSP: 3.000000 ms., Classification: 0 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.027344
    snake: 0.964844
    updown: 0.003906
    wave: 0.000000
Anomaly prediction: -0.294227
SDS playback and recording stopped
93% idle
100% idle
100% idle
100% idle
100% idle
100% idle
100% idle
100% idle
99% idle
100% idle
100% idle
100% idle
100% idle

Info: /OSCI/SystemC: Simulation stopped by user.
```
