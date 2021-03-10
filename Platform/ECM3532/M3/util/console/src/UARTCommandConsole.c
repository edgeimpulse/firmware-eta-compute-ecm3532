/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"

#include "config.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "uart_hal.h"

/* Example includes. */
#include "FreeRTOS_CLI.h"
#include "UARTCommandConsole.h"

#define CLI_PORT CONFIG_DEBUG_UART /* uart port 0 */

/* A buffer into which command outputs can be written */
char cOutputBuffer[ configCOMMAND_INT_MAX_OUTPUT_SIZE ];

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* The maximum time in ticks to wait for the UART access mutex. */
#define cmdMAX_MUTEX_WAIT		( 200 / portTICK_PERIOD_MS )

/* Characters are only ever received slowly on the CLI so it is ok to pass
received characters from the UART interrupt to the task on a queue.  This sets
the length of the queue used for that purpose. */
#define cmdRXED_CHARS_QUEUE_LENGTH			( 10 )

/* DEL acts as a backspace. */
#define cmdASCII_DEL		( 0x7F )

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask( void *pvParameters );


/*
 * Register the 'standard' sample CLI commands with FreeRTOS+CLI.
 */
extern void vRegisterSampleCLICommands( void );

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static char * const pcWelcomeMessage = "\r\n\r\nFreeRTOS command server.\r\nType help to view a list of registered commands.\r\n\r\n"CONFIG_SOC"# ";
//static const char * const pcEndOfOutputMessage = "\r\nECM3531# ";
static const char * const pcEndOfOutputMessage = "\r\n"CONFIG_SOC"# ";
static const char * const pcNewLine = "\r\n";

/*-----------------------------------------------------------*/

__attribute__((section(".initSection")))
void vUARTCommandConsoleStart( uint16_t usStackSize, unsigned portBASE_TYPE uxPriority )
{
	vRegisterSampleCLICommands();

	/* Create that task that handles the console itself. */
	xTaskCreate( 	prvUARTCommandConsoleTask,			/* The task that implements the command console. */
					"CLI",								/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					usStackSize,						/* The size of the stack allocated to the task. */
					NULL,								/* The parameter is not used, so NULL is passed. */
					uxPriority,							/* The priority allocated to the task. */
					NULL );								/* A handle is not required, so just pass NULL. */
}
/*-----------------------------------------------------------*/

static void prvUARTCommandConsoleTask( void *pvParameters )
{
char cRxedChar, *pcOutputString;
uint8_t ucInputIndex = 0;
static char cInputString[ cmdMAX_INPUT_SIZE ], cLastInputString[ cmdMAX_INPUT_SIZE ];
portBASE_TYPE xReturned;

	( void ) pvParameters;

	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console
	interface will be used at any one time. */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	/* Send the welcome message. */
    HalUartWriteBuf(CLI_PORT, (uint8_t *)pcWelcomeMessage, strlen(pcWelcomeMessage), NULL, NULL);

	for( ;; )
	{
		/* Wait for the next character to arrive.  A semaphore is used to
		ensure no CPU time is used until data has arrived. */
        HalUartReadBuf(CLI_PORT, (uint8_t *)&cRxedChar, sizeof( cRxedChar ), NULL, NULL);
		{
			/* Echo the character back. */
			if(!((cRxedChar == '\b') || (cRxedChar == '\t')))
			    HalUartWriteBuf(CLI_PORT, (uint8_t *)&cRxedChar, sizeof( cRxedChar ), NULL, NULL);

			/* Was it the end of the line? */
			if( cRxedChar == '\n' || cRxedChar == '\r' )
			{
				/* Just to space the output from the input. */
				HalUartWriteBuf(CLI_PORT, (uint8_t *)pcNewLine, strlen( pcNewLine ), NULL, NULL);

				/* See if the command is empty, indicating that the last command is
				to be executed again. */
				if( ucInputIndex == 0 )
				{
					/* Copy the last command back into the input string. */
					strcpy( cInputString, cLastInputString );
				}

				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns pdFALSE
				(indicating there is no more output) as it might generate more than
				one string. */
				do
				{
					/* Get the next output string from the command interpreter. */
					xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

					/* Write the generated string to the UART. */
					HalUartWriteBuf(CLI_PORT, (uint8_t *)pcOutputString, strlen( pcOutputString ), NULL, NULL);

				} while( xReturned != pdFALSE );

				/* All the strings generated by the input command have been sent.
				Clear the input	string ready to receive the next command.  Remember
				the command that was just processed first in case it is to be
				processed again. */
				strcpy( cLastInputString, cInputString );
				ucInputIndex = 0;
				memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

				HalUartWriteBuf(CLI_PORT, (uint8_t *)pcEndOfOutputMessage, strlen( pcEndOfOutputMessage ), NULL, NULL);
			}
			else
			{
				if( cRxedChar == '\r' )
				{
					/* Ignore the character. */
				}
				else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
				{
					/* Backspace was pressed.  Erase the last character in the
					string - if any. */
					if( ucInputIndex > 0 )
					{
						ucInputIndex--;
						cInputString[ ucInputIndex ] = '\0';
						HalUartWriteBuf(CLI_PORT, (uint8_t *)&cRxedChar, sizeof( cRxedChar ), NULL, NULL);
						cRxedChar = ' ';
						HalUartWriteBuf(CLI_PORT, (uint8_t *)&cRxedChar, sizeof( cRxedChar ), NULL, NULL);
						cRxedChar = '\b';
						HalUartWriteBuf(CLI_PORT, (uint8_t *)&cRxedChar, sizeof( cRxedChar ), NULL, NULL);
					}
				}
				else
				{
					/* A character was entered.  Add it to the string
					entered so far.  When a \n is entered the complete
					string will be passed to the command interpreter. */
					if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
					{
						if( ucInputIndex < cmdMAX_INPUT_SIZE )
						{
							cInputString[ ucInputIndex ] = cRxedChar;
							ucInputIndex++;
						}
					}
				}
			}
		}
	}
}
