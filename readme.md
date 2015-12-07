# Introduction

It is sometimes useful to have integration tests run on an embedded device while development is ongoing.
This project implements the Cucumber wire protocol to direct [Cucumber](http://cucumber.io) step invocations over a serial port so that they run on the target device under test.

It allows you to use the native Cucumber gem to execute your scripts on your development machine, and executes their steps in a server process that redirects the command execution requests to a serial port to which you connect your target device.

# The components

The block diagram shows the components and their interaction:
![components](https://cloud.githubusercontent.com/assets/19006/11642643/10f6e6d0-9d0d-11e5-9a36-de58b092533c.png)

## Only so much magic supplied...

Whle the gateway portion is generic, the RPC server portion show here was written for the [NRF51](https://www.nordicsemi.com/eng/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822). This means that unless you have one, half of this project will not work for you.
To port it to your embedded device, you'd need to change the main() in RPC to open and interact accordingly with your device's UART.


# Customising the RPC listener

The RPC listener on the device needs to be cusomised to your needs. Its intended use is to drive on-board integration tests, so receiving and executing the test suites needs to be defined by you.
In order to customise the messaging protocol and execution parameters, you need to change the following two files:


On the server side:

1. wire-server-api.c - This file contains the step-definition literals that you want to support.
Change the entries in the api-table array. Here, specify the steps' text as well as the pointers to the functions that will execute once that step is invoked by Cucumber.


Still in this file, implement the function that will be called when its corresponding step is invoked by Cucumber. An example:
```c
	static int i\_have\_a\_red\_led(struct wire\_context *context)
	{
		char buff[20];	
		int retVal = serial\_write(context->serialPortHandle, "EXEC 0\n");
		serial\_read(context->serialPortHandle, buff, sizeof(buff));
		return(retVal);
	}
```

This will send "EXEC 0" to the device. Decide on your protocol and change accordingly.

On the device side:

1. In main.c, change the read loop to execute an API function or a series of calls under tests:


```c
	if (chr == '\n')
	{
		command_buffer[idx] = 0;
		idx = 0;
		**printf(command\_buffer);**
	}
```


Here, instead of printf(), execute a device API or test based on the parsed command. In our example, it would be in response to "EXEC 0".

# Building the components

## Building the gateway

```
mkdir build
cd build
cmake ..
make
```

From then on, you can simply use ```make```.

## Building the RPC listener

```
cd nrf51-rpc-server
make
```

Don't forget to flash your device with your modified code. You can use ```flash.sh```

# Running the gateway

Plug the target device into your USB port and issue

```ls /dev``` to see the port name assigned to it.

Use that port name as the first parameter, the second being the port number to listen on. This has to match the port number lisetd in the features/step\_definitions/cucumber.wire file.

```
cd build
cucumber\_tcp2serial /dev/tty.usbmodem1411 3901
```

This will start the server and you will see logs as it receives commands from the Cucumber gem.

# Running the RPC listener

It will automatically run, as the device will execute main() on powerup.

# Example Cucumber script

```cucumber
Feature: Hello World

	Scenario: Blink red LED
		Given I have a red led
		When I turn it on
		Then it's lit up
```

# Running example

The Cuke:
![nrf51 cucumber](https://cloud.githubusercontent.com/assets/19006/11642949/e1614972-9d0e-11e5-91d7-5330a9296ff6.png)

The server logs and the RPC replying (in red):
![nrf server](https://cloud.githubusercontent.com/assets/19006/11642997/266a27e6-9d0f-11e5-8df2-d1a46e57eeab.png)

# Contributing

1. [Fork it](https://github.com/ihassin/cucumber-wire-tcp2serial.git)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

# Code of conduct
Our code of conduct is [here](https://github.com/ihassin/cucumber-wire-tcp2serial/blob/master/CODE_OF_CONDUCT.md).

# License

MIT.
However, Nordic Semiconductor license applies to any NRF51 code used here.

