//
// Created by saga910 on 2022-03-26.
//

#ifndef CHAT_ASSIGNMNET_CPT_SERVER_H
#define CHAT_ASSIGNMNET_CPT_SERVER_H

#include "common.h"

typedef struct userList{
    int userCount;
    struct userList *next;
}userList;

typedef struct user{
    int user_id;
    int user_fd;
    struct user *next;
}user;

typedef struct channel{
    uint16_t channel_id;
    struct userList *users;
    struct channel *next;
}channel;

struct serverInfo{
    channel global;
};

/**
 * Initialize a channelList object
 *
 */
channel *createChannelList(void);

/**
 *
 * @param list
 * @param input
 */
void add_Channel(channel *list,channel *input);

/**
 *
 * @param list
 * @param id
 * @return
 */
channel * create_channel(userList *list, uint16_t id);

/**
 *
 * @param ch
 */
void destroy_channel(channel *ch);

/**
 *
 * @param client
 */
void destroy_user(user *client);

/**
 *
 * @param fd
 * @param id
 * @return
 */
user * create_user(int fd, int id);

/**
 * Handle a received 'LOGIN' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LOGIN protocol message from a connected client.
 *
 * If successful, the protocol request will be fulfilled,
 * updating any necessary information contained within
 * <server_info>.
 *
 * @param name          Name of user in received Packet MSG field.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_login_response(char * name);

/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a LOGOUT protocol message from a connected client.
 *
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_logout_response(void);

/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a GET_USERS protocol message from a connected client.
 *
 * If successful, the function should collect user information
 * from the channel in the CHAN_ID field of the request packet
 * in the following format:
 *
 *  <user_id><whitespace><username><newline>
 *
 * Example given:
 *      1 'Clark Kent'
 *      2 'Bruce Wayne'
 *      3 'Fakey McFakerson'
 *
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_get_users_response(uint16_t channel_id);

/**
 * Handle a received 'JOIN_CHANNEL' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a JOIN_CHANNEL protocol message from a connected client.
 * If successful, function should add the requesting client
 * user into the channel specified by the CHANNEL_ID field
 * in the CptPacket <channel_id>.
 *
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_join_channel_response(uint16_t channel_id);

/**
 * Handle a received 'CREATE_CHANNEL' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a CREATE_CHANNEL protocol message from a connected client.
 *
 * If a <user_list> was received in the MSG field of the packet,
 * function will also parse the <user_list> string and attempt
 * to add the requested user IDs to the channel.
 *
 * If <id_list> is NULL, function will create a new channel with
 * only the requesting user within it.
 *
 * @param id_list       ID list from MSG field of received CPT packet.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_create_channel_response(char * id_list);

/**
 * Handle a received 'LEAVE_CHANNEL' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LEAVE_CHANNEL protocol message from a connected client.
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_leave_channel_response(uint16_t channel_id);

/**
 * Handle a received 'SEND' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a SEND protocol message from a connected client.
 *
 * If successful, function will send the message in the
 * MSG field of the received packet to every user in the
 * CHAN_ID field of the received packet.
 *
 * @param name          Name of user in received Packet MSG field.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_send_response(char * name);


#endif //CHAT_ASSIGNMNET_CPT_SERVER_H
