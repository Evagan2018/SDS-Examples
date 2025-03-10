# About this application

This is an standalone application using [Edge Impulse](https://edgeimpulse.com/) Machine Learning and
[SDS Recorder](https://github.com/ARM-software/SDS-Framework) for recording ML model input and output data.

The example has trained model for continuous motion recognition using an tutorial from the Edge Impulse
(Tutorial: Continuous motion recognition).

This example is pre-configured for STMicroelectronics [B-U585I-IOT02A](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html) board.

It uses USB interface for recording the data on the host machine.

Model input data:

- const data containing 1 window of 2 seconds worth of data (375 float numbers) stored in an `features` array
  in the features.h` file

Model output data:

- results of inference executed every 2 seconds, containing probabilities for 4 possible classification groups

Examples of 20 second of recorder data are available in the `./SDS Recordings` sub-folder, containing
also `.sds.yml` metadata files.

## Running the example

Follow the steps below to try the example:

1. open the `SDS_examples.csolution.yml` solution
2. select `ModelTest` project as active project and build it
3. download the program to the `B-U585I-IOT02A` board
4. start the serial terminal application and connect to the `STMicroelectronics STLink Virtual COM Port (COMx)`
5. reset the board
6. start the [SDSIO-Server](https://github.com/ARM-software/SDS-Framework/tree/main/utilities/SDSIO-Server)
   using a command similar to below, just use the COM number of the `USB Serial Device` instead of `COM0`

    ```cmd
    python sdsio-server.py serial -p COM0 --baudrate 115200
    ```

7. press the USER (blue) push-button on the board to start the recording, LED1 will turn on
8. press the USER (blue) push-button on the board again to stop the recording, LED1 will turn off
9. there should be two recorded files `ModelInput.0.sds` and `ModelOutput.0.sds` in the same folder where the `SDSIO-Server` was launched

## Output

The output in STDIO (STLink Virtual COM Port) of this example running looks like below:

```txt
Edge Impulse standalone inferencing
Inferencing settings:
.Classifier interval: 16.00 ms.
.Input frame size: 375
.Raw sample count: 125 samples.
.Raw samples per frame: 3 samples.
.Number of output classes: 4
Predictions (DSP: 1.000000 ms., Classification: 0 ms., Anomaly: 0ms.): 
#Classification results:
    idle: 0.000000
    snake: 0.000000
    updown: 0.003906
    wave: 0.996094
Anomaly prediction: -0.183448
```
