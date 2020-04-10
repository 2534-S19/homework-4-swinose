#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n"; // length is 48

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool goal_reached = false;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
    struct _eUSCI_eUSCI_UART_ConfigV1 uartConfig = {
         EUSCI_A_UART_CLOCKSOURCE_SMCLK,                    // 3MHz frequency
         19,                                                // UCBR = 19
         8,                                                 // UCBRF = 8
         0x55,                                              // UCBRS = 0x55
         EUSCI_A_UART_NO_PARITY,                            // No parity
         EUSCI_A_UART_LSB_FIRST,                            // LSB first
         EUSCI_A_UART_ONE_STOP_BIT,                         // One stop bit
         EUSCI_A_UART_MODE,                                 // UART mode
         EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,     // Oversampling
         EUSCI_A_UART_8_BIT_LEN
    };

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);  // Rx
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);  // Tx

    // TODO: Initialize EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    // TODO: Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
            rChar = UART_receiveData(EUSCI_A0_BASE);
        }
        else
        {
            rChar = 0xFF;
        }


        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        if (rChar != 0xFF)
        {
            goal_reached = charFSM(rChar);
            if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
            {
                UART_transmitData(EUSCI_A0_BASE, rChar);
            }
        }


        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if(goal_reached)
        {
            if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
            {
                int i;
                for (i = 0; i < 48; i++)
                {
                    UART_transmitData(EUSCI_A0_BASE, response[i]);
                }
                goal_reached = false;
            }
        }
    }

}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    bool finished = false;
    static unsigned int states = 0;

    switch (states%5)
    {
        case 0:
            if (rChar == '2')
            {
                states = 1;
            }
            break;

        case 1:
            if (rChar == '5')
            {
                states = 2;
            }
            else if (rChar == '2')
            {
                states = 1;
            }
            else
            {
                states = 0;
            }
            break;

        case 2:
            if (rChar == '3')
            {
                states = 3;
            }
            else if (rChar == '2')
            {
                states = 1;
            }
            else
            {
                states = 0;
            }
            break;

        case 3:
            if (rChar == '4')
            {
                states = 4;
                finished = true;
            }
            else if (rChar == '2')
            {
                states = 1;
            }
            else
            {
                states = 0;
            }
            break;

        case 4:
            if (rChar == '2')
            {
                states = 1;
            }
            else
            {
                states = 0;
            }
            break;
    }
    return finished;
}
