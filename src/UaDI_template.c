/**
 * @file UaDI_template.c
 * @brief This file implements an iota-producer
 * @author Stephan Bökelmann
 * @email sboekelmann@ep1.rub.de
 *
 * In order for the OmniView project and its interface to a UaDI compatible data producer device to be understandable, this DLL shall provide an example on how the interface is supposed to be used. 
 * This particular DLL will generate an integer every ms adding one to the previous value. This way a sawtooth wave is generated.
 * A consumer initializing a new connection shall result in a new thread started. 
 */

#include "UaDI_template.h"


struct connection{
    thread* thread;
    //list of devices
};

struct device{
    //id
    //state of the device
};

uadi_status uadi_init(uadi_lib_handle* lib_handle)
{
    //spawns a new thread for the connection
    return 0;
};

uadi_status uadi_get_meta_data(
    uadi_lib_handle lib_handle, 
    uadi_chunk_ptr meta_data)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "iota-producer");
    cJSON_AddNumberToObject(root, "version", "0.0.1");
    cJSON_AddStringToObject(root, "author", "...");
    cJSON_AddStringToObject(root, "description", "...");

    char* jsonStr = cJSON_Print(root);
    memcpy(meta_data, jsonStr, strlen(jsonStr) + 1);
    cJSON_Delete(root);
    free(jsonStr);
    return 0;
};

uadi_status uadi_enumerate(uadi_lib_handle handle, uadi_chunk_ptr device_list)
{
    const char* jsonStr = 
        "{\"devices\":["
        "{\"key\":\"123e4567-e89b-12d3-a456-426655440000\",\"vendor\":\"skunkforce e.V.\",\"description\":\"generates an iota\"},"
        "{\"key\":\"e89b4567-123e-12d3-a456-426655440000\",\"vendor\":\"skunkforce e.V.\",\"description\":\"generates an inverse iota\"}"
        "]}";

    memcpy(device_list, jsonStr, strlen(jsonStr) + 1);
    return 0;
};

DLL_EXPORT uadi_status uadi_claim_device(
    uadi_lib_handle lib_handle, 
    uadi_device_handle* device_handle, 
    char const* device_key, 
    uadi_receive_callback callback, 
    void* user_data)
{

    return 0;
};

uadi_status uadi_push_chunks(
    uadi_device_handle device_handle, 
    uadi_chunk_ptr* chunk_array, 
    size_t chunk_count)
{
    return 0;
};

uadi_status uadi_release_device(uadi_device_handle device_handle)
{
    return 0;
};

uadi_status uadi_deinit(uadi_lib_handle lib_handle)
{
    return 0;
};