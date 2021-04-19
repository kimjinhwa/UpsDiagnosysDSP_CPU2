
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN           	: origin = 0x000000, length = 0x000002
   RAMM0           	: origin = 0x0000A2, length = 0x00035E
   RAMD0           	: origin = 0x00B000, length = 0x000800
   RAMLS_01234          : origin = 0x008000, length = 0x002800
   RAMGS2      : origin = 0x00E000, length = 0x001000
   RAMGS5      : origin = 0x011000, length = 0x001000
   //RAMLS0          : origin = 0x008000, length = 0x000800
   //RAMLS1          	: origin = 0x008800, length = 0x000800
   //RAMLS2      		: origin = 0x009000, length = 0x000800
   //RAMLS3      		: origin = 0x009800, length = 0x000800
   //RAMLS4      		: origin = 0x00A000, length = 0x000800
   RESET           : origin = 0x3FFFC0, length = 0x000002

   //RAMGS2to6_combined : origin = 0x00E000, length = 0x006000
PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x0000A0     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x0003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS5      : origin = 0x00A800, length = 0x000800

// CPU2 2 3 4 5
   RAMGS3      : origin = 0x00F000, length = 0x001000
   RAMGS4      : origin = 0x010000, length = 0x001000

   RAMGS6      : origin = 0x012000, length = 0x001000
   RAMGS7      : origin = 0x013000, length = 0x001000
   RAMGS8      : origin = 0x014000, length = 0x001000
   RAMGS9      : origin = 0x015000, length = 0x001000
   RAMGS10     : origin = 0x016000, length = 0x001000
   RAMGS11     : origin = 0x017000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS12     : origin = 0x018000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS13     : origin = 0x019000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS14     : origin = 0x01A000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS15     : origin = 0x01B000, length = 0x000FF8     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */

   CANA_MSG_RAM     : origin = 0x049000, length = 0x000800
   CANB_MSG_RAM     : origin = 0x04B000, length = 0x000800

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : >>   RAMLS_01234 | RAMGS2	| RAMGS5       ,   PAGE = 0
   //.text            : >> RAMD0 |  RAMLS_01234 | 	RAMGS5       ,   PAGE = 0
   //.text            : >> RAMD0 |  RAMGS2to6_combined ,   PAGE = 0
   .cinit           : > RAMM0,     PAGE = 0
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
   .stack           : > RAMM1,     PAGE = 1
// RAMLS5-> RAMGS2
// RAMLS3-> RAMGS3
#if defined(__TI_EABI__)
   .bss             : > RAMGS2,    PAGE = 0
   .bss:output      : > RAMGS3,    PAGE = 1
   .init_array      : > RAMGS4,     PAGE = 1
   .const           : > RAMGS2,    PAGE = 0
   .data            : > RAMGS4,    PAGE = 1
   .sysmem          : > RAMGS3,    PAGE = 1
   //.bss             : > RAMLS5,    PAGE = 1
   //.bss:output      : > RAMLS3,    PAGE = 0
   //.init_array      : > RAMM0,     PAGE = 0
   //.const           : > RAMLS5,    PAGE = 1
   //.data            : > RAMLS5,    PAGE = 1
   //.sysmem          : > RAMLS5,    PAGE = 1
#else
   .pinit           : > RAMM0,     PAGE = 0
   .ebss            : > RAMLS5,    PAGE = 1
   .econst          : > RAMLS5,    PAGE = 1
   .esysmem         : > RAMLS5,    PAGE = 1
#endif

#ifdef __TI_COMPILER_VERSION__
   #if __TI_COMPILER_VERSION__ >= 15009000
    .TI.ramfunc : {} > RAMM0,      PAGE = 0
   #else
   ramfuncs         : > RAMM0      PAGE = 0
   #endif
#endif

   RFFTdata1_test   : > RAMGS7 ,   PAGE = 1
   RFFTdata1        : > RAMGS8 ,   PAGE = 1
   RFFTdata2        : > RAMGS9 ,   PAGE = 1
   RFFTdata3        : > RAMGS9 ,   PAGE = 1
   RFFTdata4        : > RAMGS9 ,   PAGE = 1
   ADCBUFFER1		: > RAMGS10,   PAGE = 1
   ADCBUFFER2		: > RAMGS11,   PAGE = 1
   ADCBUFFER3		: > RAMGS12,   PAGE = 1
   ADCBUFFER4		: > RAMGS13,   PAGE = 1
   ADCBUFFER5		: > RAMGS14,   PAGE = 1
   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }
}

