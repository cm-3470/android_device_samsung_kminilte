#include "audio_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INDENT "    "

struct enum_values_t {
    char* ctl;
    char* values[8];
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

//IFS=$'\t'; cat mix_values.txt | grep -v "\-" | grep "" | while read a1 a2 a3 a4 a5 a6 a7 a8; do echo "{\"$a1\", {\"$a2\", \"$a3\", \"$a4\", \"$a5\", \"$a6\", \"$a7\", \"$a8\"}},"; done  enums.h
static const struct enum_values_t enum_values[] = {
    {"AIF1DRC1 Mode", {"voice call DRC", 0}},
    {"AIF1DRC2 Mode", {"voice call DRC", 0}},
    {"AIF2DRC Mode", {"voice call DRC", 0}},
    {"Speaker Reference", {"SPKVDD/2", "VMID", 0}},
    {"Speaker Mode", {"Class D", "Class AB", 0}},
    {"AIF1ADCL Source", {"Left", "Right", 0}},
    {"AIF1ADCR Source", {"Left", "Right", 0}},
    {"AIF2ADCL Source", {"Left", "Right", 0}},
    {"AIF2ADCR Source", {"Left", "Right", 0}},
    {"AIF1DACL Source", {"Left", "Right", 0}},
    {"AIF1DACR Source", {"Left", "Right", 0}},
    {"AIF2DACL Source", {"Left", "Right", 0}},
    {"AIF2DACR Source", {"Left", "Right", 0}},
    {"Sidetone HPF Mux", {"2.7kHz", "1.35kHz", "675Hz", "370Hz", "180Hz", "90Hz", "45Hz", 0}},
    {"AIF1ADC1 HPF Mode", {"HiFi", "Voice 1", "Voice 2", "Voice 3", 0}},
    {"AIF1ADC2 HPF Mode", {"HiFi", "Voice 1", "Voice 2", "Voice 3", 0}},
    {"AIF2ADC HPF Mode", {"HiFi", "Voice 1", "Voice 2", "Voice 3", 0}},
    {"ADC OSR", {"Low Power", "High Performance", 0}},
    {"DAC OSR", {"Low Power", "High Performance", 0}},
    {"AIF1DAC1 Noise Gate Hold Time", {"30ms", "125ms", "250ms", "500ms", 0}},
    {"AIF1DAC2 Noise Gate Hold Time", {"30ms", "125ms", "250ms", "500ms", 0}},
    {"AIF2DAC Noise Gate Hold Time", {"30ms", "125ms", "250ms", "500ms", 0}},
    {"ADCR Mux", {"ADC", "DMIC", 0}},
    {"ADCL Mux", {"ADC", "DMIC", 0}},
    {"Right Headphone Mux", {"Mixer", "DAC", 0}},
    {"Left Headphone Mux", {"Mixer", "DAC", 0}},
    {"AIF3ADC Mux", {"AIF1ADCDAT", "AIF2ADCDAT", "AIF2DACDAT", "Mono PCM", 0}},
    {"AIF2DACR Mux", {"AIF2", "AIF3", 0}},
    {"AIF2DACL Mux", {"AIF2", "AIF3", 0}},
    {"Mono PCM Out Mux", {"None", "AIF2ADCL", "AIF2ADCR", 0}},
    {"AIF2ADC Mux", {"AIF2ADCDAT", "AIF3DACDAT", 0}},
    {"AIF2DAC Mux", {"AIF2DACDAT", "AIF3DACDAT", 0}},
    {"AIF1DAC Mux", {"AIF1DACDAT", "AIF3DACDAT", 0}},
    {"Right Sidetone", {"ADC/DMIC1", "DMIC2", 0}},
    {"Left Sidetone", {"ADC/DMIC1", "DMIC2", 0}},
    {"AIF2 Mode", {"Slave", "Master", 0}},
    {"SubMicBias Mode", {"Disable", "Force Disable", "Enable", "Force Enable", 0}},
    {"MainMicBias Mode", {"Disable", "Force Disable", "Enable", "Force Enable", 0}},
    {"AIF2 digital mute", {"Off", "On", 0}},
    {"Input Clamp", {"Off", "On", 0}},
    {"Headset STMR Mode", {"Off", "On", 0}},
    {"LineoutSwitch Mode", {"Off", "On", 0}},
};

static int section;
static FILE *f;

static void print_indent(int n)
{
    int i;
    for (i = 0; i < n; ++i)
        fprintf(f, INDENT);
}

static const char* get_enum_value(const char* ctl_name, int enum_val)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(enum_values); ++i)
    {
        if (strcmp(ctl_name, enum_values[i].ctl) == 0)
        {
            return enum_values[i].values[enum_val];
        }
    }
    return NULL;
}

static void add_rule(parse_state_t *state)
{
    if (section != state->section)
        return;
    
    int indent;    
    if (state->section == SECTION_CORE) {
        indent = 1;
    } else {
        indent = 2;
    }

    print_indent(indent);
    
    if (state->strval)
    {
        fprintf(f, "<ctl name=\"%s\" value=\"%s\" />\n", state->key, state->strval); 
#if 0
        printf("section: %d, name: %s, en: %d, key: '%s', value: '%s'\n",
                state->section, state->section_name, state->enable,
                state->key, state->strval);
#endif
    }
    else
    {
        const char* enumStr;
        if ((enumStr = get_enum_value(state->key, state->intval)))
        {
            fprintf(f, "<ctl name=\"%s\" value=\"%s\" />\n", state->key, enumStr); 
        }
        else
        {
            fprintf(f, "<ctl name=\"%s\" value=\"%d\" />\n", state->key, state->intval); 
        }
#if 0
        printf("section: %d, name: %s, en: %d, key: '%s', value: %d\n",
                state->section, state->section_name, state->enable,
                state->key, state->intval);
#endif
    }        
}

#define PREFIX_DEVICE   "Device"
#define PREFIX_VERB     "Verb"
#define PREFIX_MODIFIER "Modifier"
#define ENABLE_FLAG(e) (e ? "On" : "Off")

void mode_changed(parse_state_t *state, int oldMode)
{
    if (section != state->section)
        return;

    if (oldMode == PARSE_CTL && state->mode == PARSE_DEVVERBMOD) 
    {
        fprintf(f, INDENT"</path>\n\n");
    }    

    if (oldMode == PARSE_DEVVERBMOD && state->mode == PARSE_CTL) 
    {
        char *prefix;
        switch (state->section)
        {
            case SECTION_DEVICE: prefix = PREFIX_DEVICE; break;
            case SECTION_VERB: prefix = PREFIX_VERB; break;
            case SECTION_MODIFIER: prefix = PREFIX_MODIFIER; break;
            default: prefix= ""; break;
        }
        
        fprintf(f, INDENT"<path name=\"%s|%s|%s", 
                prefix, 
                ENABLE_FLAG(state->enable),
                state->section_name);
        
        if (state->section == SECTION_MODIFIER)
        {
            if (state->supportedDev)
                fprintf(f, "|%s", state->supportedDev);
            if (state->outputDev)
                fprintf(f, "|%s", state->outputDev);
        }

        fprintf(f, "\">\n");        
    }
}

int main(int argc, char **argv)
{
    parse_config_t config;
    config.add_rule = add_rule;
    config.mode_changed = mode_changed;
    
    f = fopen("mixer_paths.xml", "w");
    
    fprintf(f,
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
        "<mixer>\n"
        
    );
    
    for (section = SECTION_CORE; section <= SECTION_MODIFIER; ++section)
    {
        switch (section) {
            case SECTION_CORE: fprintf(f,INDENT"<!-- Initial mixer settings -->\n\n"); break;
            case SECTION_DEVICE: fprintf(f,INDENT"<!-- Devices -->\n\n"); break;
            case SECTION_VERB: fprintf(f,INDENT"<!-- Verbs -->\n\n"); break;
            case SECTION_MODIFIER: fprintf(f,INDENT"<!-- Modifiers -->\n\n"); break;
        }

        convert(&config, "tinyucm.conf");
        
        switch (section) {
            case SECTION_CORE: fprintf(f,"\n"INDENT"<!-- End of initial mixer settings -->\n\n"); break;
            case SECTION_DEVICE: fprintf(f,"\n"INDENT"<!-- End of devices -->\n\n"); break;
            case SECTION_VERB: fprintf(f,"\n"INDENT"<!-- End of verbs -->\n\n"); break;
            case SECTION_MODIFIER: fprintf(f,"\n"INDENT"<!-- End of modifiers -->\n\n"); break;
        }
    }
    
    fprintf(f,
        "</mixer>\n"
    );
    
    return 0;
}
