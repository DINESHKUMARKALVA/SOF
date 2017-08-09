divert(-1)

define(`concat',`$1$2')

define(`STR', `"'$1`"')

dnl create direct DAPM/pipeline link between 2 widgets)
define(`dapm', `"$1, , $2"')

dnl SRC name)
define(`N_SRC', `SRC'PIPELINE_ID`.'$1)

dnl W_SRC(name, format, periods_sink, periods_source, data, preload)
define(`W_SRC',
`SectionVendorTuples."'N_SRC($1)`_tuples_w" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($3)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($4)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($5)
`	}'
`}'
`SectionData."'N_SRC($1)`_data_w" {'
`	tuples "'N_SRC($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_SRC($1)`_tuples_str" {'
`	tokens "sof_comp_tokens"'
`	tuples."string" {'
`		SOF_TKN_COMP_FORMAT'	STR($2)
`	}'
`}'
`SectionData."'N_SRC($1)`_data_str" {'
`	tuples "'N_SRC($1)`_tuples_str"'
`}'
`SectionWidget."'N_SRC($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "src"'
`	no_pm "true"'
`	data ['
`		"'N_SRC($1)`_data_w"'
`		"'N_SRC($1)`_data_str"'
`		"'$2`"'
`	]'
`}')

dnl Buffer name)
define(`N_BUFFER', `BUF'PIPELINE_ID`.'$1)

dnl W_BUFFER(name, size)
define(`W_BUFFER',
`SectionVendorTuples."'N_BUFFER($1)`_tuples" {'
`	tokens "sof_buffer_tokens"'
`	tuples."word" {'
`		SOF_TKN_BUF_SIZE'	STR($2)
`	}'
`}'
`SectionData."'N_BUFFER($1)`_data" {'
`	tuples "'N_BUFFER($1)`_tuples"'
`}'
`SectionWidget."'N_BUFFER($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "buffer"'
`	no_pm "true"'
`	data ['
`		"'N_BUFFER($1)`_data"'
`	]'
`}')

dnl PCM name)
define(`N_PCM', `PCM'PCM_ID)

dnl W_PCM_PLAYBACK(stream, dmac, dmac_chan, periods_sink, periods_source, preload)
dnl  PCM platform configuration
define(`W_PCM_PLAYBACK',
`SectionVendorTuples."'N_PCM($1)`_tuples_w_comp" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($4)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($5)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($6)
`	}'
`}'
`SectionData."'N_PCM($1)`_data_w_comp" {'
`	tuples "'N_PCM($1)`_tuples_w_comp"'
`}'
`SectionVendorTuples."'N_PCM($1)`_tuples" {'
`	tokens "sof_pcm_tokens"'
`	tuples."word" {'
`		SOF_TKN_PCM_DMAC'	STR($2)
`		SOF_TKN_PCM_DMAC_CHAN'	STR($3)
`	}'
`}'
`SectionData."'N_PCM($1)`_data" {'
`	tuples "'N_PCM($1)`_tuples"'
`}'
`SectionWidget."'N_PCM`" {'
`	index "'PIPELINE_ID`"'
`	type "aif_out"'
`	no_pm "true"'
`	stream_name "'$1`"'
`	data ['
`		"'N_PCM($1)`_data"'
`		"'N_PCM($1)`_data_w_comp"'
`	]'
`}')


dnl W_PCM_PLAYBACK(stream, dmac, dmac_chan, periods_sink, periods_source, preload)
define(`W_PCM_CAPTURE',
`SectionVendorTuples."'N_PCM($1)`_tuples_w_comp" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($4)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($5)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($6)
`	}'
`}'
`SectionData."'N_PCM($1)`_data_w_comp" {'
`	tuples "'N_PCM($1)`_tuples_w_comp"'
`}'
`SectionVendorTuples."'N_PCM($1)`_tuples" {'
`	tokens "sof_pcm_tokens"'
`	tuples."word" {'
`		SOF_TKN_PCM_DMAC'	STR($2)
`		SOF_TKN_PCM_DMAC_CHAN'	STR($3)
`	}'
`}'
`SectionData."'N_PCM($1)`_data" {'
`	tuples "'N_PCM($1)`_tuples"'
`}'
`SectionWidget."'N_PCM`" {'
`	index "'PIPELINE_ID`"'
`	type "aif_out"'
`	no_pm "true"'
`	stream_name "'$1`"'
`	data ['
`		"'N_PCM($1)`_data"'
`		"'N_PCM($1)`_data_w_comp"'
`	]'
`}')

dnl PGA name)
define(`N_PGA', `PGA'PIPELINE_ID`.'$1)

dnl W_PGA(name, format, periods_sink, periods_source, preload)
define(`W_PGA',
`SectionVendorTuples."'N_PGA($1)`_tuples_w" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($3)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($4)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($5)
`	}'
`}'
`SectionData."'N_PGA($1)`_data_w" {'
`	tuples "'N_PGA($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_PGA($1)`_tuples_str" {'
`	tokens "sof_comp_tokens"'
`	tuples."string" {'
`		SOF_TKN_COMP_FORMAT'	STR($2)
`	}'
`}'
`SectionData."'N_PGA($1)`_data_str" {'
`	tuples "'N_PGA($1)`_tuples_str"'
`}'
`SectionWidget."'N_PGA($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "pga"'
`	no_pm "true"'
`	data ['
`		"'N_PGA($1)`_data_w"'
`		"'N_PGA($1)`_data_str"'
`	]'
`}')

dnl Mixer Name)
define(`N_MIXER', `MIXER'PIPELINE_ID`.'$1)

dnl Pipe Buffer name in pipeline (pipeline, buffer)
define(`NPIPELINE_MIXER', `MIXER'$1`.'$2)

dnl W_MIXER(name, format, periods_sink, periods_source, preload)
define(`W_MIXER',
`SectionVendorTuples."'N_MIXER($1)`_tuples_w" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($3)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($4)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($5)
`	}'
`}'
`SectionData."'N_MIXER($1)`_data_w" {'
`	tuples "'N_MIXER($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_MIXER($1)`_tuples_str" {'
`	tokens "sof_comp_tokens"'
`	tuples."string" {'
`		SOF_TKN_COMP_FORMAT'	STR($2)
`	}'
`}'
`SectionData."'N_MIXER($1)`_data_str" {'
`	tuples "'N_MIXER($1)`_tuples_str"'
`}'
`SectionWidget."'N_MIXER($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "mixer"'
`	no_pm "true"'
`	data ['
`		"'N_MIXER($1)`_data_w"'
`		"'N_MIXER($1)`_data_str"'
`	]'
`}')


dnl Tone name)
define(`N_TONE', `TONE'PIPELINE_ID`.'$1)

dnl W_TONE(name, format, periods_sink, periods_source, preload)
define(`W_TONE',
`SectionVendorTuples."'N_TONE($1)`_tuples_w" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($3)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($4)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($5)
`	}'
`}'
`SectionData."'N_TONE($1)`_data_w" {'
`	tuples "'N_TONE($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_TONE($1)`_tuples_str" {'
`	tokens "sof_comp_tokens"'
`	tuples."string" {'
`		SOF_TKN_COMP_FORMAT'	STR($2)
`	}'
`}'
`SectionData."'N_TONE($1)`_data_str" {'
`	tuples "'N_TONE($1)`_tuples_str"'
`}'
`SectionWidget."'N_TONE($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "siggen"'
`	no_pm "true"'
`	data ['
`		"'N_TONE($1)`_data_w"'
`		"'N_TONE($1)`_data_str"'
`	]'
`}')

dnl DAI name)
define(`N_DAI', DAI_NAME)
define(`N_DAI_OUT', DAI_NAME`.OUT')
define(`N_DAI_IN', DAI_NAME`.IN')

dnl W_DAI_OUT(stream, type, index, data, periods_sink, periods_source, preload)
define(`W_DAI_OUT',
`SectionVendorTuples."'N_DAI_IN($1)`_tuples_w_comp" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($5)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($6)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($7)
`	}'
`}'
`SectionData."'N_DAI_IN($1)`_data_w_comp" {'
`	tuples "'N_DAI_IN($1)`_tuples_w_comp"'
`}'
`SectionVendorTuples."'N_DAI_OUT($1)`_tuples_w" {'
`	tokens "sof_dai_tokens"'
`	tuples."word" {'
`		SOF_TKN_DAI_INDEX'	$3
`	}'
`}'
`SectionData."'N_DAI_OUT($1)`_data_w" {'
`	tuples "'N_DAI_OUT($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_DAI_OUT($1)`_tuples_str" {'
`	tokens "sof_dai_tokens"'
`	tuples."string" {'
`		SOF_TKN_DAI_TYPE'	$2
`	}'
`}'
`SectionData."'N_DAI_OUT($1)`_data_str" {'
`	tuples "'N_DAI_OUT($1)`_tuples_str"'
`}'
`SectionWidget."'N_DAI_OUT`" {'
`	index "'PIPELINE_ID`"'
`	type "dai_in"'
`	no_pm "true"'
`	stream_name "'$1`"'
`	data ['
`		"'N_DAI_OUT($1)`_data_w"'
`		"'N_DAI_IN($1)`_data_w_comp"'
`		"'N_DAI_OUT($1)`_data_str"'
`		"'$4`"'
`	]'
`}')

dnl W_DAI_IN(stream, type, index, data, periods_sink, periods_source, preload)
define(`W_DAI_IN',
`SectionVendorTuples."'N_DAI_IN($1)`_tuples_w_comp" {'
`	tokens "sof_comp_tokens"'
`	tuples."word" {'
`		SOF_TKN_COMP_PERIOD_SINK_COUNT'		STR($5)
`		SOF_TKN_COMP_PERIOD_SOURCE_COUNT'	STR($6)
`		SOF_TKN_COMP_PRELOAD_COUNT'		STR($7)
`	}'
`}'
`SectionData."'N_DAI_IN($1)`_data_w_comp" {'
`	tuples "'N_DAI_IN($1)`_tuples_w_comp"'
`}'
`SectionVendorTuples."'N_DAI_IN($1)`_tuples_w" {'
`	tokens "sof_dai_tokens"'
`	tuples."word" {'
`		SOF_TKN_DAI_INDEX'	$3
`	}'
`}'
`SectionData."'N_DAI_IN($1)`_data_w" {'
`	tuples "'N_DAI_IN($1)`_tuples_w"'
`}'
`SectionVendorTuples."'N_DAI_IN($1)`_tuples_str" {'
`	tokens "sof_dai_tokens"'
`	tuples."string" {'
`		SOF_TKN_DAI_TYPE'	$2
`	}'
`}'
`SectionData."'N_DAI_IN($1)`_data_str" {'
`	tuples "'N_DAI_IN($1)`_tuples_str"'
`}'
`SectionWidget."'N_DAI_IN`" {'
`	index "'PIPELINE_ID`"'
`	type "dai_out"'
`	no_pm "true"'
`	stream_name "'$1`"'
`	data ['
`		"'N_DAI_IN($1)`_data_w"'
`		"'N_DAI_IN($1)`_data_w_comp"'
`		"'N_DAI_IN($1)`_data_str"'
`		"'$4`"'
`	]'
`}')

dnl Pipe Buffer name in pipeline (pipeline, buffer)
define(`NPIPELINE_BUFFER', `BUF'$1`.'$2)

dnl Pipeline name)
define(`N_PIPELINE', `PIPELINE.'PIPELINE_ID`.'$1)

dnl W_PIPELINE(stream, deadline, priority, frames, core, platform)
define(`W_PIPELINE',
`SectionVendorTuples."'N_PIPELINE($1)`_tuples" {'
`	tokens "sof_sched_tokens"'
`	tuples."word" {'
`		SOF_TKN_SCHED_DEADLINE'		STR($2)
`		SOF_TKN_SCHED_PRIORITY'		STR($3)
`		SOF_TKN_SCHED_CORE'		STR($5)
`		SOF_TKN_SCHED_FRAMES'		STR($4)
`	}'
`}'
`SectionData."'N_PIPELINE($1)`_data" {'
`	tuples "'N_PIPELINE($1)`_tuples"'
`}'
`SectionWidget."'N_PIPELINE($1)`" {'
`	index "'PIPELINE_ID`"'
`	type "scheduler"'
`	no_pm "true"'
`	stream_name "'$1`"'
`	data ['
`		"'N_PIPELINE($1)`_data"'
`		"'$6`"'
`	]'
`}')

dnl D_DAI(id, playback, capture, data))
define(`D_DAI', `SectionDAI."'N_DAI`" {'
`	index "'PIPELINE_ID`"'
`	id "'$1`"'
`	playback "'$2`"'
`	capture "'$3`"'
`}')

dnl DAI_CLOCK(clock, freq, codec_master)
define(`DAI_CLOCK',
	$1		STR($3)
	$1_freq	STR($2))


dnl DAI_TDM(slots, width, tx_mask, rx_mask)
define(`DAI_TDM',
`	tdm_slots	'STR($1)
`	tdm_slot_width	'STR($2)
`	tx_slots	'STR($3)
`	rx_slots	'STR($4)
)

dnl DAI_CONFIG(type, idx, name, sname, format, mclk, bclk, fsync, tdm)
define(`DAI_CONFIG',
`SectionHWConfig."'$1$2`" {'
`'
`	id		"'$2`"'
`	format		"'$5`"'
`'
`	'$6
`	'$7
`	'$8
`	'$9
`}'
`'
`SectionBE."'$3`" {'
`	index "0"'
`'
`	stream_name "'$4`"'
`	hw_configs ['
`		"'$1$2`"'
`	]'
`}')

dnl COMP_SAMPLE_SIZE(FMT)
define(`COMP_SAMPLE_SIZE',
`ifelse(
	$1, `s16le', `2',
	$1, `s24_4le', `4',
	$1, `s32le', `4',
	`4')')


dnl COMP_BUFFER_SIZE( num_periods, sample_size, channels, fmames)
define(`COMP_BUFFER_SIZE', `eval(`$1 * $2 * $3 * $4')')


divert(0) dnl
