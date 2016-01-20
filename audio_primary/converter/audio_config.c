#include "audio_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define DELIM_WS " \t\r\n"

static void change_mode(parse_config_t *config, parse_state_t *state, int newMode)
{
    int old = state->mode;
    state->mode = newMode;
    config->mode_changed(state, old);

    if (old == PARSE_CTL)
    {
        state->intval = 0;
        state->strval = NULL;
    }
    
    if (state->mode == PARSE_MAIN)
    {
        free(state->section_name);
        state->section_name = NULL;

        free(state->supportedDev);
        state->supportedDev = NULL;
        
        free(state->outputDev);
        state->outputDev = NULL;
    }
}

static void skip_delims(char **str_ptr, char *delim)
{
    // skip "\s*,\s*"
    while (**str_ptr)
    {
        if (strchr(delim, **str_ptr) != 0)
            ++*str_ptr;
        else
            break;
    }
}

bool convert(parse_config_t *config, const char* in_fname)
{
    bool result = false;
    FILE *ucmFile = NULL;
    char *line = NULL;
    size_t lineBufSize = 0;
    ssize_t nread;
    parse_state_t state;

    ucmFile = fopen(in_fname, "r");
    if (!ucmFile) {
        printf("Could not open '%s' for input (%s)\n", in_fname, strerror(errno));
        goto close;
    }

    state.mode = PARSE_MAIN;
    state.section = SECTION_NONE;
    state.section_name = NULL;
    state.enable = false;
    state.key = NULL;
    state.intval = 0;
    state.strval = NULL;
    state.supportedDev = NULL;
    state.outputDev = NULL;
    state.modlist = MODLIST_NONE;
    
    while ((nread = getline(&line, &lineBufSize, ucmFile)) != -1)
    {
        char *tok;
        char *strtok_ptr = NULL;
      
        //printf("Line: %s", line);
        
        switch (state.mode)
        {
            case PARSE_MAIN: 
            {
                tok = strtok_r(line, DELIM_WS, &strtok_ptr);
                if (!tok)
                    break;

                if (strcmp(tok, "Core") == 0)
                {
                    change_mode(config, &state, PARSE_CTL);
                    state.section = SECTION_CORE;
                    state.section_name = strdup(tok);
                }
                else if (strcmp(tok, "Device") == 0)
                {
                    tok = strtok_r(NULL, "\"", &strtok_ptr);
                    change_mode(config, &state, PARSE_DEVVERBMOD);
                    state.section = SECTION_DEVICE;
                    state.section_name = strdup(tok);
                }
                else if (strcmp(tok, "Verb") == 0)
                {
                    tok = strtok_r(NULL, "\"", &strtok_ptr);
                    change_mode(config, &state, PARSE_DEVVERBMOD);
                    state.section = SECTION_VERB;
                    state.section_name = strdup(tok);
                }
                else if (strcmp(tok, "Modifier") == 0)
                {
                    tok = strtok_r(NULL, "\"", &strtok_ptr);
                    change_mode(config, &state, PARSE_DEVVERBMOD);
                    state.section = SECTION_MODIFIER;
                    state.section_name = strdup(tok);
                }
                else if (strcmp(tok, "InputRate") == 0)
                {
                }
                else if (strcmp(tok, "OutputRate") == 0)
                {
                }
                else if (strcmp(tok, "PlaybackLink") == 0)
                {
                }
                else if (strcmp(tok, "CaptureLink") == 0)
                {
                }
                else if (strcmp(tok, "BasebandLink") == 0)
                {
                }
                else if (strcmp(tok, "BluetoothLink") == 0)
                {
                }
                else if (strcmp(tok, "FmradioLink") == 0)
                {
                }
                else if (strcmp(tok, "ModifierFile") == 0)
                {
                    tok = strtok_r(NULL, "\"", &strtok_ptr);
                    if (!tok) break;
                    
                    printf("Modifier: %s\n", tok);
                    convert(config, tok);
                }
            
                break;
            }    

            case PARSE_DEVVERBMOD: 
            {
                tok = strtok_r(line, DELIM_WS, &strtok_ptr);
                if (!tok)
                    break;

                if (strcmp(tok, "Enable") == 0)
                {
                    state.enable = true;
                    change_mode(config, &state, PARSE_CTL);
                }
                else if (strcmp(tok, "Disable") == 0)
                {
                    state.enable = false;
                    change_mode(config, &state, PARSE_CTL);
                }
                else if (strcmp(tok, "SupportedDevice") == 0) // Modifier only
                {
                    state.enable = false;
                    state.modlist = MODLIST_DEVSUPP;
                    change_mode(config, &state, PARSE_LIST);                    
                }
                else if (strcmp(tok, "OutputDevice") == 0) // Modifier only
                {
                    state.enable = false;
                    state.modlist = MODLIST_DEVOUT;
                    change_mode(config, &state, PARSE_LIST);
                }
                else if (strcmp(tok, "}") == 0)
                {
                    change_mode(config, &state, PARSE_MAIN); 
                }
                
                break;
            }

            case PARSE_CTL: 
            {
                tok = strtok_r(line, DELIM_WS, &strtok_ptr);
                if (!tok)
                    break;

                //printf("CTL: '%s'\n", tok);
                
                if (strcmp(tok, "{") == 0)
                {                    
                    state.key = strtok_r(NULL, "\"", &strtok_ptr);
                    if (!state.key)
                        break;
 
                    skip_delims(&strtok_ptr, ", \t\r\n");
                    
                    if (!*strtok_ptr)
                        break;

                    state.strval = NULL;
                    state.intval = 0;
                    
                    if (*strtok_ptr == '\"')
                    {
                        // string
                        state.strval = strtok_r(NULL, "\"", &strtok_ptr);
                        if (!state.strval)
                            break;                        
                        config->add_rule(&state);
                    }
                    else
                    {
                        // integer
                        char *valueStr = strtok_r(NULL, DELIM_WS, &strtok_ptr);
                        if (!valueStr)
                            break;
                        
                        state.intval = atoi(valueStr);
                        config->add_rule(&state);                        
                    }                    
                }
                else if (strcmp(tok, "}") == 0)
                {
                    switch(state.section) 
                    {
                        case SECTION_DEVICE:
                        case SECTION_VERB:
                        case SECTION_MODIFIER:
                            change_mode(config, &state, PARSE_DEVVERBMOD);
                            break;
                        default:
                            change_mode(config, &state, PARSE_MAIN);
                            break;
                    }
                }
                
                break;
            }

            case PARSE_LIST: 
            {
                char *str_ptr = line;
                
                skip_delims(&str_ptr, " \t");
                
                if (str_ptr[0] == '}')
                {
                    change_mode(config, &state, PARSE_DEVVERBMOD);
                }
                else if (str_ptr[0] == '"')
                {
                    tok = strtok_r(str_ptr, "\"", &strtok_ptr);
                    if (state.modlist == MODLIST_DEVSUPP) {
                        state.supportedDev = strdup(tok);
                    } else {
                        state.outputDev = strdup(tok);
                    }
                }
                
                break;
            }
            
            default:
                break;
        }
    }
    
    result = true;

close:
    free(line);
    
    if (ucmFile)
      fclose(ucmFile);
    
    return result;
}
