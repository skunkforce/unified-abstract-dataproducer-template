# Unified Abstract Data Producer Interface (UaDI)

## Overview
The "Unified Abstract Data Producer Interface" (UaDI) is a template that serves as a foundation for developing your own UaDI devices for the OmniView project. UaDI aims to abstract any measurement device that can be connected to a computer. An object of this class is implemented with a low-fidelity channel towards the measurement device, which operates by sending a JSON string to the library. Additionally, it has two channels from the device to the consumer: a high-speed channel that carries actual measurement data, and a low-fidelity channel for occasional JSON strings from the device to the consumer.

## Example: Iota Producer
This particular example of such a producer is an iota producer, providing a continuous stream of numbers from 0 to 255, then starting again. It is intended for beginners to understand the API. The OmniView project provides basic usage of such a producer.

## Getting Started
### Initialization
- The consumer must first initialize a connection to the producer by calling the `uadi_init()` function with a pointer to a library handle (e.g., `uadi_lib_handle lib_handle; uadi_init(&lib_handle);`).

### Metadata and Device List
- After initialization, the consumer can call `uadi_get_meta_data()` to retrieve data about the producer, or `uadi_enumerate()` to get a list of potential devices to claim. 
- Devices are identified in the `enumerate_list`, which the consumer receives as a JSON string (e.g., `uadi_chunk_ptr enumerate_list; uadi_enumerate(lib_handle, &enumerate_list);`).

### Claiming Devices
- Calling `uadi_claim_device()` with the device key as a parameter attempts to exclusively claim the device (e.g., `uadi_device_handle device_handle; uadi_claim_device(lib_handle, &device_handle, "device_key", callback_function, user_data, chunk_array, chunk_count);`).
- In our example, a thread is spawned that will start generating either an iota if `123e4567-e89b-12d3-a456-426655440000` is claimed, or a reverse iota if `e89b4567-123e-12d3-a456-426655440000` is claimed. The data will be written into the chunks, and as soon as a chunk is full, the callback is called, handing the chunk back over to the consumer.
