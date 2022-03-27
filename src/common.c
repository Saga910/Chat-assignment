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
#include "common.h"

struct CptResponse * cpt_parse_response(uint8_t * res_buf, size_t data_size){
    struct CptResponse *res;
    res = malloc(sizeof (struct CptResponse *));

    //serialize??

    return res;
}

struct CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size){
    struct CptRequest *req;
    req = malloc(sizeof (struct CptRequest *));

    //serialize??

    return req;
}

size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer){
    size_t status = 0;
    return status;
}

size_t cpt_serialize_response(struct CptResponse * res, uint8_t * buffer){
    size_t status = 0;
    return status;
}

struct CptResponse * cpt_response_init(){
    struct CptResponse *res;

    res = malloc(sizeof (struct CptResponse *));

    res->code = 0;
    res->data = NULL;
    res->data_size = 0;

    return res;
}

void cpt_response_destroy(struct CptResponse * response)
{
    if (response != NULL)
    {
        response = NULL;
        free(response);
    }
}

void cpt_response_reset(struct CptResponse * response)
{
    if (response->code != 0)
    {
        response->code = 0;
    }
    else if (response->data != NULL)
    {
        response->data = NULL;
        free(response->data);
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
    }
}

void cpt_request_reset(struct CptRequest * packet)
{
    struct CptRequest *res = packet;
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
    else if (packet->msg_len != 0)
    {
        packet->msg_len = 0;
    }
}
