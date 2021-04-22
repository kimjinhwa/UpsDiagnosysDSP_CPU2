#include <CpuTimer1_2.h>
#include "driverlib.h"
#include "device.h"
#include "F2837xD_device.h"
#include "inc/hw_ipc.h"
#include "board.h"
#include "scistdio.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "SerialCommandUI.h"
#include "math.h"

#include "fatfs/src/ff.h"
#include "cmdline.h"
#include "ds1338z_Rtc.h"
// For Serial Command Line

#define RESULTS_BUFFER_SIZE     1024
#define RFFT_STAGES     9
#define RFFT_SIZE       (1 << RFFT_STAGES)

#pragma DATA_SECTION(RFFTin1Buff,"RFFTdata1")
uint16_t RFFTin1Buff[2*RFFT_SIZE];

#pragma DATA_SECTION(RFFTin1Buff_test,"RFFTdata1_test")
//uint16_t RFFTin1Buff_test[4096];
uint16_t RFFTin1Buff_test[1024];

#pragma DATA_SECTION(RFFTmagBuff,"RFFTdata2")
float RFFTmagBuff[RFFT_SIZE/2+1];
#pragma DATA_SECTION(RFFToutBuff,"RFFTdata3")
float RFFToutBuff[RFFT_SIZE];

#pragma DATA_SECTION(RFFTF32Coef,"RFFTdata4")

float RFFTF32Coef[RFFT_SIZE];
#pragma DATA_SECTION(adcAResults_1,"ADCBUFFER1")
#pragma DATA_SECTION(adcAResults_2,"ADCBUFFER1")
#pragma DATA_SECTION(adcAResults_3,"ADCBUFFER1")
#pragma DATA_SECTION(adcAResults_4,"ADCBUFFER1")

#pragma DATA_SECTION(adcAResults_5,"ADCBUFFER2")
#pragma DATA_SECTION(adcAResults_6,"ADCBUFFER2")
#pragma DATA_SECTION(adcAResults_7,"ADCBUFFER2")
#pragma DATA_SECTION(adcAResults_8,"ADCBUFFER2")

#pragma DATA_SECTION(adcAResults_9,"ADCBUFFER3")
#pragma DATA_SECTION(adcAResults_10,"ADCBUFFER3")
#pragma DATA_SECTION(adcAResults_11,"ADCBUFFER3")
#pragma DATA_SECTION(adcAResults_12,"ADCBUFFER3")

#pragma DATA_SECTION(adcAResults_13,"ADCBUFFER4")
#pragma DATA_SECTION(adcAResults_14,"ADCBUFFER4")
#pragma DATA_SECTION(adcAResults_15,"ADCBUFFER4")
#pragma DATA_SECTION(adcAResults_16,"ADCBUFFER4")

#pragma DATA_SECTION(adcAResults_17,"ADCBUFFER5")
#pragma DATA_SECTION(adcAResults_18,"ADCBUFFER5")
#pragma DATA_SECTION(adcAResults_19,"ADCBUFFER5")
#pragma DATA_SECTION(adcAResults_20,"ADCBUFFER5")
uint16_t adcAResults_1[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_2[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_3[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_4[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_5[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_6[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_7[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_8[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_9[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_10[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_11[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_12[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_13[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_14[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_15[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_16[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_17[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_18[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_19[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t adcAResults_20[RESULTS_BUFFER_SIZE];   // Buffer for results


struct st_fft_result {
    float maxValue;
    float freq;
    float THD ;
};

#pragma DATA_SECTION(fft_result,"GETBUFFER")
struct st_fft_result fft_result[20];

#pragma DATA_SECTION(time,"GETBUFFER")
struct rtctime_t time;

#pragma DATA_SECTION(request_fft,"GETBUFFER")
uint16_t request_fft;

#define CMD_BUF_SIZE    64
#define PATH_BUF_SIZE   40
#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

//128
#define X_SOH 0x01
//1024
#define X_STX 0x02
#define X_ACK 0x06
#define X_CAN  0x18
#define X_NAK 0x15
#define X_EOF 0x04
#define X_ETB 0x17
#define CRC_POLY 0x1021
#define min(a, b)       ((a) < (b) ? (a) : (b))

int Cmd_help(int argc, char *argv[]);
int Cmd_ls(int argc, char *argv[]);
int Cmd_test(int argc, char *argv[]);
int Cmd_cd(int argc, char *argv[]);
int Cmd_pwd(int argc, char *argv[]);
int Cmd_cat(int argc, char *argv[]);
int Cmd_del(int argc, char *argv[]);
int Cmd_get(int argc, char *argv[]);
int Cmd_cls(int argc, char *argv[]);
int Cmd_dump(int argc, char *argv[]);
int Cmd_fft(int argc, char *argv[]);
int Cmd_time(int argc, char *argv[]);
int memory_dump(unsigned long   startAddress,uint16_t mode);
void get_time();

volatile tState g_eState;
volatile tState g_eUIState;
static DIR g_sDirObject;
static char g_cCmdBuf[CMD_BUF_SIZE];
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";
static char g_cTmpBuf[PATH_BUF_SIZE];
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

static FILINFO g_sFileInfo;
static FIL g_sFileObject;

extern uint16_t cpuTimer1IntCount;
extern uint16_t cpuTimer2IntCount;

FATFS FatFs;

tCmdLineEntry g_psCmdTable[] =
{
    { "help",   Cmd_help,      " : Display list of commands" },
    { "h",      Cmd_help,   "    : alias for help" },
    { "?",      Cmd_help,   "    : alias for help" },
    { "ls",     Cmd_ls,      "   : Display list of files" },
    { "dir",     Cmd_ls,      "   : Display list of files" },
    { "chdir",  Cmd_cd,         ": Change directory" },
    { "cd",     Cmd_cd,      "   : alias for chdir" },
    { "pwd",    Cmd_pwd,      "  : Show current working directory" },
    { "cat",    Cmd_cat,      "  : Show contents of a text file" },
    { "del",    Cmd_del,      "  : delete or remove file" },
    { "get",    Cmd_get,      "  : get text file using xmodem" },
    { "test",    Cmd_test,      "  : make test text file" },
    { "cls",    Cmd_cls,      "  : clear screen" },
    { "dump",    Cmd_dump,      "  : Show Memory RFFTin1Buff" },
    { "fft",    Cmd_fft,      "  : Get FFT Data " },
    { "time",    Cmd_time,      "  : Show Now System Time" },
    { 0, 0, 0 }
};

void main(void)
{
    Device_init();
    Device_initGPIO();
	MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GS2,MEMCFG_GSRAMMASTER_CPU2);
	MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GS3,MEMCFG_GSRAMMASTER_CPU2);
	MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GS4,MEMCFG_GSRAMMASTER_CPU2);
	MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GS5,MEMCFG_GSRAMMASTER_CPU2);
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Board_init();
    initCPUTimers();
    EINT; ERTM;
    /*
    if(GPIO_readPin(SD_DETECT) ==0)
    {
       SCIprintf("\n\nSDC Card Inserted!\n")/
    }
    */


    SCIprintf("\n\nUSB Mass Storage Host program\n"); SCIprintf("Type \'help\' for help.\n\n");
    g_eState = STATE_DEVICE_READY;
    while(1){
        Loop_ReadLine();
        SCIprintf("\n\nSDC Card Inserted!\n");
    }
}

void Loop_ReadLine(void){

    int iStatus;
    if( f_mount(0,&FatFs)  == FR_OK) g_eState = STATE_DEVICE_READY;
    else g_eState = STATE_NO_DEVICE;

    while(1){
        ReadLine();
        if(g_cCmdBuf[0] == '\0')
        {
                    continue;
        }
        iStatus = CmdLineProcess(g_cCmdBuf);
        if(  GPIO_readPin(SD_DETECT) !=0 )
        {
           SCIprintf("SDC Card Not Inserted!\n");
        }
        if(iStatus == CMDLINE_BAD_CMD)
        {
           SCIprintf("Bad command!\n");
        }
        else if(iStatus == CMDLINE_TOO_MANY_ARGS)
        {
            SCIprintf("Too many arguments for command processor!\n");
        }
        else if(iStatus != 0)
         {
             SCIprintf("Command returned error code \n");
             //DESELECT ();
             if( f_mount(0,&FatFs)  == FR_OK) g_eState = STATE_DEVICE_READY;
             else g_eState = STATE_NO_DEVICE;
                        //StringFromFresult((FRESULT)iStatus));
         }

    }

}

void ReadLine(void)
{
    uint32_t ulIdx, ulPrompt;
    uint8_t ui8Char;
    tState eStateCopy;

    g_cCmdBuf[0] = '\0';
    ulIdx = 0;
    ulPrompt = 1;
    while(1){
        if(g_eState == STATE_DEVICE_ENUM) // See if a mass storage device has been enumerated.
        {
            //
            // Take it easy on the Mass storage device if it is slow to
            // start up after connecting.
            //
            //
            //if(USBHMSCDriveReady(g_psMSCInstance) != 0)
            //{
                //
                // Wait about 100ms before attempting to check if the
                // device is ready again.
                //
             //   SysCtl_delay(SysCtl_getClock(DEVICE_OSCSRC_FREQ)/30);

              //  break;
            //}
            ///

            //
            // Reset the working directory to the root.
            //
            g_cCwdBuf[0] = '/';
            g_cCwdBuf[1] = '\0';

            //
            // Attempt to open the directory.  Some drives take longer to
            // start up than others, and this may fail (even though the USB
            // device has enumerated) if it is still initializing.
            //
            f_mount(0, &FatFs);
            if(f_opendir(&g_sDirObject, g_cCwdBuf) == FR_OK)
            {
                //
                // The drive is fully ready, so move to that state.
                //
                g_eState = STATE_DEVICE_READY;
            }
        }

        //
        // See if the state has changed.  We make a copy of g_eUIState to
        // prevent a compiler warning about undefined order of volatile
        // accesses.
        //
        eStateCopy = g_eUIState;
        if(g_eState != eStateCopy)
        {
            //
            // Determine the new state.
            //
            switch(g_eState)
            {
                //
                // A previously connected device has been disconnected.
                //
            case STATE_NO_DEVICE:
            {
                if(g_eUIState == STATE_UNKNOWN_DEVICE)
                {
                    SCIprintf("\nUnknown device disconnected.\n");
                }
                else
                {
                    SCIprintf("\nMass storage device disconnected.\n");
                }
                ulPrompt = 1;
                break;
            }

            //
            // A mass storage device is being enumerated.
            //
            case STATE_DEVICE_ENUM:
            {
                break;
            }

            //
            // A mass storage device has been enumerated and initialized.
            //
            case STATE_DEVICE_READY:
            {
                SCIprintf("\nMass storage device connected.\n");
                ulPrompt = 1;
                break;
            }

            //
            // An unknown device has been connected.
            //
            case STATE_UNKNOWN_DEVICE:
            {
                SCIprintf("\nUnknown device connected.\n");
                ulPrompt = 1;
                break;
            }

            //
            // A power fault has occurred.
            //
            case STATE_POWER_FAULT:
            {
                SCIprintf("\nPower fault.\n");
                ulPrompt = 1;
                break;
            }
            }

            //
            // Save the current state.
            //
            g_eUIState = g_eState;
        }

        //
        // Print a prompt if necessary.
        //
        if(ulPrompt)
        {
            //
            // Print the prompt based on the current state.
            //
            if(g_eState == STATE_DEVICE_READY)
            {
                SCIprintf("%s> %s", g_cCwdBuf, g_cCmdBuf);
            }
            else if(g_eState == STATE_UNKNOWN_DEVICE)
            {
                SCIprintf("UNKNOWN> %s", g_cCmdBuf);
            }
            else
            {
                SCIprintf("NODEV> %s", g_cCmdBuf);
            }

            //
            // The prompt no longer needs to be printed.
            //
            ulPrompt = 0;
        }

        //
        // Loop while there are characters that have been received from the
        // SCI.
        //
        while(SCI_isDataAvailableNonFIFO(SCIC_BASE))
        {
            //
            // Read the next character from the SCI
            //
            ui8Char = SCI_readCharBlockingNonFIFO(SCIC_BASE);

            //
            // See if this character is a backspace and there is at least one
            // character in the input line.
            //
            //if((ui8Char == '\b') && (ulIdx != 0))
            if((ui8Char == 0x7F) && (ulIdx != 0))
            {
                //
                // Erase the last character from the input line.
                //
                SCIprintf("\b \b");
                ulIdx--;
                g_cCmdBuf[ulIdx] = '\0';
            }

            //
            // See if this character is a newline.
            //
            else if((ui8Char == '\r') || (ui8Char == '\n'))
            {
                //
                // Return to the caller.
                //
                SCIprintf("\n");
                return;
            }

            //
            // See if this character is an escape or Ctrl-U.
            //
            else if((ui8Char == 0x1b) || (ui8Char == 0x15))
            {
                //
                // Erase all characters in the input buffer.
                //
                while(ulIdx)
                {
                    SCIprintf("\b \b");
                    ulIdx--;
                }
                g_cCmdBuf[0] = '\0';
            }

            //
            // See if this is a printable ASCII character.
            //
            else if((ui8Char >= ' ') && (ui8Char <= '~') &&
                    (ulIdx < (sizeof(g_cCmdBuf) - 1)))
            {
                //
                // Add this character to the input buffer.
                //
                g_cCmdBuf[ulIdx++] = ui8Char;
                g_cCmdBuf[ulIdx] = '\0';
                SCIprintf("%c", (uint32_t)ui8Char);
            }
        }

        //
        // Run the main routine of the Host controller driver.
        //
        //USBHCDMain();
   }
}




int Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    SCIprintf("\nAvailable commands\n");
    SCIprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = &g_psCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The end of the
    // table has been reached when the command name is NULL.
    //
    while(pEntry->pcCmd)
    {
        //
        // Print the command name and the brief description.
        //
        SCIprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        pEntry++;
    }

    //
    // Return success.
    //
    return(0);
}
int Cmd_cls(int argc, char *argv[])
{
    SCI_writeCharBlockingNonFIFO(SCIC_BASE, 0x1b);
     SCIprintf("[2J");
    SCI_writeCharBlockingNonFIFO(SCIC_BASE, 0x1b);
     SCIprintf("[H");
     return 0;
}
void get_time()
{
    HWREG(IPC_BASE + IPC_O_SET) = IPC_SET_IPC21;  // 요청을 한다.
    while((HWREG(IPC_BASE + IPC_O_FLG) & IPC_STS_IPC21) == IPC_SET_IPC21) { };  // 요청된 처리가 완료 되기를 기다린다.
                                                                                // 나중에 타임아웃을 추가하기로 한다
}
int Cmd_time(int argc, char *argv[])
{
    BYTE buffer[30];
    uint16_t len;


    get_time();
    memset(buffer,0x00,sizeof(buffer));
   sprintf((char *)&buffer,"%d-%d-%d %d:%d:%d",2000+time.year,time.month,time.day,
           time.hour,time.minute,time.second);
   len =strlen((char *)buffer);
   SCIwrite((char *)buffer,len);
   SCIprintf("\r\n");
   return 0;
}

#define F_TO_D(x) sprintf((char *)&buffer,"%d.%03d\t",((uint16_t)(x*1000) / 1000) ,((uint16_t)(x*1000) % 1000)  )

int Cmd_fft(int argc, char *argv[])
{
    BYTE buffer[30];
    uint16_t len;
    memset(buffer,0x00,sizeof(buffer));
    int i=0;

    HWREG(IPC_BASE + IPC_O_SET) = IPC_SET_IPC22;  // 요청을 한다.
    while((HWREG(IPC_BASE + IPC_O_FLG) & IPC_STS_IPC22) == IPC_SET_IPC22) { };  // 요청된 처리가 완료 되기를 기다린다.
                                                                 // 따라서 1번데이타가 사용중이라고 설정되면 0번 데이타는 안전하게 사옹할 있다.
    for(i=0;i<20;i++)
    {
       //int nowPos=request_fft;

       sprintf((char *)&buffer,"fft_%d\t ",i);
       len =strlen((char *)buffer); SCIwrite((char *)buffer,len);

       F_TO_D(fft_result[i].freq);
       len =strlen((char *)buffer); SCIwrite((char *)buffer,len);
       SCIprintf("\t");

       F_TO_D(fft_result[i].THD);
       len =strlen((char *)buffer); SCIwrite((char *)buffer,len);

       F_TO_D(fft_result[i].maxValue);
       len =strlen((char *)buffer); SCIwrite((char *)buffer,len);

       SCIprintf("\r\n");
    }
       //while(request_fft == nowPos )DEVICE_DELAY_US(1);
    //fft_result
    //memory_dump((unsigned long)&RFFTin1Buff,1);
    //HWREG(IPC_BASE + IPC_O_SET) = IPC_SET_IPC0; // 데이타를 작업하겠다고 CPU1 알려 준다.
    //HWREG(IPC_BASE + IPC_O_CLR) = IPC_SET_IPC0; //클리어 시켜준다.
   return 0;
}
char * ftoa(float f, char * buf, int precision);
int memory_dump(unsigned long   startAddress,uint16_t mode){
   uint16_t i,j;

   BYTE buffer[40];
   uint16_t len;
   float value;
   memset(buffer,0x00,sizeof(buffer));
   //unsigned long  startAddress;
   //startAddress =(unsigned long)&RFFTin1Buff;
   //for(j=0;j<256;j++)
   for(j=0;j<64;j++)
   {
       //sprintf((char *)&buffer,"0x%04x%04x ",(startAddress +j*16) >> 16,(startAddress +j*16) & 0x0000ffff );
       sprintf((char *)&buffer,"0x%04x%04x\t",(uint16_t)((startAddress +j*16) >> 16 ),(uint16_t)((startAddress +j*16) & 0x0000ffff) );
       len =strlen((char *)buffer);
       SCIwrite((char *)buffer,len);

       for(i=0; i<16 ; i++)
       {
           memset(buffer,0x00,sizeof(buffer));
           //value =((3.0/4096.0* HWREGH(startAddress+16*j+i)) - 1.5);
           if(HWREGH(startAddress+16*j+i) >= 0x0821 )
           value =  (HWREGH(startAddress+16*j+i)-0x0821 )  ;
           else value =0;

           value = value * 3.0/4096.0 ;

           //value -= 1.5241699217f;
           //value *= 4.0;
           //value *= 4.0* 1000;
           //value *= 1000000;
           if(mode==0){
           sprintf((char *)&buffer,"%04x\t",HWREGH(startAddress+16*j+i));
           }
           else if(mode==1){
               ftoa(value,(char *)&buffer,3);
               strcat((char *)buffer,"\t");
           }
           len =strlen((char *)buffer);
           SCIwrite((char *)buffer,len);
           //SCIprintf("%x:",*(RFFTin1Buff+i));
       }
       SCIprintf("\r\n");
   }
     return 0;

}
int Cmd_dump(int argc, char *argv[])
{
    BYTE buffer[40];
    uint16_t displaymode=0;
    int16_t pos =0;
    //unsigned long address=(unsigned long)&RFFTin1Buff;
    if(argc <2 ){
         displaymode=0;
    }
    else if(argc <3){
        // display mode mode
        if(strcmp(argv[1],"-d")==0) displaymode= 1;
        else if(strcmp(argv[1],"-h")==0) displaymode= 0;
        else{
            sprintf((char *)buffer,(char *)"Parmeter error..\r\n");
            SCIwrite((char *)buffer,strlen((char *)buffer));
            sprintf((char *)buffer,(char *)"Usage: dump [-h] [-d] memory \r\n");
            SCIwrite((char *)buffer,strlen((char *)buffer));
            sprintf((char *)buffer,(char *)"memory : 0 to 20 \r\n");
            SCIwrite((char *)buffer,strlen((char *)buffer));
            return 0;
        }
    }
    else if(argc <4){
        if(strcmp(argv[1],"-d")==0) displaymode= 1;
        else if(strcmp(argv[1],"-h")==0) displaymode= 0;

        pos = atoi(argv[2]);
        if(pos <0 || pos > 19)
        {
            sprintf((char *)buffer,"Parmeter error..\r\n");
            SCIwrite((char *)buffer,strlen((char *)buffer));
            return 0;
        }
        //메모리를 복사해 온다.

        HWREG(IPC_BASE + IPC_O_SET) =  (1UL << (pos+1)); //IPC_SET_IPC21;  // 요청을 한다.
        while((HWREG(IPC_BASE + IPC_O_FLG) & (1UL << (pos+1)) ) == (1UL << (pos+1))) { };  // 요청된 처리가 완료 되기를 기다린다.

        int i;
        for(i=0;i < 1024;i++){
            HWREGH(RFFTin1Buff_test+i) = HWREGH(RFFTin1Buff +i);
        }
        memory_dump((unsigned long)&RFFTin1Buff_test,displaymode);
        //memory_dump((unsigned long)&RFFTin1Buff_test,1);
        /*
        switch(pos){
            case 0: address=(unsigned long)&adcAResults_1;break;
            case 1: address=(unsigned long)&adcAResults_2;break;
            case 2: address=(unsigned long)&adcAResults_3;break;
            case 3: address=(unsigned long)&adcAResults_4;break;
            case 4: address=(unsigned long)&adcAResults_5;break;
            case 5: address=(unsigned long)&adcAResults_6;break;
            case 6: address=(unsigned long)&adcAResults_7;break;
            case 7: address=(unsigned long)&adcAResults_8;break;
            case 8: address=(unsigned long)&adcAResults_9;break;
            case 9: address=(unsigned long)&adcAResults_10;break;
            case 10: address=(unsigned long)&adcAResults_11;break;
            case 11: address=(unsigned long)&adcAResults_12;break;
            case 12: address=(unsigned long)&adcAResults_13;break;
            case 13: address=(unsigned long)&adcAResults_14;break;
            case 14: address=(unsigned long)&adcAResults_15;break;
            case 15: address=(unsigned long)&adcAResults_16;break;
            case 16: address=(unsigned long)&adcAResults_17;break;
            case 17: address=(unsigned long)&adcAResults_18;break;
            case 18: address=(unsigned long)&adcAResults_19;break;
            case 19: address=(unsigned long)&adcAResults_20;break;
            default:address=(unsigned long)&adcAResults_1;break;
        }
        */
    }
    //메모리를 LOCK 한다.
    //메모리를 읽어도 되는 상황인지 즉 락이 걸려 있는지를 확인 한 후  락이 걸려 있지 않으면 메모리를 점유하여 읽는 동안 데이타에 쓰지 못하게 막는다.
    //while(!((HWREG(IPC_BASE + IPC_O_STS) & (IPC_STS_IPC0 << pos  )) == 0 )){ }
    //HWREG(IPC_BASE + IPC_O_SET) = (IPC_SET_IPC0 << pos) ; // 데이타를 작업하겠다고 CPU1 알려 준다.
    //memory_dump(address,displaymode);
    //HWREG(IPC_BASE + IPC_O_CLR) =  (IPC_SET_IPC0 << pos); //클리어 시켜준다.
    /*
   uint16_t i,j;

   BYTE buffer[10];
   uint16_t len;
   memset(buffer,0x00,sizeof(buffer));
   unsigned long  startAddress;
   startAddress =(unsigned long)&RFFTin1Buff;
   for(j=0;j<256;j++)
   {
       sprintf((char *)&buffer,"0x%08p ",(RFFTin1Buff+j*16));
       len =strlen((char *)buffer);
       SCIwrite((char *)buffer,len);

       for(i=0; i<16 ; i++)
       {
           sprintf((char *)&buffer,"%04x ",HWREGH(startAddress+16*j+i));
           len =strlen((char *)buffer);
           SCIwrite((char *)buffer,len);
           //SCIprintf("%x:",*(RFFTin1Buff+i));
       }
       SCIprintf("\r\n");
   }
     */
     return 0;
}

int Cmd_test(int argc, char *argv[])
{
    FIL fdst;
    FRESULT fr;
    WORD br, bw;
    BYTE buffer[10];
    int i;
    for(i=0;i<10;i++)buffer[i]=0x30+i;

    if( f_mount(0,&FatFs)  == FR_OK) g_eState = STATE_DEVICE_READY;
    else g_eState = STATE_NO_DEVICE;

     fr= f_open(&fdst, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
     if(fr == FR_OK)
     {
         br=10;
         f_write(&fdst, buffer, br, &bw);
         f_close(&fdst);
         SCIprintf("File Write OK  %d",bw);
         return 0;
     }
     SCIprintf("File Write Fail  %d",bw);
     return 0;
}
int Cmd_ls(int argc, char *argv[])
{
    uint32_t ui32TotalSize;
    uint32_t ui32FileCount;
    uint32_t ui32DirCount;
    FRESULT fresult;
    FATFS *pFatFs;

   //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Open the current directory for access.
    //
    fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    ui32TotalSize = 0;
    ui32FileCount = 0;
    ui32DirCount = 0;

    //
    // Enter loop to enumerate through all directory entries.
    //
    while(1)
    {
        //
        // Read an entry from the directory.
        //
        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            return(fresult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if(!g_sFileInfo.fname[0])
        {
            break;
        }

        //
        // If the attribute is directory, then increment the directory count.
        //
        if(g_sFileInfo.fattrib & AM_DIR)
        {
            ui32DirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else
        {
            ui32FileCount++;
            ui32TotalSize += g_sFileInfo.fsize;
        }

        //
        // Print the entry information on a single line with formatting to show
        // the attributes, date, time, size, and name.
        //
        SCIprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
                 (g_sFileInfo.fattrib & AM_DIR) ? (uint32_t)'D' : (uint32_t)'-',
                 (g_sFileInfo.fattrib & AM_RDO) ? (uint32_t)'R' : (uint32_t)'-',
                 (g_sFileInfo.fattrib & AM_HID) ? (uint32_t)'H' : (uint32_t)'-',
                 (g_sFileInfo.fattrib & AM_SYS) ? (uint32_t)'S' : (uint32_t)'-',
                 (g_sFileInfo.fattrib & AM_ARC) ? (uint32_t)'A' : (uint32_t)'-',
                 (uint32_t)((g_sFileInfo.fdate >> 9) + 1980),
                 (uint32_t)((g_sFileInfo.fdate >> 5) & 15),
                 (uint32_t)(g_sFileInfo.fdate & 31),
                 (uint32_t)((g_sFileInfo.ftime >> 11)),
                 (uint32_t)((g_sFileInfo.ftime >> 5) & 63),
                 (uint32_t)(g_sFileInfo.fsize),
                 g_sFileInfo.fname);
    }

    //
    // Print summary lines showing the file, dir, and size totals.
    //
    SCIprintf("\n%4u File(s),%10u bytes total\n%4u Dir(s)",
               ui32FileCount, ui32TotalSize, ui32DirCount);

    //
    // Get the free space.
    //
    fresult = f_getfree("/", &ui32TotalSize, &pFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    //
    // Display the amount of free space that was calculated.
    //
    SCIprintf(", %10uK bytes free\n", ui32TotalSize * pFatFs->sects_clust / 2);

    //
    // Made it to here, return with no errors.
    //
    return(0);
}

int Cmd_cd(int argc, char *argv[])
{
    unsigned int uIdx;
    FRESULT fresult;

    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Copy the current working path into a temporary buffer so it can be
    // manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If the first character is /, then this is a fully specified path, and it
    // should just be used as-is.
    //
    if(argv[1][0] == '/')
    {
        //
        // Make sure the new path is not bigger than the cwd buffer.
        //
        if(strlen(argv[1]) + 1 > sizeof(g_cCwdBuf))
        {
            SCIprintf("Resulting path name is too long\n");
            return(0);
        }

        //
        // If the new path name (in argv[1])  is not too long, then copy it
        // into the temporary buffer so it can be checked.
        //
        else
        {
            strncpy(g_cTmpBuf, argv[1], sizeof(g_cTmpBuf));
        }
    }

    //
    // If the argument is .. then attempt to remove the lowest level on the
    // CWD.
    //
    else if(!strcmp(argv[1], ".."))
    {
        //
        // Get the index to the last character in the current path.
        //
        uIdx = strlen(g_cTmpBuf) - 1;

        //
        // Back up from the end of the path name until a separator (/) is
        // found, or until we bump up to the start of the path.
        //
        while((g_cTmpBuf[uIdx] != '/') && (uIdx > 1))
        {
            //
            // Back up one character.
            //
            uIdx--;
        }

        //
        // Now we are either at the lowest level separator in the current path,
        // or at the beginning of the string (root).  So set the new end of
        // string here, effectively removing that last part of the path.
        //
        g_cTmpBuf[uIdx] = 0;
    }

    //
    // Otherwise this is just a normal path name from the current directory,
    // and it needs to be appended to the current path.
    //
    else
    {
        //
        // Test to make sure that when the new additional path is added on to
        // the current path, there is room in the buffer for the full new path.
        // It needs to include a new separator, and a trailing null character.
        //
        if(strlen(g_cTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cCwdBuf))
        {
            SCIprintf("Resulting path name is too long\n");
            return(0);
        }

        //
        // The new path is okay, so add the separator and then append the new
        // directory to the path.
        //
        else
        {
            //
            // If not already at the root level, then append a /
            //
            if(strcmp(g_cTmpBuf, "/"))
            {
                strcat(g_cTmpBuf, "/");
            }

            //
            // Append the new directory to the path.
            //
            strcat(g_cTmpBuf, argv[1]);
        }
    }

    //
    // At this point, a candidate new directory path is in chTmpBuf.  Try to
    // open it to make sure it is valid.
    //
    fresult = f_opendir(&g_sDirObject, g_cTmpBuf);

    //
    // If it can't be opened, then it is a bad path.  Inform user and return.
    //
    if(fresult != FR_OK)
    {
        SCIprintf("cd: %s\n", g_cTmpBuf);
        return(fresult);
    }

    //
    // Otherwise, it is a valid new path, so copy it into the CWD.
    //
    else
    {
        strncpy(g_cCwdBuf, g_cTmpBuf, sizeof(g_cCwdBuf));
    }

    //
    // Return success.
    //
    return(0);
}

int Cmd_pwd(int argc, char *argv[])
{
    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Print the CWD to the console.
    //
    SCIprintf("%s\n", g_cCwdBuf);

    //
    // Return success.
    //
    return(0);
};
typedef struct _xmodem_chunk {
        uint8_t start;
        uint8_t block;
        uint8_t block_neg;
        uint8_t payload[128];
        uint8_t crcH;
        uint8_t crcL;
        //uint16_t crc;
        //uint8_t dummy;
} xmodem_chunk ;
//__attribute__((packed));


static uint16_t crc_update(uint16_t crc_in, int incr)
{
        uint16_t xor = crc_in >> 15;
        uint16_t out = crc_in << 1;

        if (incr)
                out++;

        if (xor)
                out ^= CRC_POLY;

        return out;
}
static uint16_t crc16(const uint8_t *data, uint16_t size)
{
        uint16_t crc, i;

        for (crc = 0; size > 0; size--, data++)
                for (i = 0x80; i; i >>= 1)
                        crc = crc_update(crc, *data & i);

        for (i = 0; i < 16; i++)
                crc = crc_update(crc, 0);

        return crc;
}

int Cmd_get(int argc, char *argv[])
{
    FRESULT fresult;
    unsigned short usBytesRead;
    xmodem_chunk *chunk;

    uint8_t ui8Char;

    int retryCount=0;
    uint16_t next =1;
    chunk = (xmodem_chunk *)malloc (sizeof(xmodem_chunk ));
    if(chunk == NULL)
    {
        SCIprintf("Memory Allocation Error..\n");
        goto error;

    }
    if(g_eState != STATE_DEVICE_READY)
    {
        goto error;
    }
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        SCIprintf("Resulting path name is too long\n");
        goto error;
    }
    strcpy(g_cTmpBuf, g_cCwdBuf);
    if(strcmp("/", g_cCwdBuf))
    {
        strcat(g_cTmpBuf, "/");
    }
    strcat(g_cTmpBuf, argv[1]);
    fresult = f_open(&g_sFileObject, g_cTmpBuf, FA_READ);

    if(fresult != FR_OK)
    {
        goto error;
    }

    SCIprintf("Wating for receiver ping...\n");
    while(SCI_isDataAvailableNonFIFO(SCIC_BASE))
    ui8Char = SCI_readCharBlockingNonFIFO(SCIC_BASE);

    while(!SCI_isDataAvailableNonFIFO(SCIC_BASE))
    {
        ui8Char = SCI_readCharBlockingNonFIFO(SCIC_BASE);
        if(ui8Char == 'C')
        {
            break;
        }
    }
    chunk->block = 1;
    chunk->start = X_SOH;
    do
    {
        char status;
        if(next ==1 )
        {
            fresult = f_read(&g_sFileObject, chunk->payload, sizeof(chunk->payload) , &usBytesRead);
            if(fresult != FR_OK)
            {
                SCIprintf("File not found\n");
                goto error;
            }
        }
        if(usBytesRead==0)break;
        next =0;
        chunk->payload[usBytesRead] = 0; //g_cTmpBuf[usBytesRead] = 0;
        memset(chunk->payload + usBytesRead, 0xff, sizeof(chunk->payload) - usBytesRead); //memcpy(chunk->payload, g_cTmpBuf, usBytesRead);
        chunk->crcH = (crc16(chunk->payload, sizeof(chunk->payload)) >> 8) & 0xFF; //chunk->crc = swap16(crc16(chunk->payload, sizeof(chunk->payload)));
        chunk->crcL = (crc16(chunk->payload, sizeof(chunk->payload)) ) & 0xFF;
        ui8Char=sizeof(xmodem_chunk);

        chunk->block_neg = 0xff - chunk->block; //ui8Char=sizeof(xmodem_chunk); //SCIwrite((const char *)chunk, sizeof(xmodem_chunk));

        int uIdx;
        int uLen;
        uint16_t *c ;
        c = (uint16_t *)chunk ;
        uLen= sizeof(xmodem_chunk);
        for(uIdx = 0; uIdx <uLen; uIdx++)
        {
            SCI_writeCharBlockingNonFIFO(SCIC_BASE,*(c+uIdx));
        }


        ui8Char=0;
        cpuTimer1IntCount=100;
        while(cpuTimer1IntCount)
        while(SCI_isDataAvailableNonFIFO(SCIC_BASE) )
        {
            ui8Char = SCI_readCharBlockingNonFIFO(SCIC_BASE);
            cpuTimer1IntCount=0;
        }


        switch(ui8Char )
        {
        case X_NAK:
            retryCount++;
            status = 'N';
            break;
        case X_ACK:
            status = '.';
            retryCount=0;
            next = 1;
            break;
        default:
            status='?';
            break;
        }
        status=status;
        if(next)
        {
            chunk->block++;
        }
        if(retryCount>10)break;
    }
    while(usBytesRead == sizeof(chunk->payload) );
    SCI_writeCharBlockingNonFIFO(SCIC_BASE, X_EOF);

    ui8Char=0;
    cpuTimer1IntCount=100;
    while(cpuTimer1IntCount)
    while(SCI_isDataAvailableNonFIFO(SCIC_BASE) )
    {
        ui8Char = SCI_readCharBlockingNonFIFO(SCIC_BASE);
        cpuTimer1IntCount=0;
    }
    //SCI_writeCharBlockingNonFIFO(SCIC_BASE, X_ETB );
    SCI_writeCharBlockingNonFIFO(SCIC_BASE, X_EOF);
    if(retryCount>10) SCIprintf("\nError Occured...\n");
    else SCIprintf("\nFile Receive DONE\n");

error:
    free(chunk);
    return(0);
}

int Cmd_del(int argc, char *argv[])
{
    FRESULT fresult;

    if(g_eState != STATE_DEVICE_READY) return(FR_NOT_READY);
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        SCIprintf("Resulting path name is too long\n");
        return(0);
    }
    strcpy(g_cTmpBuf, g_cCwdBuf);
    if(strcmp("/", g_cCwdBuf)) strcat(g_cTmpBuf, "/");

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_cTmpBuf, argv[1]);

    fresult = f_unlink(g_cTmpBuf);
    if(fresult == FR_OK ) return 0;
    else return 1;
}
int Cmd_cat(int argc, char *argv[])
{
    FRESULT fresult;
    unsigned short usBytesRead;

    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        SCIprintf("Resulting path name is too long\n");
        return(0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if(strcmp("/", g_cCwdBuf))
    {
        strcat(g_cTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_cTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    fresult = f_open(&g_sFileObject, g_cTmpBuf, FA_READ);

    //
    // If there was some problem opening the file, then return an error.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    //
    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    //
    do
    {
        //
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        //
        fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1,
                         &usBytesRead);

        //
        // If there was an error reading, then print a newline and return the
        // error to the user.
        //
        if(fresult != FR_OK)
        {
            SCIprintf("\n");
            return(fresult);
        }

        //
        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        //
        g_cTmpBuf[usBytesRead] = 0;

        //
        // Print the last chunk of the file that was received.
        //
        SCIprintf("%s", g_cTmpBuf);

        //
        // Continue reading until less than the full number of bytes are read.
        // That means the end of the buffer was reached.
        //
    }
    while(usBytesRead == sizeof(g_cTmpBuf) - 1);

    //
    // Return success.
    //
    return(0);
}

