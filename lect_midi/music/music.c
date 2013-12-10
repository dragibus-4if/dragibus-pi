#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <unistd.h>
#include <config.h>

#include <lv2/lv2plug.in/ns/ext/atom/forge.h>

#include <audio.h>
#include <uart.h>
#include <hw.h>

#include <mf_read.h>
#include <lv2.h>

Lv2World* lv2_world;

void
music_init()
{
  int32_t samplerate = audio_init();
  lv2_world = lv2_init(samplerate);
}

void
play_music()
{
    uint32_t counter=0;
    LV2_Atom_Forge_Frame midi_seq_frame;
    int buffer_processed = 0;
    const Lv2Plugin *plugin;
    plugin = lv2_world->plugin_list;

    DISABLE_IRQ();

    lv2_port *output_left = new_lv2_port(lv2_audio_port, 1);
    lv2_port *output_right = new_lv2_port(lv2_audio_port, 2);
    lv2_port *midi_in = new_lv2_port(lv2_atom_port, 3);

    plugin->descriptor->connect_port(plugin->handle, midi_in->id, midi_in->buffer);
    plugin->descriptor->connect_port(plugin->handle, output_left->id, output_left->buffer);
    plugin->descriptor->connect_port(plugin->handle, output_right->id, output_right->buffer);

    lv2_atom_forge_set_buffer(&forge,
                              midi_in->buffer,
                              LV2_ATOM_BUFFER_SIZE);
    lv2_atom_forge_sequence_head(&forge, &midi_seq_frame, 0);
    
    init_midi_source(&forge);
    
    while (1) {

    DISABLE_IRQ();

        if (!buffer_processed) {
	  forge_midi_input();


            lv2_atom_forge_pop(&forge, &midi_seq_frame);

            plugin->descriptor->run(plugin->handle, LV2_AUDIO_BUFFER_SIZE);

            lv2_atom_forge_set_buffer(&forge,
                                      midi_in->buffer,
                                      sizeof(uint8_t) * midi_in->buffer_sz);
            lv2_atom_forge_sequence_head(&forge, &midi_seq_frame, 0);
            buffer_processed = 1;
        }

        if (buffer_processed && audio_buffer_free_space() > LV2_AUDIO_BUFFER_SIZE * 2) {
            audio_buffer_write(output_left->buffer, output_right->buffer, output_left->buffer_sz);
            buffer_processed = 0;
            counter++;
        }
	ENABLE_IRQ();
    }
}
