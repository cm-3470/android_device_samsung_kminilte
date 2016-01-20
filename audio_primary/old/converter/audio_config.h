#ifndef _AUDIO_CONFIG_H_
#define _AUDIO_CONFIG_H_

#include <stdbool.h>

enum {
    PARSE_MAIN,
    PARSE_DEVVERBMOD,
    PARSE_CTL,
    PARSE_LIST
};

enum {
    SECTION_NONE,
    SECTION_CORE,
    SECTION_DEVICE,
    SECTION_VERB,
    SECTION_MODIFIER
};

enum {
    MODLIST_NONE,
    MODLIST_DEVSUPP,
    MODLIST_DEVOUT
};

typedef struct parse_state_t 
{
    int mode;
    int section;
    char *section_name;
    bool enable;
    char* key;
    int intval;
    char* strval;
    char* supportedDev;
    char* outputDev;
    int modlist;
} parse_state_t;

typedef void (*mode_changed_cb_t)(parse_state_t *state, int oldMode);
typedef void (*add_rule_cb_t)(parse_state_t *state);

typedef struct parse_config_t
{
    add_rule_cb_t add_rule;
    mode_changed_cb_t mode_changed;
} parse_config_t;


bool convert(parse_config_t *config, const char* in_fname);

#endif