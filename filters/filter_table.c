#include "filter_table.h"
/* Automatically generated table. */

char * ftable_names[] = {
			"Reverb Filter", 
			"Tremolo Filter", 
			"Test Filter", 
			"Noise gate", 
			"Band Pass Filter", 
			"Flanging/Chorus Filter", 
			"Echo Filter", 
			"Parallel Filter", 
			"Gain Filter", 
			"Low Pass Filter", 
			"High Pass Filter", 
};

char * ftable_descs[] = {
			"Filter adding reverberation.", 
			"Filter adding tremolo/modulation.", 
			"Has absolutely no effect.", 
			"Noise gate filter.", 
			"Butterworth band pass.", 
			"Flanging or chorus effect.", 
			"Filter adding some echo.", 
			"Allows up to 4 filters to be mixed in parallel.", 
			"Simple linear gain filter.", 
			"Butterworth low pass.", 
			"Butterworth high pass.", 
}; 

char * ftable_params[] = {
			"[float](0.00:2.00)gain,[int](0:4095)delay", 
			"[float](0.00:0.50)depth,[int](0:1000)frequency", 
			"", 
			"[int](0:2048)threshold,[float](0:1)reduction", 
			"[int](0:20000)corner_low,[int](0:20000)corner_high,[int](0:128)order", 
			"[float](0.00:2.00)gain,[int](0:4095)delay,[int](0:1000)frequency,[int](0:2048)amplitude", 
			"[float](0.00:2.00)gain,[int](0:4095)delay", 
			"[float](0.00:2.00)gain0,[int](-1:128)filter0,[float](0.00:2.00)gain1,[int](-1:128)filter1,[float](0.00:2.00)gain2,[int](-1:128)filter2,[float](0.00:2.00)gain3,[int](-1:128)filter3 ", 
			"[float](0.00:2.00)gain", 
			"[int](0:20000)corner,[int](0:128)order", 
			"[int](0:20000)corner,[int](0:128)order", 
}; 

applyFilter ftable_applys[] = {
			&reverb_filter_apply, 
			&tremolo_filter_apply, 
			&test_filter_apply, 
			&ngate_filter_apply, 
			&bp_filter_apply, 
			&flanging_filter_apply, 
			&echo_filter_apply, 
			&parallel_filter_apply, 
			&gain_filter_apply, 
			&lp_filter_apply, 
			&hp_filter_apply, 
};

createFilter ftable_creators[] = {
			&reverb_filter_create, 
			&tremolo_filter_create, 
			&test_filter_create, 
			&ngate_filter_create, 
			&bp_filter_create, 
			&flanging_filter_create, 
			&echo_filter_create, 
			&parallel_filter_create, 
			&gain_filter_create, 
			&lp_filter_create, 
			&hp_filter_create, 
};