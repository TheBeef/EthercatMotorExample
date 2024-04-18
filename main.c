/*******************************************************************************
 * FILENAME: main.c
 *
 * PROJECT:
 *    EtherCatMotorExample
 *
 * FILE DESCRIPTION:
 *    This file has the example of controlling an EtherCat motor and using
 *    the SOEM library in it.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (31 Jul 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include <signal.h>
#include <sys/mman.h>
#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatmain.h"
#include "ethercatcoe.h"
#include "ethercatconfig.h"
#include "ethercatprint.h"
#include "wiznet_drv.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*** DEFINES                  ***/
#define SLAVEINDEX                  1   // What slave on the EtherCat chain are we going to talk to

#define REVOLUTION_PULSES               0x6500000
#define DEGREE_PULSES                   (REVOLUTION_PULSES/360)
#define START_ACCELERATION              0x96
#define STOP_DECELERATION               0x96

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
bool SOEM_Write8(uint16_t Index,uint8_t SubIndex,uint8_t Value);
bool SOEM_Write16(uint16_t Index,uint8_t SubIndex,uint16_t Value);
bool SOEM_Write32(uint16_t Index,uint8_t SubIndex,uint32_t Value);
uint8_t SOEM_Read8(uint16_t Index,uint8_t SubIndex);
uint16_t SOEM_Read16(uint16_t Index,uint8_t SubIndex);
uint32_t SOEM_Read32(uint16_t Index,uint8_t SubIndex);
void PrintError(const char *fmt,...);
bool InitECat(void);
void ShutdownECat(void);
void Wait(void);
void GotoPos(int pos);

/*** VARIABLE DEFINITIONS     ***/
char IOmap[4096];

int main(void)
{
    if(!InitECat())
    {
        PrintError("Failed to init ECat");
        return 1;
    }

    GotoPos(0);
    GotoPos(360);
    GotoPos(0);

    /******************************************************/
    /* Ok we are in a known state, start the testing here */
    /******************************************************/

    ShutdownECat();
    return 0;
}

/*******************************************************************************
 * NAME:
 *    PrintError
 *
 * SYNOPSIS:
 *    void PrintError(const char *fmt,...);
 *
 * PARAMETERS:
 *    fmt [I] -- The printf() formating string
 *    ... [I] -- printf() args
 *
 * FUNCTION:
 *    This function prints an error in red with the word ERROR: before it.
 *    It also adds a \n to the end of the line.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Outputs the stdout.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void PrintError(const char *fmt,...)
{
    va_list args;

    va_start(args,fmt);
    printf("\33[1;31mERROR:");  // Red error
    vprintf(fmt,args);
    printf("\33[m\n");          // Normal
    va_end(args);
}

/*******************************************************************************
 * NAME:
 *    SOEM_Write8
 *
 * SYNOPSIS:
 *    bool SOEM_Write8(uint16_t Index,uint8_t SubIndex,uint8_t Value);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to write to
 *    SubIndex [I] -- The sub index for this register
 *    Value [I] -- The value to write.
 *
 * FUNCTION:
 *    This function uses the ec_SDOwrite() to write to the EtherCat motor.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  Error was printed to the screen.
 *
 * SEE ALSO:
 *    SOEM_Write16(), SOEM_Write32()
 ******************************************************************************/
bool SOEM_Write8(uint16_t Index,uint8_t SubIndex,uint8_t Value)
{
    if(ec_SDOwrite(SLAVEINDEX,Index,SubIndex,false,sizeof(Value),&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Write8(0x%04X,0x%x)",Index,Value);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SOEM_Write16
 *
 * SYNOPSIS:
 *    bool SOEM_Write16(uint16_t Index,uint8_t SubIndex,uint8_t Value);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to write to
 *    SubIndex [I] -- The sub index for this register
 *    Value [I] -- The value to write.
 *
 * FUNCTION:
 *    This function uses the ec_SDOwrite() to write to the EtherCat motor.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  Error was printed to the screen.
 *
 * SEE ALSO:
 *    SOEM_Write8(), SOEM_Write32()
 ******************************************************************************/
bool SOEM_Write16(uint16_t Index,uint8_t SubIndex,uint16_t Value)
{
    if(ec_SDOwrite(SLAVEINDEX,Index,SubIndex,false,sizeof(Value),&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Write16(0x%04X,0x%x)",Index,Value);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SOEM_Write32
 *
 * SYNOPSIS:
 *    bool SOEM_Write32(uint16_t Index,uint8_t SubIndex,uint8_t Value);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to write to
 *    SubIndex [I] -- The sub index for this register
 *    Value [I] -- The value to write.
 *
 * FUNCTION:
 *    This function uses the ec_SDOwrite() to write to the EtherCat motor.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  Error was printed to the screen.
 *
 * SEE ALSO:
 *    SOEM_Write16(), SOEM_Write8()
 ******************************************************************************/
bool SOEM_Write32(uint16_t Index,uint8_t SubIndex,uint32_t Value)
{
    if(ec_SDOwrite(SLAVEINDEX,Index,SubIndex,false,sizeof(Value),&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Write32(0x%04X,0x%x)",Index,Value);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SOEM_Read8
 *
 * SYNOPSIS:
 *    uint8_t SOEM_Read8(uint16_t Index,uint8_t SubIndex);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to read from
 *    SubIndex [I] -- The sub index for this register
 *
 * FUNCTION:
 *    This function uses the ec_SDOread() to read from the motor.
 *
 * RETURNS:
 *    The value read or 0 if there was an error.
 *
 * SEE ALSO:
 *    SOEM_Read16(), SOEM_Read32()
 ******************************************************************************/
uint8_t SOEM_Read8(uint16_t Index,uint8_t SubIndex)
{
    uint8_t Value;
    int Size;

    Value=0;
    Size=sizeof(Value);
    if(ec_SDOread(SLAVEINDEX,Index,SubIndex,false,&Size,&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Read8(0x%04X,0x%x)",Index,SubIndex);
        return 0;
    }
    return Value;
}

/*******************************************************************************
 * NAME:
 *    SOEM_Read16
 *
 * SYNOPSIS:
 *    uint8_t SOEM_Read16(uint16_t Index,uint8_t SubIndex);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to read from
 *    SubIndex [I] -- The sub index for this register
 *
 * FUNCTION:
 *    This function uses the ec_SDOread() to read from the motor.
 *
 * RETURNS:
 *    The value read or 0 if there was an error.
 *
 * SEE ALSO:
 *    SOEM_Read8(), SOEM_Read32()
 ******************************************************************************/
uint16_t SOEM_Read16(uint16_t Index,uint8_t SubIndex)
{
    uint16_t Value;
    int Size;

    Value=0;
    Size=sizeof(Value);
    if(ec_SDOread(SLAVEINDEX,Index,SubIndex,false,&Size,&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Read16(0x%04X,0x%x)",Index,SubIndex);
        return 0;
    }
    return Value;
}

/*******************************************************************************
 * NAME:
 *    SOEM_Read32
 *
 * SYNOPSIS:
 *    uint8_t SOEM_Read32(uint16_t Index,uint8_t SubIndex);
 *
 * PARAMETERS:
 *    Index [I] -- The EtherCat register to read from
 *    SubIndex [I] -- The sub index for this register
 *
 * FUNCTION:
 *    This function uses the ec_SDOread() to read from the motor.
 *
 * RETURNS:
 *    The value read or 0 if there was an error.
 *
 * SEE ALSO:
 *    SOEM_Read16(), SOEM_Read8()
 ******************************************************************************/
uint32_t SOEM_Read32(uint16_t Index,uint8_t SubIndex)
{
    uint32_t Value;
    int Size;

    Value=0;
    Size=sizeof(Value);
    if(ec_SDOread(SLAVEINDEX,Index,SubIndex,false,&Size,&Value,
            EC_TIMEOUTRXM)==0)
    {
        PrintError("SOEM_Read32(0x%04X,0x%x)",Index,SubIndex);
        return 0;
    }
    return Value;
}

/*******************************************************************************
 * NAME:
 *    InitECat
 *
 * SYNOPSIS:
 *    bool InitECat(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function it's the Ethercat master (SOEM lib)
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool InitECat(void)
{
    int i;
    int oloop;
    int iloop;
    bool Working;

    wiznet_hw_config(8,1,1000000); //select SPI-W5500 parameters, before ec_init

    Working=false;
    if(ec_init("wiznet"))
    {
        /* find and auto-config slaves */
        if(ec_config_init(false)>0)
        {
            ec_config_map(&IOmap);

            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0,EC_STATE_SAFE_OP,EC_TIMEOUTSTATE*4);

            oloop=ec_slave[0].Obytes;
            if((oloop==0) && (ec_slave[0].Obits>0))
                oloop = 1;

            iloop=ec_slave[0].Ibytes;
            if((iloop==0) && (ec_slave[0].Ibits>0))
                iloop = 1;

            ec_slave[0].state = EC_STATE_OPERATIONAL;

            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);

            /* request OP state for all slaves */
            ec_writestate(0);
            ec_statecheck(0,EC_STATE_OPERATIONAL,EC_TIMEOUTSTATE); //wait for OP

            if(ec_slave[0].state==EC_STATE_OPERATIONAL)
            {
                Working=true;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i=1;i<=ec_slavecount;i++)
                {
                    if(ec_slave[i].state!=EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
        }
        else
        {
            printf("No slaves found!\n");
            Working=false;
        }
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n", "wiznet");
        return false;
    }

    return Working;
}

/*******************************************************************************
 * NAME:
 *    ShutdownECat
 *
 * SYNOPSIS:
 *    bool ShutdownECat(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shuts down the Ethercat init'ed with InitECat()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void ShutdownECat(void)
{
    /* stop SOEM, close socket */
    ec_slave[0].state = EC_STATE_SAFE_OP;
    /* request SAFE_OP state for all slaves */
    ec_writestate(0);
    /* wait for all slaves to reach state */
    ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);
    ec_slave[0].state = EC_STATE_PRE_OP;
    /* request SAFE_OP state for all slaves */
    ec_writestate(0);
    /* wait for all slaves to reach state */
    ec_statecheck(0, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE);

    ec_close();
}

/*******************************************************************************
 * NAME:
 *    Wait
 *
 * SYNOPSIS:
 *    void Wait(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function waits for the motor to get to the target.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Wait(void)
{
    int32_t Pos;
    uint16_t Status;

    for(;;)
    {
        Pos=SOEM_Read32(0x6064,0x00);
        Status=SOEM_Read16(0x6041,0x00);
        printf("POS:0x%08X,0x%04X\r",Pos,Status);
        fflush(stdout);

        if(!(Status & (1<<12)))
            break;
    }
    printf("\n");
}

/*******************************************************************************
 * NAME:
 *    GotoPos
 *
 * SYNOPSIS:
 *    void GotoPos(int pos);
 *
 * PARAMETERS:
 *    pos [I] -- The position to goto in deg.
 *
 * FUNCTION:
 *    This function tells the motor to goto a position, waits for it to get
 *    there.
 *
 * RETURNS:
 *    NONE
 *
 * LIMITATIONS:
 *    Resets the motor before it start it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void GotoPos(int pos)
{
    printf("GOTO %d\n",pos);

    SOEM_Write8(0x6060,0x00,1);          // Set mode
    SOEM_Write16(0x6040,0x00,0x0000);    // Clear bits
    SOEM_Write16(0x6040,0x00,0x0006);    // Shutdown
    SOEM_Write16(0x6040,0x00,0x0007);    // Switch on
    SOEM_Write16(0x6040,0x00,0x000F);    // Enable

    SOEM_Write32(0x6083,0x00,0x96);    // Set profile acceleration
    SOEM_Write32(0x6084,0x00,0x96);    // Set profile deceleration

    SOEM_Write32(0x6081,0x00,0x0200);    // Set provile velocity
    SOEM_Write32(0x607A,0x00,REVOLUTION_PULSES/360*pos);           // Set target

    SOEM_Write16(0x6040,0x00,0x000F);    // Mask control word to "enable"
    SOEM_Write16(0x6040,0x00,0x001F);    // Engage new set point (absolute)

    Wait();
}
