# SDS-Examples
This repository contains examples that show how to use the Synchronous Data Streaming (SDS) framework.

## Directory Structure

Directory                                            | Content
:----------------------------------------------------|:---------------------------------------------------------
Hardware/StreamRecording.csolution.yml               | Record data in target hardware with various examples      
[Hardware/TestData](./Hardware/TestData)             | Record test data to validate configuration
[Hardware/ModelTest](./Hardware/ModelTest)           | Record data with ML model and test data
[Hardware/ModelSensor](./Hardware/ModelSensor)       | Record data with ML model and physical sensor data
[Simulate/TestData](./Simuate/TestData)              | Use recorded test data for input validation
[Simulate/ModelSensor](./Simulate/ModelSensor)       | Use recorded sensor data for ML model validation
[Boards](./Boards)                                   | Various board layers for different hardware targets
[Sensors](./Sensors)                                 | Interface to physical sensors
[SDS_Recorder](./SDS_Recorder)                       | SDS Recorder implementations
[SDS_Recorder/FileSystem](./SDS_Recorder/FileSystem) | SDS recording to memory card
[SDS_Recorder/Network](./SDS_Recorder/Network)       | SDS recording via Ethernet interface
[SDS_Recorder/USB](./SDS_Recorder/USB)               | SDS recording via USB VCom interface
[SDS_Recorder/USB](./SDS_Recorder/WiFi)              | SDS recording via WiFi interface
[SDS_Playback](./SDS_Playback)                       | SDS Playback implementations for simulation interface

