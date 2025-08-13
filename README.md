# SDS-Examples

This repository contains examples that show the usage of the [Synchronous Data Streaming (SDS) Framework](https://github.com/ARM-software/SDS-Framework).

The SDS examples are configured for various Evaluation Boards and use the [MDK-Middleware](https://www.keil.arm.com/packs/mdk-middleware-keil/overview/) for the [SDSIO Interface](https://arm-software.github.io/SDS-Framework/main/sdsio.html). New hardware targets can be added using board layers that provide with required API interfaces.

## Directory Structure

Directory                                 | Content
:-----------------------------------------|:---------------------------------------------------------
[Alif/AppKit](./Alif/AppKit)              | SDS Framework deployed to Alif AppKit using USB communication.
[Alif/DevKit](./Alif/DevKit)              | SDS Framework deployed to Alif DevKit using Ethernet communication.
[ST/B-U585I-IOT02A](./ST/B-U585I-IOT02A)  | SDS Framework deployed to [ST B-U585I-IOT02A](https://www.keil.arm.com/boards/stmicroelectronics-b-u585i-iot02a-revc-c3bc599) using USB communication.
[Jupyter](./Jupyter)                      | Display SDS Data Files using a Jupyter notebook.
