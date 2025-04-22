# Network SDS Interface

The Network SDS Interface uses the Ethernet-based IoT Socket communication implemented with the MDK-Middleware Network.
It is based on the following components:

- [SDS Recorder](https://github.com/ARM-software/SDS-Framework/tree/main/sds#synchronous-data-stream-recorder) data streaming,
- [IoT Socket](https://mdk-packs.github.io/IoT_Socket/latest/index.html) communication interface,
- [MDK-Middleware Network](https://arm-software.github.io/MDK-Middleware/latest/Network/index.html) protocol stack,
- [CMSIS-Driver Ethernet](https://arm-software.github.io/CMSIS_6/latest/Driver/index.html) physical interface.

## SDS Configuration

The following SDS components are required:

- `SDS:Buffer`
- `SDS:IO:Socket`
- `SDS:RecPlay&CMSIS-RTOS2`

The IP address of the SDSIO server must be updated in `sdsio_config_socket.h` with the
address reported by the SDSIO server at startup:

```c
#define SDSIO_SERVER_IP           "0.0.0.0"
```

You can leave the other configuration settings at their default values.

## IoT Socket Configuration

The following IoT Utility components are required:

- `IoT Utility:Socket:MDK Network`

No other configuration settings are required.

## Network Configuration

The following MDK-Middleware Network components are required:

- `Network&MDK:CORE`
- `Network&MDK:Interface:ETH`
- `Network&MDK:Service:DNS Client`
- `Network&MDK:Socket:BSD`
- `Network&MDK:Socket:TCP`
- `Network&MDK:Socket:UDP`

The communication does not use the **IPv6** protocol, therefore the variant `IPv4 only` can be selected in `Net_Config.h`:

```c
#define NET_CORE_VARIANT        0
```

You can leave the other configuration settings at their default values.

## Starting SDSIO server

SDSIO server is a Python-based SDS I/O server utility for PC included in the
[SDS-Framework](https://github.com/ARM-software/SDS-Framework/tree/main/documentation/utilities.md#sdsio-server) pack.

To start the SDSIO server, run it from the SDSIO-Server root folder with:

```cmd
python sdsio-server.py socket
```

This activates the server with the default settings. Further information about the SDSIO server application
can be found in [SDS-Framework](https://github.com/ARM-software/SDS-Framework/tree/main/documentation/utilities.md#sdsio-server).

***Note***

- The evaluation board and the PC must be in the same network in order to establish a connection.
