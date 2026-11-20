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

#include <limits.h>

static int parseNumber(struct mg_str token, long long *value)
{
    if (token.buf == NULL || token.len == 0 || value == NULL) {
        return -1;
    }

    size_t i = 0;
    int negative = 0;

    if (token.buf[0] == '-') {
        negative = 1;
        i++;
    }

    if (i == token.len) {
        return -1;
    }

    unsigned long long magnitude = 0;

    for (; i < token.len; i++) {
        char c = token.buf[i];

        if (c < '0' || c > '9') {
            return -1;
        }

        unsigned long long digit = (unsigned long long)(c - '0');

        if (magnitude > (ULLONG_MAX - digit) / 10ULL) {
            return -1;
        }

        magnitude = magnitude * 10ULL + digit;
    }

    if ((!negative && magnitude > (unsigned long long)LLONG_MAX) ||
        (negative && magnitude > (unsigned long long)LLONG_MAX + 1ULL)) {
        return -1;
    }

    if (negative) {
        *value = magnitude == (unsigned long long)LLONG_MAX + 1ULL
            ? LLONG_MIN
            : -(long long)magnitude;
    } else {
        *value = (long long)magnitude;
    }

    return 0;
}

static int parseJsonNumber(
    struct mg_str body,
    const char *path,
    long long *value
) {
    int tokenLength = 0;

    int tokenPosition = mg_json_get(
        body,
        path,
        &tokenLength
    );

    if (tokenPosition < 0 ||
        tokenLength <= 0 ||
        (size_t)tokenPosition > body.len ||
        (size_t)tokenLength > body.len - (size_t)tokenPosition) {
        return -1;
    }

    return parseNumber(
        mg_str_n(
            body.buf + tokenPosition,
            (size_t)tokenLength
        ),
        value
    );
}

static int parsePathIndex(
    struct mg_str token,
    size_t maximum,
    size_t *index
) {
    long long number;

    if (parseNumber(token, &number) != 0 ||
        number < 1 ||
        (unsigned long long)number > maximum ||
        index == NULL) {
        return -1;
    }

    *index = (size_t)number - 1;
    return 0;
}

static void replyJsonError(
    struct mg_connection *connection,
    int status,
    const char *message
) {
    mg_http_reply(
        connection,
        status,
        "Content-Type: application/json\r\n",
        "{\"error\":\"%s\"}\n",
        message
    );
}

static ManagedArray *findArray(
    struct mg_connection *connection,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken,
    size_t *groupIndex,
    size_t *slotIndex
) {
    if (parsePathIndex(
            groupToken,
            MAX_GROUPS,
            groupIndex
        ) != 0 ||
        parsePathIndex(
            slotToken,
            MAX_ARRAY_PER_GROUP,
            slotIndex
        ) != 0) {
        replyJsonError(connection, 400, "Invalid group or slot");
        return NULL;
    }

    ManagedArray *array = getArray(
        manager,
        *groupIndex,
        *slotIndex
    );

    if (array == NULL) {
        replyJsonError(connection, 404, "Array not found");
    }

    return array;
}

static void getArrayHandler(
    struct mg_connection *connection,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken
) {
    size_t groupIndex;
    size_t slotIndex;

    ManagedArray *array = findArray(
        connection,
        manager,
        groupToken,
        slotToken,
        &groupIndex,
        &slotIndex
    );

    if (array == NULL) {
        return;
    }

    char *json = NULL;
    size_t capacity = 0;
    size_t length = 0;

    if (appendJson(
            &json,
            &capacity,
            &length,
            "{\"group\":%zu,\"slot\":%zu,\"size\":%zu,\"data\":[",
            groupIndex + 1,
            slotIndex + 1,
            array->size
        ) != 0) {
        free(json);
        replyJsonError(connection, 500, "JSON generation failed");
        return;
    }

    for (size_t i = 0; i < array->size; i++) {
        if (appendJson(
                &json,
                &capacity,
                &length,
                "%s%d",
                i == 0 ? "" : ",",
                array->data[i]
            ) != 0) {
            free(json);
            replyJsonError(connection, 500, "JSON generation failed");
            return;
        }
    }

    if (appendJson(&json, &capacity, &length, "]}") != 0) {
        free(json);
        replyJsonError(connection, 500, "JSON generation failed");
        return;
    }

    mg_http_reply(
        connection,
        200,
        "Content-Type: application/json\r\n",
        "%s\n",
        json
    );

    free(json);
}

static void resizeArrayHandler(
    struct mg_connection *connection,
    struct mg_http_message *message,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken
) {
    if (!mg_match(message->method, mg_str("PATCH"), NULL)) {
        replyJsonError(connection, 405, "Method not allowed");
        return;
    }

    size_t groupIndex;
    size_t slotIndex;

    ManagedArray *array = findArray(
        connection,
        manager,
        groupToken,
        slotToken,
        &groupIndex,
        &slotIndex
    );

    if (array == NULL) {
        return;
    }

    long long requestedSize;

    if (parseJsonNumber(
            message->body,
            "$.size",
            &requestedSize
        ) != 0 ||
        requestedSize < 1 ||
        requestedSize > MAX_SIZE) {
        replyJsonError(connection, 400, "Invalid array size");
        return;
    }

    int result = resizeArray(
        array,
        (size_t)requestedSize
    );

    if (result == 1) {
        mg_http_reply(
            connection,
            200,
            "Content-Type: application/json\r\n",
            "{\"status\":\"success\",\"group\":%zu,\"slot\":%zu,\"size\":%zu}\n",
            groupIndex + 1,
            slotIndex + 1,
            array->size
        );
    } else if (result == ERR_ALLOCATION_FAILED) {
        replyJsonError(connection, 500, "Memory allocation failed");
    } else {
        replyJsonError(connection, 400, "Array resize failed");
    }
}

static void addElementHandler(
    struct mg_connection *connection,
    struct mg_http_message *message,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken
) {
    if (!mg_match(message->method, mg_str("POST"), NULL)) {
        replyJsonError(connection, 405, "Method not allowed");
        return;
    }

    size_t groupIndex;
    size_t slotIndex;

    ManagedArray *array = findArray(
        connection,
        manager,
        groupToken,
        slotToken,
        &groupIndex,
        &slotIndex
    );

    if (array == NULL) {
        return;
    }

    long long position;
    long long value;

    if (parseJsonNumber(
            message->body,
            "$.position",
            &position
        ) != 0 ||
        parseJsonNumber(
            message->body,
            "$.value",
            &value
        ) != 0 ||
        position < 1 ||
        position > MAX_SIZE + 1 ||
        value < INT_MIN ||
        value > INT_MAX) {
        replyJsonError(connection, 400, "Invalid element input");
        return;
    }

    int result = addElement(
        array,
        (size_t)(position - 1),
        (int)value
    );

    if (result == ADD_SUCCESS) {
        mg_http_reply(
            connection,
            200,
            "Content-Type: application/json\r\n",
            "{\"status\":\"success\",\"group\":%zu,\"slot\":%zu,\"size\":%zu}\n",
            groupIndex + 1,
            slotIndex + 1,
            array->size
        );
    } else if (result == ADD_ERR_MAX_SIZE) {
        replyJsonError(connection, 409, "Array is already at maximum size");
    } else if (result == ERR_ALLOCATION_FAILED) {
        replyJsonError(connection, 500, "Memory allocation failed");
    } else {
        replyJsonError(connection, 400, "Invalid insertion position");
    }
}

static void removeElementHandler(
    struct mg_connection *connection,
    struct mg_http_message *message,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken,
    struct mg_str positionToken
) {
    if (!mg_match(message->method, mg_str("DELETE"), NULL)) {
        replyJsonError(connection, 405, "Method not allowed");
        return;
    }

    size_t groupIndex;
    size_t slotIndex;

    ManagedArray *array = findArray(
        connection,
        manager,
        groupToken,
        slotToken,
        &groupIndex,
        &slotIndex
    );

    if (array == NULL) {
        return;
    }

    size_t position;

    if (parsePathIndex(
            positionToken,
            MAX_SIZE,
            &position
        ) != 0) {
        replyJsonError(connection, 400, "Invalid removal position");
        return;
    }

    int result = removeElement(array, position);

    if (result == REMOVE_SUCCESS) {
        mg_http_reply(
            connection,
            200,
            "Content-Type: application/json\r\n",
            "{\"status\":\"success\",\"group\":%zu,\"slot\":%zu,\"size\":%zu}\n",
            groupIndex + 1,
            slotIndex + 1,
            array->size
        );
    } else {
        replyJsonError(connection, 400, "Invalid removal position");
    }
}

static void deleteArrayHandler(
    struct mg_connection *connection,
    struct mg_http_message *message,
    ArrayManager *manager,
    struct mg_str groupToken,
    struct mg_str slotToken
) {
    if (!mg_match(message->method, mg_str("DELETE"), NULL)) {
        replyJsonError(connection, 405, "Method not allowed");
        return;
    }

    size_t groupIndex;
    size_t slotIndex;

    if (parsePathIndex(
            groupToken,
            MAX_GROUPS,
            &groupIndex
        ) != 0 ||
        parsePathIndex(
            slotToken,
            MAX_ARRAY_PER_GROUP,
            &slotIndex
        ) != 0) {
        replyJsonError(connection, 400, "Invalid group or slot");
        return;
    }

    int result = deleteArray(
        manager,
        groupIndex,
        slotIndex
    );

    if (result == DELETE_SUCCESS) {
        mg_http_reply(
            connection,
            200,
            "Content-Type: application/json\r\n",
            "{\"status\":\"success\",\"group\":%zu,\"slot\":%zu}\n",
            groupIndex + 1,
            slotIndex + 1
        );
    } else {
        replyJsonError(connection, 404, "Array not found");
    }
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
        struct mg_str caps[4];

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
        } else if (mg_match(
                       message->uri,
                       mg_str("/api/arrays/*/*/elements/*"),
                       caps
                   )) {

            removeElementHandler(
                connection,
                message,
                manager,
                caps[0],
                caps[1],
                caps[2]
            );

        } else if (mg_match(
                       message->uri,
                       mg_str("/api/arrays/*/*/elements"),
                       caps
                   )) {

            addElementHandler(
                connection,
                message,
                manager,
                caps[0],
                caps[1]
            );

        } else if (mg_match(
                       message->uri,
                       mg_str("/api/arrays/*/*"),
                       caps
                   )) {

            if (mg_match(message->method, mg_str("GET"), NULL)) {

                getArrayHandler(
                    connection,
                    manager,
                    caps[0],
                    caps[1]
                );

            } else if (mg_match(message->method, mg_str("PATCH"), NULL)) {

                resizeArrayHandler(
                    connection,
                    message,
                    manager,
                    caps[0],
                    caps[1]
                );

            } else if (mg_match(message->method, mg_str("DELETE"), NULL)) {

                deleteArrayHandler(
                    connection,
                    message,
                    manager,
                    caps[0],
                    caps[1]
                );

            } else {
                replyJsonError(
                    connection,
                    405,
                    "Method not allowed"
                );
            }

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

    if (mg_http_listen(&manager, "http://0.0.0.0:8080",
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
