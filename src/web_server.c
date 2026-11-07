
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "array_manager.h"
#include "mongoose.h"


/* Build JSON strings dynamically. */

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


/* Return information about all groups. */

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
        mg_http_reply(connection, 500, "", "JSON generation failed\n");
        return;
    }

    for (size_t i = 0; i < MAX_GROUPS; i++) {

        size_t occupied = 0;

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
            mg_http_reply(connection, 500, "", "JSON generation failed\n");
            return;
        }

        size_t arrayCount = 0;

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
                mg_http_reply(connection, 500, "", "JSON generation failed\n");
                return;
            }

            arrayCount++;
        }

        if (appendJson(&json, &capacity, &length, "]}")) {

            free(json);
            mg_http_reply(connection, 500, "", "JSON generation failed\n");
            return;
        }
    }

    if (appendJson(&json, &capacity, &length, "]}")) {

        free(json);
        mg_http_reply(connection, 500, "", "JSON generation failed\n");
        return;
    }

    mg_http_reply(connection, 200,
                  "Content-Type: application/json\r\n",
                  "%s\n", json);

    free(json);
}


/* Extract and validate the requested array size. */

static int parseArraySize(struct mg_str body, size_t *size) {

    if (size == NULL) {
        return -1;
    }

    int tokenLength = 0;

    int tokenPosition = mg_json_get(
        body,
        "$.size",
        &tokenLength
    );

    if (tokenPosition < 0 || tokenLength <= 0) {
        return -1;
    }

    if ((size_t)tokenPosition > body.len ||
        (size_t)tokenLength > body.len - (size_t)tokenPosition) {
        return -1;
    }

    size_t value = 0;

    for (int i = 0; i < tokenLength; i++) {

        char character = body.buf[tokenPosition + i];

        if (character < '0' || character > '9') {
            return -1;
        }

        size_t digit = (size_t)(character - '0');

        if (value > (MAX_SIZE - digit) / 10) {
            return -1;
        }

        value = value * 10 + digit;
    }

    if (value == 0 || value > MAX_SIZE) {
        return -1;
    }

    *size = value;

    return 0;
}


/* Handle array creation requests. */

static void createArrayHandler(
    struct mg_connection *connection,
    struct mg_http_message *message,
    ArrayManager *manager
) {

    if (connection == NULL || message == NULL || manager == NULL) {
        return;
    }

    /* Verify HTTP method. */

    if (!mg_match(message->method, mg_str("POST"), NULL)) {

        mg_http_reply(connection, 405,
                      "Content-Type: application/json\r\n",
                      "{\"error\":\"Method not allowed\"}\n");

        return;
    }


    /* Extract and validate array size. */

    size_t newSize;

    if (parseArraySize(message->body, &newSize) != 0) {

        mg_http_reply(connection, 400,
                      "Content-Type: application/json\r\n",
                      "{\"error\":\"Invalid array size\"}\n");

        return;
    }


    /* Call the existing C array manager. */

    int selectedGroup;
    int selectedSlot;

    int result = createArray(
        manager,
        newSize,
        &selectedGroup,
        &selectedSlot
    );


    /* Handle creation errors. */

    if (result != CREATE_SUCCESS) {

        if (result == ERR_NO_FREE_SLOTS) {

            mg_http_reply(connection, 409,
                          "Content-Type: application/json\r\n",
                          "{\"error\":\"No available slots\"}\n");

        } else if (result == ERR_ALLOCATION_FAILED) {

            mg_http_reply(connection, 500,
                          "Content-Type: application/json\r\n",
                          "{\"error\":\"Memory allocation failed\"}\n");

        } else {

            mg_http_reply(connection, 400,
                          "Content-Type: application/json\r\n",
                          "{\"error\":\"Array creation failed\"}\n");
        }

        return;
    }


    /* Return successful creation details. */

    mg_http_reply(connection, 201,
                  "Content-Type: application/json\r\n",
                  "{"
                  "\"status\":\"success\","
                  "\"message\":\"Array created successfully\","
                  "\"group\":%d,"
                  "\"slot\":%d,"
                  "\"size\":%zu"
                  "}\n",
                  selectedGroup + 1,
                  selectedSlot + 1,
                  newSize);
}


/* HTTP request router. */

static void eventHandler(struct mg_connection *connection,
                         int event,
                         void *eventData) {

    if (event == MG_EV_HTTP_MSG) {

        struct mg_http_message *message = eventData;

        ArrayManager *manager = connection->fn_data;

        if (mg_match(message->uri, mg_str("/api/hello"), NULL)) {

            if (!mg_match(message->method, mg_str("GET"), NULL)) {

                mg_http_reply(connection, 405, "",
                              "Method not allowed\n");
                return;
            }

            mg_http_reply(connection, 200,
                          "Content-Type: application/json\r\n",
                          "{\"status\":\"ok\"}\n");

        } else if (mg_match(message->uri, mg_str("/api/groups"), NULL)) {

            if (!mg_match(message->method, mg_str("GET"), NULL)) {

                mg_http_reply(connection, 405, "",
                              "Method not allowed\n");
                return;
            }

            getGroups(connection, manager);

        } else if (mg_match(message->uri, mg_str("/api/arrays"), NULL)) {

            createArrayHandler(connection, message, manager);

        } else {

            struct mg_http_serve_opts options = {
                .root_dir = "./web"
            };

            mg_http_serve_dir(connection, message, &options);
        }
    }
}


/* Initialize and run the HTTP server. */

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
