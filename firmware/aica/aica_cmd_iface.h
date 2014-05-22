/* KallistiOS ##version##

   aica_cmd_iface.h
   (c)2000-2002 Dan Potter

   2011 SWAT

   Definitions for the SH-4/AICA interface. This file is meant to be
   included from both the ARM and SH-4 sides of the fence.
*/
         
#ifndef __ARM_AICA_CMD_IFACE_H
#define __ARM_AICA_CMD_IFACE_H

#ifndef __ARCH_TYPES_H
typedef unsigned long uint8;
typedef unsigned long uint32;
#endif

/* Command queue; one of these for passing data from the SH-4 to the
   AICA, and another for the other direction. If a command is written
   to the queue and it is longer than the amount of space between the
   head point and the queue size, the command will wrap around to
   the beginning (i.e., queue commands _can_ be split up). */
typedef struct aica_queue {
	uint32		head;		/* Insertion point offset (in bytes)	*/
	uint32		tail;		/* Removal point offset (in bytes)	*/
	uint32		size;		/* Queue size (in bytes)		*/
	uint32		valid;		/* 1 if the queue structs are valid	*/
	uint32		process_ok;	/* 1 if it's ok to process the data	*/
	uint32		data;		/* Pointer to queue data buffer		*/
} aica_queue_t;

/* Command queue struct for commanding the AICA from the SH-4 */
typedef struct aica_cmd {
	uint32		size;		/* Command data size in dwords				*/
	uint32		cmd;		/* Command ID						*/
	uint32		timestamp;	/* When to execute the command (0 == now)		*/
	uint32		cmd_id;		/* Command ID, for cmd/response pairs, or channel id	*/
	uint32		misc[4];	/* Misc Parameters / Padding				*/
	uint8		cmd_data[0];	/* Command data 					*/
} aica_cmd_t;

/* Maximum command size -- 256 dwords */
#define AICA_CMD_MAX_SIZE	256

/* This is the cmd_data for AICA_CMD_CHAN. Make this 16 dwords long
   for two aica bus queues. */
typedef struct aica_channel {
	uint32		cmd;		/* Command ID		*/
	uint32		base;		/* Sample base in RAM	*/
	uint32		type;		/* (8/16bit/ADPCM)	*/
	uint32		length;		/* Sample length	*/
	uint32		loop;		/* Sample looping	*/
	uint32		loopstart;	/* Sample loop start	*/
	uint32		loopend;	/* Sample loop end	*/
	uint32		freq;		/* Frequency		*/
	uint32		vol;		/* Volume 0-255		*/
	uint32		pan;		/* Pan 0-255		*/
	uint32		pos;		/* Sample playback pos	*/
	uint32		pad[5];		/* Padding		*/
} aica_channel_t;

typedef struct aica_decoder {
	uint32		cmd;
	uint32		codec;
	uint32		base;
	uint32		out;
	uint32		length;
	uint32		chan[2];
	uint32		pad[10];
} aica_decoder_t;

/* Declare an aica_cmd_t big enough to hold an aica_channel_t
   using temp name T, aica_cmd_t name CMDR, and aica_channel_t name CHANR */
#define AICA_CMDSTR_CHANNEL(T, CMDR, CHANR) \
	uint8	T[sizeof(aica_cmd_t) + sizeof(aica_channel_t)]; \
	aica_cmd_t	* CMDR = (aica_cmd_t *)T; \
	aica_channel_t	* CHANR = (aica_channel_t *)(CMDR->cmd_data);
#define AICA_CMDSTR_CHANNEL_SIZE	((sizeof(aica_cmd_t) + sizeof(aica_channel_t))/4)


#define AICA_CMDSTR_DECODER(T, CMDR, DECR) \
	uint8	T[sizeof(aica_cmd_t) + sizeof(aica_decoder_t)]; \
	aica_cmd_t	* CMDR = (aica_cmd_t *)T; \
	aica_decoder_t	* DECR = (aica_decoder_t *)(CMDR->cmd_data);
#define AICA_CMDSTR_DECODER_SIZE	((sizeof(aica_cmd_t) + sizeof(aica_decoder_t))/4)

/* Command values (for aica_cmd_t) */
#define AICA_CMD_NONE		0x00000000	/* No command (dummy packet)	*/
#define AICA_CMD_PING		0x00000001	/* Check for signs of life	*/
#define AICA_CMD_CHAN		0x00000002	/* Perform a wavetable action	*/
#define AICA_CMD_SYNC_CLOCK	0x00000003	/* Reset the millisecond clock	*/

#define AICA_CMD_CPU_CLOCK			0x00000004	/* */
#define AICA_CMD_DECODER			0x00000005

#define AICA_DECODER_CMD_NONE		0x00000000
#define AICA_DECODER_CMD_INIT		0x00000001
#define AICA_DECODER_CMD_SHUTDOWN	0x00000003
#define AICA_DECODER_CMD_DECODE		0x00000005


#define AICA_CODEC_MP3	0x00000001
#define AICA_CODEC_AAC	0x00000002

#define AICA_DECODER_CMD_MASK	0x0000000f

/* Response values (for aica_cmd_t) */
#define AICA_RESP_NONE		0x00000000
#define AICA_RESP_PONG		0x00000001	/* Response to CMD_PING 			*/
#define AICA_RESP_DBGPRINT	0x00000002	/* Entire payload is a null-terminated string	*/

/* Command values (for aica_channel_t commands) */
#define AICA_CH_CMD_MASK	0x0000000f

#define AICA_CH_CMD_NONE	0x00000000
#define AICA_CH_CMD_START	0x00000001
#define AICA_CH_CMD_STOP	0x00000002
#define AICA_CH_CMD_UPDATE	0x00000003

/* Start values */
#define AICA_CH_START_MASK	0x00300000

#define AICA_CH_START_DELAY	0x00100000 /* Set params, but delay key-on */
#define AICA_CH_START_SYNC	0x00200000 /* Set key-on for all selected channels */

/* Update values */
#define AICA_CH_UPDATE_MASK	0x000ff000

#define AICA_CH_UPDATE_SET_FREQ	0x00001000 /* frequency		*/
#define AICA_CH_UPDATE_SET_VOL	0x00002000 /* volume		*/
#define AICA_CH_UPDATE_SET_PAN	0x00004000 /* panning		*/

/* Sample types */
#define AICA_SM_8BIT	1
#define AICA_SM_16BIT	0
#define AICA_SM_ADPCM	2



/* This is where our SH-4/AICA comm variables go... */

/* 0x000000 - 0x040000 are reserved for the program */

/* Location of the SH-4 to AICA queue; commands from here will be
   periodically processed by the AICA and then removed from the queue. */
#define AICA_MEM_CMD_QUEUE	0x040000	/* 32K */

/* Location of the AICA to SH-4 queue; commands from here will be
   periodically processed by the SH-4 and then removed from the queue. */
#define AICA_MEM_RESP_QUEUE	0x048000	/* 32K */

/* This is the channel base, which holds status structs for all the
   channels. This is READ-ONLY from the SH-4 side. */
#define AICA_MEM_CHANNELS	0x050000	/* 64 * 16*4 = 4K */

/* The clock value (in milliseconds) */
#define AICA_MEM_CLOCK		0x051000	/* 4 bytes */

/* 0x051004 - 0x052000 are reserved for future expansion */

/* Open ram for sample data */
#define AICA_RAM_START		0x052000
#define AICA_RAM_END		0x200000



/* Quick access to the AICA channels */
#define AICA_CHANNEL(x)		(AICA_MEM_CHANNELS + (x) * sizeof(aica_channel_t))

#endif	/* __ARM_AICA_CMD_IFACE_H */