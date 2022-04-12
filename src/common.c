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

#include <stdlib.h>
#include <stdio.h>
#include "common.h"

struct CptResponse * cpt_parse_response(uint8_t * res_buf, size_t data_size){
    struct CptResponse *res;
    res = malloc(data_size);

    int * current = (int *) 1;

    res->code = res_buf[0];
    res->data_size = unpack_u16(res_buf, current);
    res->channel_id = unpack_u16(res_buf, current);
    res->user_id = unpack_u16(res_buf, current);
    res->msg_len = unpack_u16(res_buf, current);
    res->msg = &res_buf[8];

    return res;
}

struct CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size){

    struct CptRequest *req;
    req = malloc(req_size);

    int * current = (int *) 2;

    req->version = req_buf[0];
    printf("version = %d\n", req_buf[0]);
    req->command = req_buf[1];
    printf("command = %d\n", req_buf[1]);
    req->channel_id = unpack_u16(req_buf, current);
    printf("channel id = %d\n", req_buf[2] + req_buf[3]);
    req->msg_len = unpack_u16(req_buf, current);
    printf("msg_len = %d\n", req_buf[4] + req_buf[5]);
    // req->msg = req_buf[6];

    char* temp = strdup(req_buf + 6);
    printf("res_msg = %s\n", temp);

    return req;
}

size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer)
{
    uint8_t temp[2];

    buffer[0] = req->version;
    buffer[1] = req-> command;
    pack_u16(req->channel_id, temp);
    buffer[2] = temp[0];
    buffer[3] = temp[1];
    pack_u16(req->msg_len, temp);
    buffer[4] = temp[0];
    buffer[5] = temp[1];

    size_t req_size;

    for (int i = 0; i < req->msg_len; i++) {
        buffer[6 + i] = (uint8_t) req->msg[i];
        printf("buffer[%d] = %c\n", 6 + i, req->msg[i]);
    }

    buffer[6 + req->msg_len] = '\0';

    req_size = 6 + req->msg_len;

    return req_size;
}

size_t cpt_serialize_response(struct CptResponse * res, uint8_t * buffer)
{
    uint8_t temp[2];

    buffer[0] = res->code;
    pack_u16(res->data_size, temp);
    buffer[1] = temp[0];
    buffer[2] = temp[1];
    pack_u16(res->channel_id, temp);
    buffer[3] = temp[0];
    buffer[4] = temp[1];
    pack_u16(res->user_id, temp);
    buffer[5] = temp[0];
    buffer[6] = temp[1];
    pack_u16(res->msg_len, temp);

    for (int i = 0; i < res->msg_len; i++) {
        buffer[7 + i] = (uint8_t) res->msg[i];
    }

    size_t res_size;
    res_size = 7 + res->msg_len;

    return res_size;
}

struct CptResponse * cpt_response_init(){
    struct CptResponse *res;

    res = malloc(sizeof (struct CptResponse *));

    res->code = 0;
    res->data_size = 0;
    res->channel_id = 0;
    res->user_id = 0;
    res->msg_len = 0;

    return res;
}

void cpt_response_destroy(struct CptResponse * response)
{
    if (response != NULL)
    {
        free(response);
        response = NULL;
    }
}

void cpt_response_reset(struct CptResponse * response)
{
    if (response->code != 0)
    {
        response->code = 0;
    }
    else if (response->data_size != 0)
    {
        response->data_size = 0;
    }
}

struct CptRequest * cpt_request_init()
{
    struct CptRequest *req;

    req = malloc(sizeof (struct CptRequest *));

    req->version = 0;
    req->command = 0;
    req->channel_id = 0;
    req->msg_len = 0;
    req->msg = NULL;

    return req;
}

void cpt_request_destroy(struct CptRequest * cpt)
{
    if (cpt != NULL)
    {
        cpt->version = 0;
        cpt->command = 0;
        cpt->channel_id = 0;
        cpt->msg_len = 0;
        cpt->msg = NULL;
        free(cpt);
        free(cpt->msg);
        cpt = NULL;
    }
}

void cpt_request_reset(struct CptRequest * packet)
{
    if (packet->version != 0)
    {
        packet->version = 0;
    }
    else if (packet->msg != NULL)
    {
        packet->msg = NULL;
        free(packet->msg);
    }
    else if (packet->channel_id != 0)
    {
        packet->channel_id = 0;
    }
    else if (packet->command != 0)
    {
        packet->command = 0;
    }
    else if (packet->msg_len != 0)
    {
        packet->msg_len = 0;
    }
}

void pack_u16(uint16_t value, uint8_t buf[2])
{
    uint16_t sig_byte_mask;
    uint16_t lsig_byte_mask;
    uint8_t sig_byte;
    uint8_t lsig_byte;

    sig_byte_mask = 0b1111111100000000;
    lsig_byte_mask = 0b0000000011111111;


    sig_byte = (uint8_t) ((value & sig_byte_mask) >> 8);
    lsig_byte = (uint8_t) (value & lsig_byte_mask);

    buf[0] = sig_byte;
    buf[1] = lsig_byte;
}

uint16_t unpack_u16(uint8_t * buf, int * count)
{
    uint16_t byte;

    byte = 0;
    byte = (uint16_t) (( * buf + byte) << 8);
    (count)++;
    buf++;

    byte += ( * buf);
    buf++;

    return byte;
}






