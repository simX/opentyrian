#ifndef FMOPL_H
#define FMOPL_H

#include "SDL_types.h"

/* --- select emulation chips --- */
#define BUILD_YM3812 1 /* (HAS_YM3812) */
#define BUILD_YM3526 (HAS_YM3526)
#define BUILD_Y8950  (HAS_Y8950)

/* select output bits size of output : 8 or 16 */
#define OPL_SAMPLE_BITS 16

/* compiler dependence */
#ifndef OSD_CPU_H
#define OSD_CPU_H
#endif

#if (OPL_SAMPLE_BITS == 16)
typedef Sint16 OPLSAMPLE;
#elif (OPL_SAMPLE_BITS == 8)
typedef Sint8 OPLSAMPLE;
#else
#error "Only 8-bit and 16-bit audio supported."
#endif


typedef void (*OPL_TIMERHANDLER)(int channel,float interval_Sec);
typedef void (*OPL_IRQHANDLER)(int param,int irq);
typedef void (*OPL_UPDATEHANDLER)(int param,int min_interval_us);
typedef void (*OPL_PORTHANDLER_W)(int param,unsigned char data);
typedef unsigned char (*OPL_PORTHANDLER_R)(int param);



struct OPL_SLOT
{
	Uint32	ar;			/* attack rate: AR<<2			*/
	Uint32	dr;			/* decay rate:  DR<<2			*/
	Uint32	rr;			/* release rate:RR<<2			*/
	Uint8	KSR;		/* key scale rate				*/
	Uint8	ksl;		/* keyscale level				*/
	Uint8	ksr;		/* key scale rate: kcode>>KSR	*/
	Uint8	mul;		/* multiple: mul_tab[ML]		*/

	/* Phase Generator */
	Uint32	Cnt;		/* frequency counter			*/
	Uint32	Incr;		/* frequency counter step		*/
	Uint8   FB;			/* feedback shift value			*/
	Sint32   *connect1;	/* slot1 output pointer			*/
	Sint32   op1_out[2];	/* slot1 output for feedback	*/
	Uint8   CON;		/* connection (algorithm) type	*/

	/* Envelope Generator */
	Uint8	eg_type;	/* percussive/non-percussive mode */
	Uint8	state;		/* phase type					*/
	Uint32	TL;			/* total level: TL << 2			*/
	Sint32	TLL;		/* adjusted now TL				*/
	Sint32	volume;		/* envelope counter				*/
	Uint32	sl;			/* sustain level: sl_tab[SL]	*/
	Uint8	eg_sh_ar;	/* (attack state)				*/
	Uint8	eg_sel_ar;	/* (attack state)				*/
	Uint8	eg_sh_dr;	/* (decay state)				*/
	Uint8	eg_sel_dr;	/* (decay state)				*/
	Uint8	eg_sh_rr;	/* (release state)				*/
	Uint8	eg_sel_rr;	/* (release state)				*/
	Uint32	key;		/* 0 = KEY OFF, >0 = KEY ON		*/

	/* LFO */
	Uint32	AMmask;		/* LFO Amplitude Modulation enable mask */
	Uint8	vib;		/* LFO Phase Modulation enable flag (active high)*/

	/* waveform select */
	unsigned int wavetable;
};

struct OPL_CH
{
	OPL_SLOT SLOT[2];
	/* phase generator state */
	Uint32  block_fnum;	/* block+fnum					*/
	Uint32  fc;			/* Freq. Increment base			*/
	Uint32  ksl_base;	/* KeyScaleLevel Base step		*/
	Uint8   kcode;		/* key code (for key scaling)	*/
};

/* OPL state */
struct FM_OPL {
	/* FM channel slots */
	OPL_CH	P_CH[9];				/* OPL/OPL2 chips have 9 channels*/

	Uint32	eg_cnt;					/* global envelope generator counter	*/
	Uint32	eg_timer;				/* global envelope generator counter works at frequency = chipclock/72 */
	Uint32	eg_timer_add;			/* step of eg_timer						*/
	Uint32	eg_timer_overflow;		/* envelope generator timer overlfows every 1 sample (on real chip) */

	Uint8	rhythm;					/* Rhythm mode					*/

	Uint32	fn_tab[1024];			/* fnumber->increment counter	*/

	/* LFO */
	Uint8	lfo_am_depth;
	Uint8	lfo_pm_depth_range;
	Uint32	lfo_am_cnt;
	Uint32	lfo_am_inc;
	Uint32	lfo_pm_cnt;
	Uint32	lfo_pm_inc;

	Uint32	noise_rng;				/* 23 bit noise shift register	*/
	Uint32	noise_p;				/* current noise 'phase'		*/
	Uint32	noise_f;				/* current noise period			*/

	Uint8	wavesel;				/* waveform select enable flag	*/

	int		T[2];					/* timer counters				*/
	int		TC[2];
	Uint8	st[2];					/* timer enable					*/

#if BUILD_Y8950
	/* Delta-T ADPCM unit (Y8950) */

	YM_DELTAT *deltat;

	/* Keyboard and I/O ports interface */
	Uint8	portDirection;
	Uint8	portLatch;
	OPL_PORTHANDLER_R porthandler_r;
	OPL_PORTHANDLER_W porthandler_w;
	int		port_param;
	OPL_PORTHANDLER_R keyboardhandler_r;
	OPL_PORTHANDLER_W keyboardhandler_w;
	int		keyboard_param;
#endif

	/* external event callback handlers */
	OPL_TIMERHANDLER  TimerHandler;	/* TIMER handler				*/
	int TimerParam;					/* TIMER parameter				*/
	OPL_IRQHANDLER    IRQHandler;	/* IRQ handler					*/
	int IRQParam;					/* IRQ parameter				*/
	OPL_UPDATEHANDLER UpdateHandler;/* stream update handler		*/
	int UpdateParam;				/* stream update parameter		*/

	Uint8 type;						/* chip type					*/
	Uint8 address;					/* address register				*/
	Uint8 status;					/* status flag					*/
	Uint8 statusmask;				/* status mask					*/
	Uint8 mode;						/* Reg.08 : CSM,notesel,etc.	*/

	int clock;						/* master clock  (Hz)			*/
	int rate;						/* sampling rate (Hz)			*/
	float freqbase;				/* frequency base				*/
	float TimerBase;				/* Timer base time (==sampling time)*/
};



#if BUILD_YM3812

int  YM3812Init(int num, int clock, int rate);
void YM3812Shutdown(void);
void YM3812ResetChip(int which);
int  YM3812Write(int which, int a, int v);
unsigned char YM3812Read(int which, int a);
int  YM3812TimerOver(int which, int c);
void YM3812UpdateOne(int which, OPLSAMPLE *buffer, int length);

void YM3812SetTimerHandler(int which, OPL_TIMERHANDLER TimerHandler, int channelOffset);
void YM3812SetIRQHandler(int which, OPL_IRQHANDLER IRQHandler, int param);
void YM3812SetUpdateHandler(int which, OPL_UPDATEHANDLER UpdateHandler, int param);

#endif


#if BUILD_YM3526

/*
** Initialize YM3526 emulator(s).
**
** 'num' is the number of virtual YM3526's to allocate
** 'clock' is the chip clock in Hz
** 'rate' is sampling rate
*/
int  YM3526Init(int num, int clock, int rate);
/* shutdown the YM3526 emulators*/
void YM3526Shutdown(void);
void YM3526ResetChip(int which);
int  YM3526Write(int which, int a, int v);
unsigned char YM3526Read(int which, int a);
int  YM3526TimerOver(int which, int c);
/*
** Generate samples for one of the YM3526's
**
** 'which' is the virtual YM3526 number
** '*buffer' is the output buffer pointer
** 'length' is the number of samples that should be generated
*/
void YM3526UpdateOne(int which, OPLSAMPLE *buffer, int length);

void YM3526SetTimerHandler(int which, OPL_TIMERHANDLER TimerHandler, int channelOffset);
void YM3526SetIRQHandler(int which, OPL_IRQHANDLER IRQHandler, int param);
void YM3526SetUpdateHandler(int which, OPL_UPDATEHANDLER UpdateHandler, int param);

#endif


#if BUILD_Y8950

#include "ymdeltat.h"

/* Y8950 port handlers */
void Y8950SetPortHandler(int which, OPL_PORTHANDLER_W PortHandler_w, OPL_PORTHANDLER_R PortHandler_r, int param);
void Y8950SetKeyboardHandler(int which, OPL_PORTHANDLER_W KeyboardHandler_w, OPL_PORTHANDLER_R KeyboardHandler_r, int param);
void Y8950SetDeltaTMemory(int which, void * deltat_mem_ptr, int deltat_mem_size );

int  Y8950Init (int num, int clock, int rate);
void Y8950Shutdown (void);
void Y8950ResetChip (int which);
int  Y8950Write (int which, int a, int v);
unsigned char Y8950Read (int which, int a);
int  Y8950TimerOver (int which, int c);
void Y8950UpdateOne (int which, Sint16 *buffer, int length);

void Y8950SetTimerHandler (int which, OPL_TIMERHANDLER TimerHandler, int channelOffset);
void Y8950SetIRQHandler (int which, OPL_IRQHANDLER IRQHandler, int param);
void Y8950SetUpdateHandler (int which, OPL_UPDATEHANDLER UpdateHandler, int param);

#endif


int limit( int val, int max, int min );
void OPL_STATUS_SET(FM_OPL *OPL,int flag);
void OPL_STATUS_RESET(FM_OPL *OPL,int flag);
void OPL_STATUSMASK_SET(FM_OPL *OPL,int flag);
void advance_lfo(FM_OPL *OPL);
void advance(FM_OPL *OPL);
signed int op_calc(Uint32 phase, unsigned int env, signed int pm, unsigned int wave_tab);
signed int op_calc1(Uint32 phase, unsigned int env, signed int pm, unsigned int wave_tab);
void OPL_CALC_CH( OPL_CH *CH );
void OPL_CALC_RH( OPL_CH *CH, unsigned int noise );
void FM_KEYON(OPL_SLOT *SLOT, Uint32 key_set);
void FM_KEYOFF(OPL_SLOT *SLOT, Uint32 key_clr);
void CALC_FCSLOT(OPL_CH *CH,OPL_SLOT *SLOT);
void set_mul(FM_OPL *OPL,int slot,int v);
void set_ksl_tl(FM_OPL *OPL,int slot,int v);
void set_ar_dr(FM_OPL *OPL,int slot,int v);
void set_sl_rr(FM_OPL *OPL,int slot,int v);
void CSMKeyControll(OPL_CH *CH);


#endif /* FMOPL_H */
