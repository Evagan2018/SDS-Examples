# SDS Application on STMicroelectronics B-U585I-IOT02A board with SDSIO using USB Interface

This application is an example of using **Edge Impulse Continuous Motion Recognition ML algorithm** on a physical board.
It enables recording and playback of real-world data streams using physical hardware or just playback using simulation models (Arm Virtual Hardware - FVP).
The real-world data streams are captured in SDS data files.

## Pre-Requisite

To run this example:

- Install [Keil Studio for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack) and run a blinky example on the board to verify tool installation.
- Use `cpackget add ARM::SDS` to add the SDS Framework and [Setup the SDS Python utilities](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server).
- Setup the [Python environment](https://arm-software.github.io/SDS-Framework/main/utilities.html#setup) for running the SDS Utilities.

## STMicroelectronics B-U585I-IOT02A board

The [STMicroelectronics B-U585I-IOT02A](https://www.keil.arm.com/boards/stmicroelectronics-b-u585i-iot02a-revc-c3bc599/) board is based on a Cortex-M33 processor.

## Projects

The `SDS.csolution.yml` application is configured for the targets [Alif AppKit-E7-AIML](https://www.keil.arm.com/boards/alif-semiconductor-appkit-e7-aiml-gen-2-140e28d/guide/) or [AVH-SSE-300](https://github.com/ARM-software/AVH) FVP simulation models. It contains wto projects.

- **`DataTest.cproject.yml`**: Verifies the SDSIO interface on hardware.
- **`AlgorithmTest.cproject.yml`**: Verifies a Motion Recognition ML algorithm with recording and playback of SDS data files.

## Layer Type: Board

The board layer implements the Hardware Abstraction Layer (HAL) layer. Depending on the active target that is selected, a different board layers with I/O interfaces is used:

- `Board/B-U585I-IOT02A/Board.clayer.yml` uses the **USB Interface** for SDS file I/O on the development board.
- `Board/Corstone-300/Board.clayer.yml' uses the **VSI Interface** for SDS file I/O on AVH.

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

## DataTest Project on ST B-U585I-IOT02A board

The DataTest project allows you to verify the SDS I/O communication and it is recommended to use this project first.  Build and run this project in VS Code using the following steps:

1. Use **Manage Solution Settings** and select as Active Project **DataTest** with Build Type **DebugRec**.
2. **Build solution** creates the executable image.
3. Connect the USB debug port (J2) of the Alif AppKit and configure jumpers for SETOOLS.
5. **Load and Run** to download the application.
6. Configure jumpers for UART4 and use the VS Code **Serial Monitor** to observe the output below.

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

1. Connect a second USB cable between Host and Alif AppKit J1 connector.
2. Open in VS Code a Terminal Window and start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server) with `sdsio-server.py usb`
3. Open the VS Code **Serial Monitor** and Start Monitoring the UART output.
4. **Load and Run** the application on the Alif AppKit.
5. Press the joystick (SW2) on the Alif AppKit to start recording.
6. Press the joystick (SW2) again to stop recording.

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

Each run records two files: `DataInput.<n>.sds` and `DataOutput.<0>.sds` in the folder where SDSIO-Server was started. `<n>` is a sequential number.

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
3. Connect a second USB cable between Host and Alif AppKit J1 connector.
4. Open in VS Code a Terminal Window and start the [SDSIO-Server](https://arm-software.github.io/SDS-Framework/main/utilities.html#sdsio-server) with `sdsio-server.py usb`
5. **Load and Run** the application on the Alif AppKit hardware.
6. Press a joystick (SW2) on the Alif AppKit to start playback of `DataInput` and recording of `DataOutput`.
7. Wait for playback to finish, it will finish automatically when all data from `DataInput.0.sds` SDS file was played back.

The stream `DataInput.<n>.sds` is read back and the algorithm processes this data. The stream `DataOutput.<m>.sds` is written whereby `<m>` is the next available file index.
The files `DataOutput.<n>.sds` and `DataOutput.<m>.sds` should be identical. In this example the file pairs are `DataOutput.0.sds`/`DataOutput.2.sds`, and `DataOutput.1.sds`/`DataOutput.3.sds`.

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
