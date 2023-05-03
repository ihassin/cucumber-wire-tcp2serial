| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# ESP32 RPC Server Example

A simple example which has two LEDs: one red (on pin 14), one green (on pin 15).  Running the code when connected to `wire-server` will allow the "Hello World" test feature to run.

## How to use example

### Hardware Required

The example can be run on any ESP32, ESP32-S and ESP32-C series based development board connected to a computer with a single USB cable for flashing and monitoring.

### Setup the Hardware

Connect the external serial interface to the board as follows.

```
  -----------------------------------------------------------------------------------------
  | Target chip Interface | Kconfig Option     | Default ESP Pin      | External UART Pin |
  | ----------------------|--------------------|----------------------|--------------------
  | Transmit Data (TxD)   | EXAMPLE_UART_TXD   | GPIO4                | RxD               |
  | Receive Data (RxD)    | EXAMPLE_UART_RXD   | GPIO5                | TxD               |
  | Ground                | n/a                | GND                  | GND               |
  -----------------------------------------------------------------------------------------
```
Note: Some GPIOs can not be used with certain chips because they are reserved for internal use. Please refer to UART documentation for selected target.

Optionally, you can set-up and use a serial interface that has RTS and CTS signals in order to verify that the
hardware control flow works. Connect the extra signals according to the following table, configure both extra pins in
the example code `uart_echo_example_main.c` by replacing existing `UART_PIN_NO_CHANGE` macros with the appropriate pin
numbers and configure UART1 driver to use the hardware flow control by setting `.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS`
and adding `.rx_flow_ctrl_thresh = 122` to the `uart_config` structure.

```
  ---------------------------------------------------------------
  | Target chip Interface | Macro           | External UART Pin |
  | ----------------------|-----------------|--------------------
  | Transmit Data (TxD)   | ECHO_TEST_RTS   | CTS               |
  | Receive Data (RxD)    | ECHO_TEST_CTS   | RTS               |
  | Ground                | n/a             | GND               |
  ---------------------------------------------------------------
```

### Configure the project

Use the command below to configure project using Kconfig menu as showed in the table above.
The default Kconfig values can be changed such as: EXAMPLE_TASK_STACK_SIZE, EXAMPLE_UART_BAUD_RATE, EXAMPLE_UART_PORT_NUM (Refer to Kconfig file).
```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash
```

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

