# Unified abstract Data Producer Interface (UaDI)
This repo implements a template on which you can build your own UaDI device for the OmniView project.
Any measurement-device that can be connected to a computer should potentially be abstractable to a UaD.
An object of this class has a low fidelity-channel in the direction of the measurement-device, that is implemented by using a JSON-string, that a consumer can send to the library. 
It also has two channels from device to the consumer: one high-speed channel that transports actual measurement data, and one low-fidelity channel, that transports occasional JSON strings from the device to the consumer.

This particular example of such a producer is an iota-producer, providing a continuous stream of numbers from 0 to 255 and starting again.
it shall be used for every beginner to understand the api. The OmniView project provides a basic usage of such a producer. 
The library itself can be viewd as the producer. the consumer has to initialize a connection to the producer by calling the uadi_init() function.
after that the consumer can either call uadi_get_meta_data() to get data about the producer, or call uadi_enumerate(). this provides the consumer with a list of potential devices to claim for the consumer. the devices are identified by a string in the enumerate_list, which is a json string that the consumer receives. calling uadi_claim_device() with the device-key as a parameter, trys to claim the device exclusively. in our example, a thread is spawned, that will start generating either an iota if `123e4567-e89b-12d3-a456-426655440000` is claimed or an reverse iota if `e89b4567-123e-12d3-a456-426655440000` is claimed.
