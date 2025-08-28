# SDS-Examples

This repository contains examples that show the usage of the [Synchronous Data Streaming (SDS) Framework](https://github.com/ARM-software/SDS-Framework).

The SDS examples are configured for various Evaluation Boards and use the [MDK-Middleware](https://www.keil.arm.com/packs/mdk-middleware-keil/overview/) for the [SDSIO Interface](https://arm-software.github.io/SDS-Framework/main/sdsio.html). New hardware targets can be added using board layers that provide the required API interfaces.

## Directory Structure

Directory                                 | Content
:-----------------------------------------|:---------------------------------------------------------
[.ci](./.ci)                              | Files that are related to the Continuous Integration (CI) tests of this BSP.
[.github/workflows](https://github.com/Open-CMSIS-Pack/STM32L496G-DISCO_BSP/tree/main/.github/workflows) | [GitHub Actions](#github-actions) scripts for build and execution tests.
[Alif/AppKit](./Alif/AppKit)              | SDS Framework deployed to Alif AppKit using USB communication.
[Alif/DevKit](./Alif/DevKit)              | SDS Framework deployed to Alif DevKit using Ethernet communication.
[ST/B-U585I-IOT02A](./ST/B-U585I-IOT02A)  | SDS Framework deployed to [ST B-U585I-IOT02A](https://www.keil.arm.com/boards/stmicroelectronics-b-u585i-iot02a-revc-c3bc599) using USB communication.
[Jupyter](./Jupyter)                      | Display SDS Data Files using a Jupyter notebook.

## Using VS Code

The examples are configured for [Keil Studio for VS Code](https://www.keil.arm.com/). Install [Keil Studio for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack) from the VS Code marketplace and run a blinky example for the related board first to verify tool installation.

> IMPORTANT
>
> - Each example has a local VS Code configuration. Use in VS Code **Open Folder** to open the folder of each project individually.

## Issues or Questions

Raise questions or issues for these examples on the repository [github.com/ARM-software/SDS-Framework](https://github.com/ARM-software/SDS-Framework/tree/main?tab=readme-ov-file#issues-and-labels).