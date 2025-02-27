# SDS-Examples
This repository contains examples that show how to use the Synchronous Data Streaming (SDS) framework.

## Directory Structure

Directory                                              | Content
:------------------------------------------------------|:---------------------------------------------------------
Hardware/StreamRecording.csolution.yml                 | Record data in target hardware with various examples
[Hardware/DataTest](./Hardware/DataTest)               | Record test data to validate configuration
[Hardware/ModelTest](./Hardware/ModelTest)             | Record data with ML model and test data
[Hardware/ModelSensor](./Hardware/ModelSensor)         | Record data with ML model and physical sensor data
[Simulate/TestData](./Simuate/TestData)                | Use recorded test data for input validation
[Simulate/ModelSensor](./Simulate/ModelSensor)         | Use recorded sensor data for ML model validation
[Boards](./Boards)                                     | Various board layers for different hardware targets
[Sensors](./Sensors)                                   | Interface to physical sensors
[SDS_Interface](./SDS_Interface)                       | SDS Interface implementations
[SDS_Interface/FileSystem](./SDS_Interface/FileSystem) | SDS Interface (SDS_IO via file system)
[SDS_Interface/Network](./SDS_Interface/Network)       | SDS Interface (SDS_IO via network)
[SDS_Interface/USB](./SDS_Interface/USB)               | SDS Interface (SDS_IO via USB Device - VCOM)
[SDS_Interface/WiFi](./SDS_Interface/WiFi)             | SDS Interface (SDS_IO via WiFi)

