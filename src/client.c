#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define SEND 0
#define LOGOUT 1
#define LOGIN 2
#define GET_USERS 3
#define CREATE_CHAN 4
#define JOIN_CHAN 6
#define LEAVE_CHAN 7
#define CREATE_PRIVATE_CHAN 8

struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string *IP;
    struct dc_setting_string *port;
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

size_t cpt_login(void * client_info, uint8_t * serial_buf, char * name);
size_t cpt_logout(void * client_info, uint8_t * serial_buf);
size_t cpt_get_users(void * client_info, uint8_t * serial_buf, uint16_t channel_id);
size_t cpt_create_channel(void * client_info, uint8_t * serial_buf, char * user_list);
size_t cpt_join_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id);
size_t cpt_leave_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id);
int cpt_send(void * client_info, uint8_t * serial_buf, char * msg);

struct CptRequest * cpt_request_init(void);
void cpt_request_destroy(struct CptRequest * cpt);
void cpt_request_reset(struct CptRequest * packet);

size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer);
size_t cpt_serialize_request(struct CptRequest * req, uint8_t * buffer);


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
    settings->port = dc_setting_string_create(env, err);
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
                    dc_options_set_string,
                    "port",
                    required_argument,
                    'p',
                    "PORT",
                    dc_string_from_string,
                    "port",
                    dc_string_from_config,
                    "8080"},
            {(struct dc_setting *)settings->ID,
                    dc_options_set_string,
                    "id",
                    required_argument,
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
    dc_setting_string_destroy(env, &app_settings->port);
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
    const char *message;

    DC_TRACE(env);

    app_settings = (struct application_settings *)settings;




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



struct CptRequest * cpt_request_init(){
    struct CptRequest *req = NULL;

    req->version = 1;

    return req;
}

void cpt_request_destroy(struct CptRequest * cpt){

}

void cpt_request_reset(struct CptRequest * packet){

}

size_t cpt_login(void * client_info, uint8_t * serial_buf, char * name){
    size_t status = 0;

    return status;
}
size_t cpt_logout(void * client_info, uint8_t * serial_buf){
    size_t status = 0;

    return status;
}
size_t cpt_get_users(void * client_info, uint8_t * serial_buf, uint16_t channel_id){
    size_t status = 0;

    return status;
}
size_t cpt_create_channel(void * client_info, uint8_t * serial_buf, char * user_list){
    size_t status = 0;

    return status;
}
size_t cpt_join_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id){
    size_t status = 0;

    return status;
}
size_t cpt_leave_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id){

}
int cpt_send(void * client_info, uint8_t * serial_buf, char * msg){
    int status = 0;

    return status;
}
