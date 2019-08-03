# Sensor Blast Library
The SensorBlast library is a collection of software drivers aimed at providing an easy interface
to a sensor attached in an embedded system. The aim here is to be a collection of non-vendor
cosntrained APIs with minimal overhead "fluff" and no marketing overhead or gimmicks- just plain
old code. 

All of the drivers and code examples are written in user space for Linux, however they can easily
ported to support a kernel space driver or even a microcontroller platform. All hardware specific
interaction is organized in the **src/sensors/interface** folder.  Any device specific interaction
such as GPIO or I2C should be ported here.

# Supported Sensors
Currently the SensorBlast library supports only the [HDC2010](http://www.ti.com/product/HDC2010).
 temperature and humidity sensor. More sensing devices are planned. 

# HDC2010
The HDC2010 driver and subsequent code examples are all intended to be used with an I2C interface.
Select code examples will utilize a GPIO for an interrupt line. Th

## Code Examples

### [HDC2010 Polling](https://github.com/tilogan/sensorblast/blob/master/examples/hdc2010/hdc2010_poll/hdc2010_poll.c)
This code example demonstrates the interrupt functionality of the HDC2010. Instead of discretely pausing for a
set duration of time this code example will wait for an interrupt betweeen initiating a data measurement and
reading the result. 

### [HDC2010 Periodic](https://github.com/tilogan/sensorblast/blob/master/examples/hdc2010/hdc2010_periodic/hdc2010_periodic.c)
This is the most simple code example that will initiate a measurement,
[pause for the amount of time specified in the datasheet](www.ti.com/lit/gpn/hdc2010), and then read/print
the result over standard out. 

### [HDC2010 AWS Publish](https://github.com/tilogan/sensorblast/tree/master/examples/hdc2010/hdc2010_aws_publish)
The AWS Publish code examples leverages the AWS Embedded C SDK and uses an MQTT publisher to publish
data to a defined MQTT subscriber on AWS. This requires that you download the [AWS Embedded C SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C)
to your device. Furthermore you must specify the directory in which your keys/certificates reside. See the
[Getting Started with AWS](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html) tutorial
on guidance on how to get started.
