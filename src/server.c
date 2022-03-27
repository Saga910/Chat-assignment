#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <dc_posix/sys/dc_socket.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include "cpt_server.h"


struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string *IP;
    struct dc_setting_uint16 *port;
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
                    (const void *) 8080},
    };

    // note the trick here - we use calloc and add 1 to ensure the last line is all 0/NULL
    settings->opts.opts_count = (sizeof(opts) / sizeof(struct options)) + 1;
    settings->opts.opts_size = sizeof(struct options);
    settings->opts.opts = dc_calloc(env, err, settings->opts.opts_count, settings->opts.opts_size);
    dc_memcpy(env, settings->opts.opts, opts, sizeof(opts));
    settings->opts.flags = "c:i:p";
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
    int socket_addr = -1, reusable, on = 1, rc, timeout, current_size=0, nfds = 1;
    struct sockaddr_in6 sockaddrIn;
    struct addrinfo *addrinfo;
    uint16_t *port;
    struct pollfd pollfd[200];

    DC_TRACE(env);

    app_settings = (struct application_settings *)settings;
    port = (uint16_t *) app_settings->port;

    socket_addr = socket(AF_INET6, SOCK_STREAM, 0);

    if(socket_addr < 0){
        printf("Failed to create a socket.\n");
        exit(-1);
    }

    reusable = setsockopt(socket_addr, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    if(reusable < 0){
        printf("failed to set socket opt.\n");
        exit(-1);
    }

    rc = ioctl(socket_addr, FIONBIO, (char *)&on);

    if(rc < 0){
        perror("ioctl() failed");
        close(socket_addr);
        exit(-1);
    }

    memset(&sockaddrIn, 0, sizeof(sockaddrIn));
    sockaddrIn.sin6_family = AF_INET6;
    memcpy(&sockaddrIn.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    sockaddrIn.sin6_port = htons(*port);
    rc = bind(socket_addr, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if(rc < 0){
        perror("bind() failed");
        close(socket_addr);
        exit(-1);
    }

    rc = listen(socket_addr, 32);
    if(rc < 0){
        perror("listen() failed");
        close(socket_addr);
        exit(-1);
    }

    memset(pollfd, 0, sizeof(pollfd));
    pollfd[0].fd = socket_addr;
    pollfd[0].events = POLLIN;

    timeout = (3 * 60 * 1000);

    do {
        printf("Waiting on poll()....\n");

        rc = poll(pollfd, (nfds_t) nfds, timeout);
        if(rc < 0){
            perror("  poll() failed");
            break;
        }

        if(rc == 0){
            printf(" Poll timeout. \n");
            break;
        }

        current_size = nfds;

        for(int i = 0; i < current_size; i++){

        }

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


int cpt_login_response(void * server_info, char * name){
    int status = 0;

    return status;
}
int cpt_logout_response(void * server_info){
    int status = 0;

    return status;
}
int cpt_get_users_response(void * server_info, uint16_t channel_id){
    int status = 0;

    return status;
}
int cpt_join_channel_response(void * server_info, uint16_t channel_id){
    int status = 0;

    return status;
}
int cpt_create_channel_response(void * server_info, char * id_list){
    int status = 0;

    return status;
}
int cpt_leave_channel_response(void * server_info, uint16_t channel_id){
    int status = 0;

    return status;
}
int cpt_send_response(void * server_info, char * name){
    int status = 0;

    return status;
}
