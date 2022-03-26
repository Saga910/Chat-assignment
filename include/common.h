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
    uint16_t channel_type;
    uint16_t channel_id;
    uint16_t msg_len;
    char *msg;
};

struct CptResponse{
    uint8_t code;
    uint8_t *data;
};

struct CptMsgResponse{
    uint16_t channel_id;
    uint16_t user_id;
    uint16_t msg_len;
    uint8_t *msg;
};

#endif // TEMPLATE_COMMON_H
