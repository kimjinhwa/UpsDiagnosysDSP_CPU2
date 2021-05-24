/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "board.h"

void Board_init()
{
	EALLOW;

	PinMux_init();
	GPIO_init();
	SCI_init();
	SPI_init();

	EDIS;
}

void PinMux_init()
{
	//GPIO57 -> SD_DETECT Pinmux
	GPIO_setPinConfig(GPIO_57_GPIO57);
	//GPIO61 -> SPI_CS Pinmux
	GPIO_setPinConfig(GPIO_61_GPIO61);
	//SCIC -> mySCI0 Pinmux
	GPIO_setPinConfig(GPIO_90_SCIRXDC);
	GPIO_setPinConfig(GPIO_89_SCITXDC);
	//SPIA -> mySPI1 Pinmux
	GPIO_setPinConfig(GPIO_58_SPISIMOA);
	GPIO_setPinConfig(GPIO_59_SPISOMIA);
	GPIO_setPinConfig(GPIO_60_SPICLKA);
	GPIO_setPinConfig(GPIO_19_SPISTEA);

}

void GPIO_init(){
		
	//SD_DETECT initialization
	GPIO_setDirectionMode(SD_DETECT, GPIO_DIR_MODE_IN);
	GPIO_setPadConfig(SD_DETECT, GPIO_PIN_TYPE_STD);
	GPIO_setMasterCore(SD_DETECT, GPIO_CORE_CPU2);
	GPIO_setQualificationMode(SD_DETECT, GPIO_QUAL_SYNC);
		
	//SPI_CS initialization
	GPIO_setDirectionMode(SPI_CS, GPIO_DIR_MODE_OUT);
	GPIO_setPadConfig(SPI_CS, GPIO_PIN_TYPE_STD);
	GPIO_setMasterCore(SPI_CS, GPIO_CORE_CPU2);
	GPIO_setQualificationMode(SPI_CS, GPIO_QUAL_SYNC);
}
void SCI_init(){
	
	//mySCI0 initialization
	SCI_clearInterruptStatus(mySCI0_BASE, SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_FE | SCI_INT_OE | SCI_INT_PE | SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
	SCI_clearOverflowStatus(mySCI0_BASE);

	SCI_disableFIFO(mySCI0_BASE);
	SCI_resetChannels(mySCI0_BASE);

	SCI_setConfig(mySCI0_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8|SCI_CONFIG_STOP_ONE|SCI_CONFIG_PAR_NONE));
	SCI_disableLoopback(mySCI0_BASE);
	SCI_performSoftwareReset(mySCI0_BASE);
	SCI_enableModule(mySCI0_BASE);
}
void SPI_init()
{
	
	//mySPI1 initialization
	SPI_disableModule(mySPI1_BASE);
	SPI_setConfig(mySPI1_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL1PHA0,
				  SPI_MODE_MASTER, 400000, 	8);
	SPI_disableFIFO(mySPI1_BASE);
	SPI_disableLoopback(mySPI1_BASE);
	SPI_setEmulationMode(mySPI1_BASE, SPI_EMULATION_FREE_RUN);
	SPI_enableModule(mySPI1_BASE);
}
