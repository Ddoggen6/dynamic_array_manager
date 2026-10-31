#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "array_manager.h"
#include "mongoose.h"



static int appendJson(char **json,
                      size_t *capacity,
                      size_t *length,
                      const char *format, ...) {

    if (json == NULL || capacity == NULL ||
        length == NULL || format == NULL ||
        *length > *capacity ||
        (*json == NULL && (*capacity != 0 || *length != 0))) {
        return -1;
    }

    va_list args;
    va_start(args, format);

    va_list argsCopy;
    va_copy(argsCopy, args);

    int needed = vsnprintf(NULL, 0, format, argsCopy);

    va_end(argsCopy);

    if (needed < 0) {
        va_end(args);
        return -1;
    }

    if (*length > SIZE_MAX - (size_t)needed - 1) {
        va_end(args);
        return -1;
    }

    size_t required = *length + (size_t)needed + 1;

    if (required > *capacity) {
        size_t newCapacity = (*capacity == 0) ? 128 : *capacity;

        while (newCapacity < required) {
            if (newCapacity > SIZE_MAX / 2) {
                newCapacity = required;
                break;
            }

            newCapacity *= 2;
        }

        char *temp = realloc(*json, newCapacity);

        if (temp == NULL) {
            va_end(args);
            return -1;
        }

        *json = temp;
        *capacity = newCapacity;
    }

    int written = vsnprintf(
        *json + *length,
        *capacity - *length,
        format,
        args
    );

    va_end(args);

    if (written != needed) {
        return -1;
    }

    *length += (size_t)written;

    return 0;
}

static void getGroups(struct mg_connection *connection,
                      ArrayManager *manager) {

    if (connection == NULL || manager == NULL) {
        return;
    }

    char *json = NULL;
    size_t capacity = 0;
    size_t length = 0;

    if (appendJson(&json, &capacity, &length, "{\"groups\":[")) {
        free(json);
        return;
    }

    for (size_t i = 0; i < MAX_GROUPS; i++) {

        size_t occupied = 0;

        // First loop: count occupied slots.
        for (size_t j = 0; j < MAX_ARRAY_PER_GROUP; j++) {

            if (manager->groups[i].arrays[j].data != NULL) {
                occupied++;
            }
        }

        size_t available = MAX_ARRAY_PER_GROUP - occupied;

        if (appendJson(&json, &capacity, &length,
                       "%s{\"group\":%zu,\"occupied\":%zu,\"available\":%zu,\"arrays\":[",
                       i == 0 ? "" : ",",
                       i + 1,
                       occupied,
                       available)) {
            free(json);
            return;
        }

        size_t arrayCount = 0;

        // Second loop: populate the arrays field.
        for (size_t j = 0; j < MAX_ARRAY_PER_GROUP; j++) {

            ManagedArray *array = &manager->groups[i].arrays[j];

            if (array->data == NULL) {
                continue;
            }

            if (appendJson(&json, &capacity, &length,
                           "%s{\"slot\":%zu,\"size\":%zu}",
                           arrayCount == 0 ? "" : ",",
                           j + 1,
                           array->size)) {
                free(json);
                return;
            }

            arrayCount++;
        }

        // Close the arrays field and the group object.
        if (appendJson(&json, &capacity, &length, "]}")) {
            free(json);
            return;
        }
    }

    // Close the groups array and the root JSON object.
    if (appendJson(&json, &capacity, &length, "]}")) {
        free(json);
        return;
    }

    mg_http_reply(connection, 200,
                  "Content-Type: application/json\r\n",
                  "%s\n", json);

    free(json);
}






static void eventHandler(struct mg_connection *connection,
                         int event,
                         void *eventData) {

    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *message = eventData;

        ArrayManager *manager = connection->fn_data;

        if (mg_match(message->uri, mg_str("/api/hello"), NULL)) {

            mg_http_reply(connection, 200,
                          "Content-Type: application/json\r\n",
                          "{\"status\":\"ok\"}\n");

        } else if (mg_match(message->uri, mg_str("/api/groups"), NULL)) {

            getGroups(connection, manager);

        } else {

            struct mg_http_serve_opts options = {
                .root_dir = "./web"
            };

            mg_http_serve_dir(connection, message, &options);
        }
    }
}
int main(void) {
    struct mg_mgr manager;

    ArrayManager appManager;
    initializeManager(&appManager);





    mg_mgr_init(&manager);

    if (mg_http_listen(&manager, "http://127.0.0.1:8080",
                       eventHandler, &appManager) == NULL) {
        fprintf(stderr, "Could not start HTTP server.\n");
        mg_mgr_free(&manager);
        return 1;
    }

    printf("Server running at http://127.0.0.1:8080\n");

    for (;;) {
        mg_mgr_poll(&manager, 1000);
    }

    mg_mgr_free(&manager);
    return 0;
}
