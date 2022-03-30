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
    res = malloc(sizeof (struct CptResponse *));

    char * res_content = NULL;
    res_content = malloc(data_size);
    sprintf(res_content, "%s", res_buf);

    char code_arr[8] = {0,};
    char data_size_arr[16] = {0,};
    char chanid_arr[16] = {0,};
    char userid_arr[16] = {0,};
    char msglen_arr[16] = {0,};

    strncpy(code_arr, res_content, 8);
    strncpy(data_size_arr, res_content + 8, 16);
    strncpy(chanid_arr, res_content + 16, 16);
    strncpy(userid_arr, res_content + 32, 16);
    strncpy(msglen_arr, res_content + 48, 16);

    res->code = (uint8_t) bin_to_dec(code_arr);
    res->data_size = (uint16_t) bin_to_dec(data_size_arr);
    res->channel_id = (uint16_t) bin_to_dec(chanid_arr);
    res->user_id = (uint16_t) bin_to_dec(userid_arr);
    res->msg_len = (uint16_t) bin_to_dec(msglen_arr);

    for (int i = 0; i < strlen(res_content) - 64; i++) {
        res->msg[i] = (uint8_t) res_content[64 + i];
    }

    return res;
}

struct CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size){

    struct CptRequest *req;
    req = malloc(sizeof (struct CptRequest *));

    char * req_content = NULL;
    req_content = malloc(req_size);
    sprintf(req_content, "%s", req_buf);

    char version_arr[8] = {0,};
    char command_arr[8] = {0,};
    char chanid_arr[16] = {0,};
    char msglen_arr[16] = {0,};

    strncpy(version_arr, req_content, 8);
    strncpy(command_arr, req_content + 8, 8);
    strncpy(chanid_arr, req_content + 16, 16);
    strncpy(msglen_arr, req_content + 32, 16);

    req->version = (uint8_t) bin_to_dec(version_arr);
    req->command = (uint8_t) bin_to_dec(command_arr);
    req->channel_id = (uint16_t) bin_to_dec(chanid_arr);
    req->msg_len = (uint16_t) bin_to_dec(msglen_arr);
    strncpy(req->msg, req_content + 48, req_size);

    return req;
}

size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer)
{

    char * req_version = uint8_to_bin(req->version);
    char * req_command = uint8_to_bin(req->command);
    char * req_chan = uint16_to_bin(req->channel_id);
    char * req_msglen = uint16_to_bin(req->msg_len);

    char * concat_str = NULL;
    concat_str = malloc(sizeof(char *) * 6 + req->msg_len + 1);
    buffer = malloc(sizeof(uint8_t *) + 1);

    sprintf(concat_str, "%s%s%s%s%s", req_version, req_command, req_chan, req_msglen, req->msg);
    printf("conc: %s\n", concat_str);

    buffer = (uint8_t *) strdup(concat_str);
    printf("buff: %s\n", buffer);

    size_t req_size;
    req_size = 6 + req->msg_len;

    free(concat_str);

    return req_size;


}

size_t cpt_serialize_response(struct CptResponse * res, uint8_t * buffer)
{

        char * res_code = uint8_to_bin(res->code);
        char * res_data_size = uint16_to_bin(res->data_size);
        char * res_chan = uint16_to_bin(res->channel_id);
        char * res_user = uint16_to_bin(res->user_id);
        char * res_msglen = uint16_to_bin(res->msg_len);

        char * concat_str = NULL;
        concat_str = malloc(sizeof(char *) * 6 + res->msg_len + 1);
        buffer = malloc(sizeof(uint8_t *) + 1);

        sprintf(concat_str, "%s%s%s%s%s%s", res_code, res_data_size, res_chan, res_user, res_msglen, res->msg);
        printf("conc: %s\n", concat_str);

        buffer = (uint8_t *) strdup(concat_str);
        printf("buff: %s\n", buffer);

        size_t res_size;
        res_size = 6 + res->msg_len;

        free(concat_str);

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

char * uint8_to_bin(uint8_t num)
{
    int bits = 8;
    char * bin_str;
    bin_str = malloc(bits + 1);

    for(int i = bits - 1; i >= 0; i--)
    {
        bin_str[i] = (num & 1) + '0';
        num >>= 1;
    }
    bin_str[bits] = '\0';
    return bin_str;
}

char * uint16_to_bin(uint16_t num)
{
    int bits = 16;
    char * bin_str;

    bin_str = malloc(bits + 1);

    for(int i = bits - 1; i >= 0; i--)
    {
        bin_str[i] = (num & 1) + '0';
        num >>= 1;
    }
    bin_str[bits] = '\0';
    return bin_str;
}

int bin_to_dec(char * bin_arr)
{
    int x;
    int dec = 0;

    int len = strlen(bin_arr);
    for (int i = 0; i < len; i++)
    {
        x = (i == 0) ? 1 : 2 * x;
        dec += (bin_arr[len - i - 1] - '0') * x;
    }

    return dec;
}







