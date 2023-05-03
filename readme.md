# Introduction

It is sometimes useful to have integration tests run on an embedded device while development is ongoing.
This project implements the Cucumber wire protocol to direct [Cucumber](http://cucumber.io) step invocations over a serial port so that they run on the target device under test.

It allows you to use the native Cucumber gem to execute your scripts on your development machine, and executes their steps in a server process that redirects the command execution requests to a serial port to which you connect your target device.

# The components

The block diagram shows the components and their interaction:
![arch](https://cloud.githubusercontent.com/assets/19006/11671751/74d61006-9dd8-11e5-95ad-8f1d1e23f327.png)

## Only so much magic supplied...

Whle the gateway portion is generic, the RPC server portion show here was written for the [NRF51](https://www.nordicsemi.com/eng/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822). This means that unless you have one, half of this project will not work for you.
To port it to your embedded device, you'd need to change the main() in RPC to open and interact accordingly with your device's UART.


# Customising the RPC Server

The RPC Server runs on the device and needs to be customised to your needs. Its intended use is to drive on-board integration tests, so handling the received requests and executing the test suites needs to be defined by you.
In order to customise the messaging protocol and execution parameters, you need to change the following two files:


## On the server side:

wire-server/server.rb - This file contains the step-definition literals that you want to support.
Change the entries in the `steps` hash. Here, specify the steps' text as well as the ID which will be sent to the microcontroller when that step is invoked by Cucumber.

So change this:

```
        steps = {
            "I have a {string} led": 0,
            "I turn it on": 1,
            "the {string} led is {string}": 2
        }
```
to suit your needs.

This will send "EXEC 0" or "EXEC 1" or "EXEC 2" to the device depending on the step. Decide on your protocol and change accordingly.

The "{string}" parts of the text describe parameters, which will match text in "s in the feature file step.  Those parameters will be sent on the same line as the "EXEC..." text, after the ID.  They'll be surrounded by "s and in [], separated by ,s.  So for the three steps defined in the example above, and in the provided example feature file, the following will be sent over serial:

```
EXEC 0 ["red"]
EXEC 1 []
EXEC 2 ["red", "lit up"]
```

## On the device side:

In nrf51-rpc-server/api.c, change the execution to call the needed APIs under tests:

Change:

```
char *api_handler(char *command)
{
    if(strstr(command, "0")) {
        nrf_gpio_pin_clear(RED);
        nrf_gpio_pin_set(GREEN);
        return("0\n");
    }

    if(strstr(command, "1")) {      // Turn on red
        nrf_gpio_pin_clear(GREEN);
        nrf_gpio_pin_set(RED);
        return("0\n");
    }

    if(strstr(command, "2")) {  // Check that red is on
        return(nrf_gpio_pin_read(RED) ? "0\n" : "1\n");
    }
    return("1\n");
}
```

to suit your needs.

# Building the components

## Building the RPC Serever

See the readme in the relevant RPC server folder for instructions.  To make sure the codes in the `steps.json` file are matched correctly there is a tool to generate a C/C++ header file with a `#define` for each step.

To generate that (which you'll need to do whenever you modify `steps.json`):
```
cd wire-server
bundle exec ruby generate_step_header.rb steps.h
```

Then copy the `steps.h` file to the soruce folder in your RPC server, and build it.

Don't forget to flash your device with your modified code.

# Running the wire-server

Plug the target device into your USB port and issue

`ls /dev` to see the port name assigned to it.

Use that port name as the first parameter, the second being the port number to listen on. This has to match the port number lisetd in the features/step\_definitions/cucumber.wire file.

```
cd wire-server
bundle exec ruby server.rb /dev/tty.usbmodem1411 3901
```

This will start the server and you will see logs as it receives commands from the Cucumber gem.

# Running the RPC Server

It will automatically run, as soon as the device is plugged into the USB port as it will execute main() on powerup.

# Example Cucumber script

```
Feature: Hello World

	Scenario: Blink red LED
		Given I have a red led
		When I turn it on
		Then it's lit up
```

# Running example

To run the tests, then run Cucumber:

```
bundle exec cucumber
```

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

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/ihassin/cucumber-wire-tcp2serial/trend.png)](https://bitdeli.com/free "Bitdeli Badge")
