#ifndef MORSE_COMMON_H
#define MORSE_COMMON_H

/*
 * This file is part of dc_dump.
 *
 *  dc_dump is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with dc_dump.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <bits/stdint-uintn.h>
#include <string.h>

#define VERSION 1.1
#define SUCCESS 1
#define MESSAGE 2
#define USER_CONNECTED 3
#define USER_DISCONNECTED 4
#define MESSAGE_FAILED 5
#define CHANNEL_CREATED 6
#define CHANNEL_CREATION_ERROR 7
#define CHANNEL_DESTROYED 8
#define USER_JOINED_CHANNEL 9
#define USER_LEFT_CHANNEL 10
#define USER_LIST 11
#define UNKNOWN_CMD 12
#define LOGIN_FAIL 13
#define UNKNOWN_CHANNEL 14
#define BAD_VERSION 15
#define SEND_FAILED 16
#define CHAN_ID_OVERFLOW 17
#define MSG_OVERFLOW 18
#define MSG_LEN_OVERFLOW 19
#define CHAN_EMPTY 20
#define INVALID_ID 21
#define UNAUTH_ACCESS 22
#define SERVER_FULL 23
#define RESERVED 255

enum commands{
    SEND = 1,
    LOGOUT = 2,
    GET_USERS = 3,
    CREATE_CHANNEL = 4,
    JOIN_CHANNEL = 5,
    LEAVE_CHANNEL = 6,
    LOGIN = 7
};

struct CptRequest{
    uint8_t version;
    uint8_t command;
    uint16_t channel_id;
    uint16_t msg_len;
    char *msg;
};

struct CptResponse{
    uint8_t code;
    uint8_t *data;
    uint16_t data_size;
};

struct CptMsgResponse{
    uint16_t channel_id;
    uint16_t user_id;
    uint16_t msg_len;
    uint8_t *msg;
};

/**
* Serialize a CptRequest struct for transmission.
*
* @param cpt    A CptRequest struct.
* @return       Size of the serialized packet.
*/
size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer);

/**
* Serialize a CptResponse object for transmission.
*
* @param cpt    A CptResponse object.
* @return       Size of the serialized packet.
*/
size_t cpt_serialize_response(struct CptResponse * res, uint8_t * buffer);

/**
 * Initialize CptRequest object.
 *
 * Dynamically allocates a cpt struct and
 * initializes all fields.
 *
 * @return Pointer to cpt struct.
*/
struct CptRequest * cpt_request_init(void);

/**
 * Free all memory and set fields to null.
 *
 * @param cpt   Pointer to a cpt structure.
*/
void cpt_request_destroy(struct CptRequest * cpt);

/**
 * Reset packet parameters.
 *
 * Reset the packet parameters,
 * and free memory for certain params.
 *
 * @param packet    A CptRequest struct.
*/
void cpt_request_reset(struct CptRequest * packet);

/**
 * Initialize CptResponse server-side packet.
 *
 * Initializes a CptResponse, returning a dynamically
 * allocated pointer to a CptResponse struct.
 *
 * @param res_code    Received client-side packet.
 * @return Pointer to a CptResponse object.
 */
struct CptResponse * cpt_response_init(void);

/**
 * Destroy CptResponse object.
 *
 * Destroys CptResponse object, freeing any allocated memory
 * and setting all pointers to null.
 *
 * @param response  Pointer to a CptResponse object.
 */
void cpt_response_destroy(struct CptResponse * response);

/**
 * Reset packet parameters.
 *
 * Reset the response parameters, and free memory for certain params.
 *
 * @param response		Pointer to a CptResponse object.
*/
void cpt_response_reset(struct CptResponse * response);

/**
 * @brief Parse serialized server response.
 *
 * @param response  Address to a CptResponse object.
 * @param buffer    Serialized response from server.
 * @return Pointer to filled CptResponse.
 */
struct CptResponse * cpt_parse_response(uint8_t * res_buf, size_t data_size);

/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return A pointer to a cpt struct.
*/
struct CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size);

#endif // TEMPLATE_COMMON_H
