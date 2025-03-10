# About this application

This is an standalone application using [Edge Impulse](https://edgeimpulse.com/) Machine Learning and
[SDS Player/Recorder](https://github.com/ARM-software/SDS-Framework) for playback of recorded input data to ML model
and recording of ML model output data.

The example has trained model for continuous motion recognition using an tutorial from the Edge Impulse
(Tutorial: Continuous motion recognition).

This example is pre-configured for STMicroelectronics [B-U585I-IOT02A](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html) board.

It uses USB interface for playback/recording the data from/on the host machine.

Model input data:

- recorded data in `./SDS Recordings/ModelInput.0.sds` file

Model output data:

- results of inference executed every 2 seconds, containing probabilities for 4 possible classification groups

Examples of 20 second of previously recorded model input data used for playback as well as
recorded model output data are available in the `./SDS Recordings` sub-folder, containing also
`.sds.yml` metadata files.

## Running the example

Follow the steps below to try the example:

1. open the `SDS_examples.csolution.yml` solution
2. select `ModelPlay` project as active project and build it
3. download the program to the `B-U585I-IOT02A` board
4. start the serial terminal application and connect to the `STMicroelectronics STLink Virtual COM Port (COMx)`
5. reset the board
6. copy the `ModelInput.0.sds` file from `./SDS Recordings` to location where `SDSIO-Server` is located
7. start the [SDSIO-Server](https://github.com/ARM-software/SDS-Framework/tree/main/utilities/SDSIO-Server)
   using a command similar to below, just use the COM number of the `USB Serial Device` instead of `COM0`

    ```cmd
    python sdsio-server.py serial -p COM0 --baudrate 115200
    ```

8. press the USER (blue) push-button on the board to start the playback, LED1 will turn on
9. when all the data from `ModelInput.0.sds` file is played back the playback will stop, LED1 will turn off
10. there should be one recorded file `ModelOutput.0.sds` in the same folder where the `SDSIO-Server` was launched

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
