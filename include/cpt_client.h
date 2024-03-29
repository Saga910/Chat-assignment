//
// Created by saga910 on 2022-03-26.
//

#ifndef CHAT_ASSIGNMNET_CPT_CLIENT_H
#define CHAT_ASSIGNMNET_CPT_CLIENT_H

#include "common.h"
#include <arpa/inet.h>

struct client_info
{
    uint16_t user_id;
    uint16_t channel_id;
};

/**
 * Prepare a LOGIN request packet for the server.
 *
 * Prepares a LOGIN request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to
 * persist the client's information until cpt_logout() is called.
 *
 * @param cpt            CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param name           Client login name.
 * @return The size of the serialized packet in <serial_buf>.
*/

size_t cpt_login(uint8_t * serial_buf, char * name);

/**
 * Prepare a LOGOUT request packet for the server.
 *
 * Prepares a LOGOUT request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to remove
 * any instance of the requesting client's information.
 *
 * @param cpt            CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_logout(uint8_t * serial_buf);

/**
 * Prepare a GET_USERS request packet for the server.
 *
 * Prepares a GET_USERS request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to
 * send back a list of users specified by the <channel_id>.
 *
 * @param client_info    CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param channel_id     The ID of the CHANNEL to get users from.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_get_users(uint8_t * serial_buf, uint16_t channel_id);

/**
 * Prepare a CREATE_CHANNEL request packet for the server.
 *
 * Prepares a CREATE_CHANNEL request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to create
 * a new channel.
 *
 *      > <user_list> may be optionally passed as user selection
 *        parameters for the new Channel.
 *      > If <members> is not NULL, it will be assigned to the
 *        MSG field of the packet.
 *
 * @param cpt            CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param user_list      Whitespace separated user IDs as a string.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_create_channel(uint8_t * serial_buf, char * user_list);

/**
 * Prepare a JOIN_CHANNEL request packet for the server.
 *
 * Prepares a JOIN_CHANNEL request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to add
 * the client's information to an existing channel.
 *
 * @param client_info    CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param channel_id     The target channel id.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_join_channel(uint8_t * serial_buf, uint16_t channel_id);

/**
 * Prepare a LEAVE_CHANNEL request packet for the server.
 *
 * Prepares a LEAVE_CHANNEL request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to remove the client's information
 * from the channel specified by <channel_id>.
 *
 * @param client_info    CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param channel_id     The target channel id.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_leave_channel(uint8_t * serial_buf, uint16_t channel_id);

/**
 * Prepare a SEND request packet for the server.
 *
 * Prepares a SEND request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to SEND
 * the message specified by <msg> to ever user in the channel
 * specified within the packet CHAN_ID field.
 *
 * @param client_info    CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param msg            Intended chat message.
 * @return Size of the resulting serialized packet in <serial_buf>.
*/
size_t cpt_send(uint8_t * serial_buf, char * msg);


#endif //CHAT_ASSIGNMNET_CPT_CLIENT_H
