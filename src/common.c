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

#include "common.h"

struct CptResponse * cpt_parse_response(uint8_t * res_buf, size_t data_size){
    struct CptRequest *req = NULL;

    return req;
}

struct CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size){
    struct CptRequest *req = NULL;

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
    struct CptRequest *req = NULL;

    req->version = 1;

    return req;
}

void cpt_response_destroy(struct CptResponse * response){

}

void cpt_response_reset(struct CptResponse * response){

}

struct CptRequest * cpt_request_init(){
    struct CptRequest *req = NULL;

    req->version = 1;

    return req;
}

void cpt_request_destroy(struct CptRequest * cpt){

}

void cpt_request_reset(struct CptRequest * packet){

}
