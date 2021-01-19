# Edge Impulse firmware for Eta Compute ECM3532 AI Sensor

Edge Impulse enables developers to create the next generation of intelligent device solutions with embedded Machine Learning. This repository contains the Edge Impulse firmware for the Eta Compute ECM3532 AI Sensor development board. This device supports all Edge Impulse device features, including ingestion, remote management and inferencing.

> **Note:** Do you just want to use this development board with Edge Impulse? No need to build this firmware. See the instructions [here](https://docs.edgeimpulse.com/docs/eta-compute-ecm3532-ai-sensor) for a prebuilt image and instructions. Or, you can use the [data forwarder](https://docs.edgeimpulse.com/docs/cli-data-forwarder) to capture data from any sensor.

## Requirements

### Hardware

* [Eta Compute ECM3532 AI Sensor](https://etacompute.com/products/) development board.
* [Sparkfun FTDI Basic Breakout](https://www.sparkfun.com/products/9873) breakout board, or a similar FTDI to USB board that supports 3.3V - the ECM3532 AI Sensor is not 5V tolerant.

### Software

* [Node.js 12](https://nodejs.org/en/download/) or higher.
* [GNU Make](https://www.gnu.org/software/make/).
* [GNU ARM Embedded Toolchain 9-2019-q4-major](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) - make sure `arm-none-eabi-gcc` is in your PATH.
* [Edge Impulse CLI](https://docs.edgeimpulse.com/docs/cli-installation).

    You can install this via npm:

    ```
    $ npm install edge-impulse-cli@latest -g
    ```

## Building the application

1. Your development board should already have the bootloader present. If this is not the case:
    * Grab [bootloader/bootloader.bin](bootloader/bootloader.bin).
    * Connect a J-Link to J2.
    * Open JFlashLite (in the [JLink software and documentation pack](https://www.segger.com/downloads/jlink/)) and flash the bootloader to address `0x01000000`.

        ![Flash bootloader](images/flash_bootloader.png)

1. Build the application:

    ```
    $ cd soc/ecm3532/boards/eta_ai_sensor/examples/m3/edge-impulse-ingestion/gcc/flash_bootloader
    $ make
    ```

1. Flash the application:

    ```
    $ eta-flash-tool --firmware-path bin/edge_impulse_ingestion.bin
    ```
