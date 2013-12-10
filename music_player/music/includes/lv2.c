#include <string.h>
#include <malloc.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>

#include <audio.h>
#include <lv2.h>

#include "../descriptors.h"


typedef struct urid_map_entry {
    const char* uri;
    uint32_t id;
    struct urid_map_entry* next;
} urid_map_entry;

static urid_map_entry* urid_table;

static LV2_URID urid_map_func(LV2_URID_Map_Handle handle, const char* uri) {
    static LV2_URID id = 1;
    urid_map_entry* next;
    if (urid_table == NULL) {
        urid_table = malloc(sizeof(urid_map_entry));
        urid_table->uri = "http://www.joebutton.co.uk/pitracker/urid-map-error";
        urid_table->id = 0;
        urid_table->next = NULL;
    }

    urid_map_entry* cur = urid_table;

    while (1) {
        if (!strcmp(cur->uri, uri)) {
            return cur->id;
        }
        if (cur->next == NULL) {
            next = malloc(sizeof(urid_map_entry));
            next->uri = uri;
            next->id = id++;
            next->next = NULL;
            cur->next = next;
            return next->id;
        } else cur = cur->next;
    }
}

static const char *urid_unmap_func(LV2_URID_Unmap_Handle handle, LV2_URID urid) {
    urid_map_entry* cur = urid_table;
    while (1) {
        if (cur->id == urid) {
            return cur->uri;
        }
        if (cur->next == NULL) {
            return NULL;
        }
        cur = cur->next;
    }
}

LV2_URID_Map lv2_urid_map = {NULL, urid_map_func};
static LV2_URID_Unmap lv2_urid_unmap = {NULL, urid_unmap_func};

static LV2_Feature map_feature       = { LV2_URID__map, &lv2_urid_map };
static LV2_Feature unmap_feature     = { LV2_URID__unmap, &lv2_urid_unmap };


// function pointer for running our descriptor functions
static int (*get_descriptor)(uint32_t index);


static void load_lv2_descriptors(Lv2World *world) {
    unsigned int i;
    Lv2Plugin *cur, *prev=NULL, *first=NULL;
    for (i=0;lv2_descriptor_loaders[i];i++) {
        cur = malloc(sizeof(Lv2Plugin));
        cur->next = NULL;
        if (prev) prev->next = cur;
        else first=cur;
        get_descriptor = lv2_descriptor_loaders[i];
        cur->descriptor = (LV2_Descriptor*)get_descriptor(0);

        cur->handle = cur->descriptor->instantiate(
                                                 cur->descriptor,
                                                 world->sample_rate,
                                                 NULL,
                                                 world->lv2_features);
        prev = cur;
    }
    world->num_plugins = i;
    world->plugin_list = first;
}

Lv2World *lv2_init(uint32_t sample_rate) {
    Lv2World *world = malloc(sizeof(Lv2World));
    world->sample_rate = sample_rate;
    urid_table = NULL;
    world->lv2_features[0] = &map_feature;
    world->lv2_features[1] = &unmap_feature;
    world->lv2_features[2] = NULL ;
	lv2_atom_forge_init(&forge, &lv2_urid_map);
    load_lv2_descriptors(world);
    return world;
}


lv2_port *new_lv2_port(enum lv2_port_type type, uint32_t id) {
    lv2_port *port = malloc(sizeof(lv2_port));
    port->type = type;
    port->id = id;
    if (type == lv2_audio_port) {
        port->buffer = malloc(sizeof(float) * LV2_AUDIO_BUFFER_SIZE);
        port->buffer_sz = LV2_AUDIO_BUFFER_SIZE;
    } else if (type == lv2_atom_port) {
        port->buffer = malloc(sizeof(uint8_t) * LV2_ATOM_BUFFER_SIZE);
        port->buffer_sz = LV2_ATOM_BUFFER_SIZE;
    }
    return port;
}

