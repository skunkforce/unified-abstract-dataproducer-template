/**
 * @file UaDI_template.h
 * @brief This header file defines the API for interacting with various types of data producers.
 * @author Stephan Bökelmann
 * @email sboekelmann@ep1.rub.de
 *
 * This file defines the API for interacting with various types of data
 * producers. It includes functions for initializing the library, enumerating
 * available data producers, claiming and releasing devices, managing data 
 * chunks, and waiting for data. Detailed error codes and data management 
 * policies are provided for robust integration.
 */

#ifndef UADI_TEMPLATE_H
#define UADI_TEMPLATE_H

#include <stddef.h>

/* _WIN32 Macro is defined by the compiler when compiling for Windows
 * Linux doesn't need any additional defines.
 */
#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief Handle for the connection-id to the library instance.
 * @see uadi_init(...)
 * @see uadi_deinit(...)
 *
 * This is a void pointer to an instance that implements the DLL interface.
 * The consumer is responsible for allocating and deallocating memory for this
 * handle. A valid handle can be obtained by calling uadi_init(...) with an a 
 * pointer to this memory location.
 */
typedef void* uadi_lib_handle;

/** 
 * @brief Handle for a device instance.
 * @see uadi_claim_device(...)
 * @see uadi_release_device(...)
 *
 * This pointer points to an instance that implements the interface of an 
 * abstract data producer. The library is responsible for managing the 
 * lifecycle of this handle. It gets created by claiming a device and gets 
 * destroyed after a device has been released by the consumer. If the device 
 * still holds data when it is being released, it will trigger the receive 
 * callback, until the data is consumed.
 */
typedef void* uadi_device_handle;


/** 
 * @brief Pointer to a chunk of memory.
 * @see uadi_init(...)
 * @see uadi_push_chunks(...)
 *
 * A chunk of memory in the terminology of UaDI is a already allocated piece of 
 * memory, that gets created and destroyed by the consumer. It is meant to be 
 * the container of any information that is larger than a status code. It gets 
 * passed to the library, by handing over a pointer to the already allocated 
 * piece as well as the number of chunks that are contiguously allocated after 
 * the pointer. The library is supposed to handle chunks in a manner, that not 
 * all chunks have to be allocated contiguously, but multiple chunks can only 
 * be given to the library at once, if they are allocated contiguously. 
 *
 * Each UaD-Library is allowed to define their chunk size. Thus a initialization protocol is needed. The consumer is responsible 
 * for allocating and deallocating these chunks. These chunks are passed to the 
 * library via the uadi_push_chunks(...) function.
 */
typedef unsigned char* uadi_chunk_ptr;
#define UADI_DEFAULT_CHUNK_SIZE 128 * 1024

/**
 * @brief Status code for uadi_receive_callback.
 * @see uadi_receive_struct
 * This status code is part of the uadi_receive_struct.
 * The consumer is responsible for checking the status code, befor handling the 
 * pointers to the received data.
 * Status Codes:
 * - UADI_SUCCESS: 1
 * - UADI_ERROR: -1
 * 
 */
typedef int uadi_status;

/** 
 * @brief Structure to receive data from the library.
 * @see uadi_receive_callback(...)
 *
 * This structure is used when receiving chunks of data from the library.
 * It contains pointers to information and data packets. The format of data 
 * packets is an array of floats. Information packets are JSON strings.
 */
struct uadi_receive_struct{
    uadi_chunk_ptr infopack_ptr;
    uadi_chunk_ptr datapack_ptr;
    uadi_status status;
};

// Error codes
#define UADI_SUCCESS 0
#define UADI_ERROR -1
#define UADI_BUFFER_TOO_SMALL -2
#define UADI_INVALID_HANDLE -3
#define UADI_NO_DATA -4
#define UADI_OUT_OF_CHUNKS -5
#define UADI_INTERNAL_ERROR -255

/**
 * @brief Callback function for receiving data from the library.
 * @see uadi_receive_struct(...)
 * @see uadi_register_receive_callback(...)
 * This function is defined by the consumer, and called from the library when 
 * data is available. The function needs to be implemented by the consumer in a 
 * way, that it can handle a pointer to the received data. The void pointer is 
 * used by the consumer to provide context for the function. It might be a
 * pointer to a queue for example.
 */
typedef void(*uadi_receive_callback)(uadi_receive_struct*, void*);

/**
* @brief Callback function for recycling unused chunks.
* @see uadi_release_device(...)
* This function is defined by the consumer, and called from the library in 
* order to recycle unused chunks back to the consumer. The function needs to
* know the context, therefor it'll also take a void* to the consumers context.
* Eventhough the context may be the same as the pointer for the 
* receive_callback, it can be used separately. 
*/
typedef void(*uadi_recycle_unused_chunk_callback)(chunk_ptr, size_t, void*);


/**
 * @brief Initialize the library and fills a preallocated empty handle with an actual library handle.
 * @param lib_handle Pointer to the preallocated library handle.
 * @return uadi_status Status code of the operation.
 * @see uadi_deinit(...)
 * The library handle is used internally by the library. To keep track of the 
 * connection. This way, the library can handle multiple connections from 
 * different consumers. 
 * The consumer needs to keep the library handle and use it with other calls, 
 * as long as there hasn't been a device claimed. As soon as a device has been 
 * claimed, the device handle implicitly also holds the library handle.
 * It needs to be made sure, that after a device has been released, the library 
 * handle is still valid, until the consumer calls uadi_deinit(...). This has 
 * to be done in order to keep RAII intact.
 */
DLL_EXPORT uadi_status uadi_init(uadi_lib_handle* lib_handle);

/**
 * @brief This function fills a preallocated chunk of memory with JSON-formatted meta-data from the library itself. 
 * @param lib_handle Pointer to the library handle.
 * @param meta_data Pointer to the preallocated memory for the meta-string.
 * @param meta_data_size Size of the preallocated memory for the meta-string.
 * @return uadi_status Status code of the operation.
 * Meta-data can include all kinds of data, such as device information, version 
 * information, etc.
 * It shall not exceed 128KB in size, even though it is not enforced by the 
 * library. One could potentially have a longer JSON string than this and the 
 * call would fail with UADI_BUFFER_TOO_SMALL. In that case, the consumer would 
 * have to call the function again with a larger chunk of memory.
 * A consumer is not required to call this function.
 */
DLL_EXPORT uadi_status uadi_get_meta_data(
    uadi_lib_handle lib_handle, 
    char* meta_data,
    size_t meta_data_size);

/**
 * @brief This function enumerates all available data producer devices.
 * @param lib_handle Pointer to the library handle.
 * @param device_list Pointer to preallocated charbuffer where device list shall be stored.
 * @param device_list_size Size of the preallocated charbuffer.
 * @return uadi_status Status code of the operation.
 * @see uadi_claim_device(...)
 * @see uadi_release_device(...)
 * The library is viewed as the producer, anyhow, the producer may include 
 * several devices. The consumer needs to be aware of these devices and claim 
 * one to receive its data. A device is claimed exclusively, meaning, that only 
 * one consumer at a time can claim it. The received device list is a 
 * JSON-formatted string, containing all available devices.
 */
DLL_EXPORT uadi_status uadi_enumerate(
    uadi_lib_handle handle, 
    char* device_list,
    size_t device_list_size);

/**
 * @brief This function claims a data producer device.
 * @param lib_handle Pointer to the library handle.
 * @param device_handle Pointer to the preallocated empty device handle.
 * @param device_key Pointer to a zero-terminated array of characters containing the device key.
 * @param receive_callback Pointer to the callback function.
 * @param receive_context Pointer to the consumers context.
 * @param recycle_callback Pointer to the recycle callback function.
 * @param recycle_context Pointer to the consumers context.
 * @param chunk_array Pointer to the preallocated chunk array.
 * @param chunk_count Number of chunks in the chunk array.
 * @return uadi_status Status code of the operation.
 * @see uadi_enumerate(...)
 * @see uadi_release_device(...)
 * @see push_chunks(...)
 * This function is the heart of the measurement process.
 * It is used by the consumer to properly claim and set up a device.
 * In order for the device to function, it needs memory to store received data 
 * from the device, as well as a routine from the consumer, that is called when 
 * new data is available.
 * The consumer needs to keep the device handle and use it with other calls. 
 * The device handle is implicitly also holds the library handle.
 * The device handle is an exclusive handle, meaning, that only one consumer at 
 * a time can claim it. Leaking the handle will result in a loss of the claimed 
 * device.
 * The callback function is called whenever a new chunk from the device is 
 * available. A device can't be released as long as there is available data 
 * from the device. The release function will stop the new acquisition of data, 
 * but will make sure, that the callback function is called with all available 
 * data.
 * A device may also give back unused chunks to the consumer, this is done
 * by using the recycle function.
 * The user data pointer is used by the consumer to provide context for the 
 * function. It might be a pointer to a queue for example.
 */
DLL_EXPORT uadi_status uadi_claim_device(
    uadi_lib_handle lib_handle, 
    uadi_device_handle* device_handle, 
    char const* device_key, 
    uadi_receive_callback receive_callback, 
    void* receive_context,
    uadi_recycle_unused_chunk_callback recycle_callback,
    void* recycle_context,
    uadi_chunk_ptr* chunk_array, 
    size_t chunk_count);

/**
 * @brief This function is used to push chunks of memory to a device.
 * @param device_handle Pointer to the device handle.
 * @param chunk_array Pointer to the preallocated chunk array.
 * @param chunk_count Number of chunks in the chunk array.
 * @return uadi_status Status code of the operation.
 * The push chunks function will hand over chunks of memory to a device inside 
 * the library. The chunks might be empty, but might also be filled with 
 * control data for the device to handle.
 */
DLL_EXPORT uadi_status uadi_push_chunks(uadi_device_handle device_handle, uadi_chunk_ptr* chunk_array, size_t chunk_count);

/**
 * @brief This function releases a device.
 * @param device_handle Pointer to the device handle.
 * @return uadi_status Status code of the operation.
 * @see uadi_register_receive_callback(...)
 * @see uadi_claim_device(...)
 * After a consumer is done with the device, it has to release it. The release 
 * function will stop the acquisition of new data from the device and will make 
 * sure, that the callback function is called with all remaining chunks in the 
 * devices queue. Empty chunks will be propagated back to the consumer as info-
 * packs, containing nothing but a terminating zero.
 */
DLL_EXPORT uadi_status uadi_release_device(uadi_device_handle device_handle);

/**
 * @brief This function deinitializes the library.
 * @param lib_handle Pointer to the library handle.
 * @return uadi_status Status code of the operation.
 * @see uadi_init(...)
 * After the library is deinitialized, it is no longer usable.
 */
DLL_EXPORT uadi_status uadi_deinit(uadi_lib_handle lib_handle);

#ifdef __cplusplus
}
#endif

#endif // UADI_TEMPLATE_H