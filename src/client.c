#include "common.h"
#include "cpt_client.h"
#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dc_posix/dc_netdb.h>
#include <dc_posix/sys/dc_socket.h>

#define BUFFER 1024
#define FALSE 0

struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string *IP;
    struct dc_setting_uint16 *port;
    struct dc_setting_string *ID;
};



static struct dc_application_settings *create_settings(const struct dc_posix_env *env, struct dc_error *err);
static int destroy_settings(const struct dc_posix_env *env,
                            struct dc_error *err,
                            struct dc_application_settings **psettings);
static int run(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings);
static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env,
                           const char *file_name,
                           const char *function_name,
                           size_t line_number);

int main(int argc, char *argv[])
{
    dc_posix_tracer tracer;
    dc_error_reporter reporter;
    struct dc_posix_env env;
    struct dc_error err;
    struct dc_application_info *info;
    int ret_val;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);
    info = dc_application_info_create(&env, &err, "Chat Application");
    ret_val = dc_application_run(&env, &err, info, create_settings, destroy_settings, run, dc_default_create_lifecycle, dc_default_destroy_lifecycle, NULL, argc, argv);
    dc_application_info_destroy(&env, &info);
    dc_error_reset(&err);

    return ret_val;
}

static struct dc_application_settings *create_settings(const struct dc_posix_env *env, struct dc_error *err)
{
    static const uint16_t defaultport = 8080;
    struct application_settings *settings;

    DC_TRACE(env);
    settings = dc_malloc(env, err, sizeof(struct application_settings));

    if(settings == NULL)
    {
        return NULL;
    }

    settings->opts.parent.config_path = dc_setting_path_create(env, err);
    settings->IP = dc_setting_string_create(env, err);
    settings->port = dc_setting_uint16_create(env, err);
    settings->ID = dc_setting_string_create(env, err);


    struct options opts[] = {
            {(struct dc_setting *)settings->opts.parent.config_path,
                    dc_options_set_path,
                    "config",
                    required_argument,
                    'c',
                    "CONFIG",
                    dc_string_from_string,
                    NULL,
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->IP,
                    dc_options_set_string,
                    "ip",
                    required_argument,
                    'i',
                    "IP",
                    dc_string_from_string,
                    "ip",
                    dc_string_from_config,
                    "127.0.0.1"},
            {(struct dc_setting *)settings->port,
                    dc_options_set_uint16,
                    "port",
                    required_argument,
                    'p',
                    "PORT",
                    dc_string_from_string,
                    "port",
                    dc_string_from_config,
                    &defaultport},
            {(struct dc_setting *)settings->ID,
                    dc_options_set_string,
                    "id",
                    optional_argument,
                    'd',
                    "ID",
                    dc_string_from_string,
                    "id",
                    dc_string_from_config,
                    "Default"}
    };

    // note the trick here - we use calloc and add 1 to ensure the last line is all 0/NULL
    settings->opts.opts_count = (sizeof(opts) / sizeof(struct options)) + 1;
    settings->opts.opts_size = sizeof(struct options);
    settings->opts.opts = dc_calloc(env, err, settings->opts.opts_count, settings->opts.opts_size);
    dc_memcpy(env, settings->opts.opts, opts, sizeof(opts));
    settings->opts.flags = "i:p:d";
    settings->opts.env_prefix = "DC_CHAT_";

    return (struct dc_application_settings *)settings;
}

static int destroy_settings(const struct dc_posix_env *env,
                            __attribute__((unused)) struct dc_error *err,
                            struct dc_application_settings **psettings)
{
    struct application_settings *app_settings;

    DC_TRACE(env);
    app_settings = (struct application_settings *)*psettings;
    dc_setting_string_destroy(env, &app_settings->ID);
    dc_setting_string_destroy(env, &app_settings->IP);
    dc_setting_uint16_destroy(env, &app_settings->port);
    dc_free(env, app_settings->opts.opts, app_settings->opts.opts_count);
    dc_free(env, *psettings, sizeof(struct application_settings));

    if(env->null_free)
    {
        *psettings = NULL;
    }

    return 0;
}

static int run(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings)
{
    struct application_settings *app_settings;
    int sockfd = -1;
    char   send_buf[BUFFER];
    char   recv_buf[BUFFER];
    const char *server;
    uint16_t port;
    ssize_t rc;
    struct sockaddr_in6 *sockaddrIn;
    struct addrinfo hints;
    struct addrinfo *res=NULL;
    in_port_t converted_port;
    socklen_t size;

    DC_TRACE(env);

    app_settings = (struct application_settings *)settings;

    server = dc_setting_string_get(env, app_settings->IP);
    port = dc_setting_uint16_get(env, app_settings->port);
    converted_port = htons(port);

    do
    {
        dc_memset(env, &hints, 0, sizeof(hints));
        hints.ai_family = PF_INET6;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_V4MAPPED;
        rc = dc_getaddrinfo(env, err, server, NULL, &hints, &res);

        if (rc != 0)
        {
            printf("Host not found --> %s\n", gai_strerror(rc));
            if (rc == EAI_SYSTEM)
                perror("getaddrinfo() failed");
            break;
        }


        sockfd = dc_socket(env, err, res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
        {
            perror("socket() failed");
            break;
        }

        sockaddrIn = (struct sockaddr_in6 *)res->ai_addr;
        sockaddrIn->sin6_port = converted_port;
        size = sizeof(struct sockaddr_in6);

        rc = dc_connect(env, err, sockfd, res->ai_addr, size);

        if (rc < 0)
        {
            perror("connect() failed");
            break;
        }

        while (1){

            rc = read(STDIN_FILENO, send_buf, sizeof(send_buf));

            if (rc < 0)
            {
                perror("read() failed");
                break;
            }

            if(getchar() == '\n'){

                struct CptRequest *req;


                rc = send(sockfd, send_buf, sizeof(send_buf), 0);

                if (rc < 0)
                {
                    perror("send() failed");
                    break;
                }

            }


        }


//        rc = recv(sockfd, recv_buf, sizeof(recv_buf), 0);

        if (rc < 0)
        {
            perror("recv() failed");
            break;
        }

//        printf("Rec: %s\n", recv_buf);

    } while (FALSE);


    if (sockfd != -1){
        close(sockfd);
    }

    if (res != NULL){
        freeaddrinfo(res);
    }

    return EXIT_SUCCESS;
}

static void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "ERROR: %s : %s : @ %zu : %d\n", err->file_name, err->function_name, err->line_number, 0);
    fprintf(stderr, "ERROR: %s\n", err->message);
}

static void trace_reporter(__attribute__((unused)) const struct dc_posix_env *env,
                           const char *file_name,
                           const char *function_name,
                           size_t line_number)
{
    fprintf(stdout, "TRACE: %s : %s : @ %zu\n", file_name, function_name, line_number);
}

size_t cpt_login(void * client_info, uint8_t * serial_buf, char * name)
{
    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->command = LOGIN;
    new_req->msg = name;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;
}

size_t cpt_logout(void * client_info, uint8_t * serial_buf)
{
    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->command = LOGOUT;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;
}

size_t cpt_get_users(void * client_info, uint8_t * serial_buf, uint16_t channel_id)
{

    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->command = GET_USERS;
    new_req->channel_id = channel_id;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;

}

size_t cpt_create_channel(void * client_info, uint8_t * serial_buf, char * user_list){
    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->channel_id = 0;
    new_req->command = CREATE_CHANNEL;
    new_req->msg = user_list;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;
}

size_t cpt_join_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id){
    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->channel_id = channel_id;
    new_req->command = JOIN_CHANNEL;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;
}

size_t cpt_leave_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id){

    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->channel_id = channel_id;
    new_req->command = LEAVE_CHANNEL;
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;

}

size_t cpt_send(void * client_info, uint8_t * serial_buf, char * msg)
{
    struct CptRequest * new_req;
    new_req = cpt_request_init();
    new_req->msg = msg;
    new_req->command = SEND;
    new_req->msg_len = sizeof (msg);
    size_t req_size;
    req_size = cpt_serialize_request(new_req, serial_buf);

    return req_size;

}

