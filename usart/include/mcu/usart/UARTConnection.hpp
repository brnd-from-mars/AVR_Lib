//
// Created by Brendan Berg on 14.02.19.
//

#ifndef ATAR_CENTRAL_UARTCONNECTION_HPP
#define ATAR_CENTRAL_UARTCONNECTION_HPP

#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>

#include <mcu/util/Buffer.hpp>

#include "ControlCharacters.hpp"
#include "UARTDataSource.hpp"
#include "UART.hpp"


#if defined(__AVR_ATmega328P__)
#define UART_NUM 1
extern "C" void USART_RX_vect(void) __attribute__ ((signal));
extern "C" void USART_TX_vect(void) __attribute__ ((signal));
#endif

#if defined(__AVR__ATmega1280__) || defined(__AVR_ATmega2560__)
#define UART_NUM 4
extern "C" void USART0_RX_vect(void) __attribute__ ((signal));
extern "C" void USART0_TX_vect(void) __attribute__ ((signal));
extern "C" void USART1_RX_vect(void) __attribute__ ((signal));
extern "C" void USART1_TX_vect(void) __attribute__ ((signal));
extern "C" void USART2_RX_vect(void) __attribute__ ((signal));
extern "C" void USART2_TX_vect(void) __attribute__ ((signal));
extern "C" void USART3_RX_vect(void) __attribute__ ((signal));
extern "C" void USART3_TX_vect(void) __attribute__ ((signal));
#endif


class UARTConnection
{

public:

    UARTConnection (uint8_t uartID, uint32_t baud);

    bool DataAvailable ();

    uint8_t ReadByte ();

    void WriteByte (uint8_t data);

    void WriteString (const char data[]);

    void WriteString (const char data[], uint8_t len);

    void SendData (UARTDataSource& dataSource);


private:

    UART m_UART;

    Buffer<128> m_TransmitBuffer;

    Buffer<128> m_ReceiveBuffer;

    bool m_OngoingTransmission;

    void StartTransmission ();

    void ReceiveCompleteInterruptServiceRoutine ();

    void TransmitCompleteInterruptServiceRoutine ();

    static UARTConnection* instancePtr[UART_NUM];


#if defined(__AVR_ATmega328P__)
    friend void USART_RX_vect();
    friend void USART_TX_vect();
#endif

#if defined(__AVR__ATmega1280__) || defined(__AVR_ATmega2560__)
    friend void USART0_RX_vect();
    friend void USART0_TX_vect();
    friend void USART1_RX_vect();
    friend void USART1_TX_vect();
    friend void USART2_RX_vect();
    friend void USART2_TX_vect();
    friend void USART3_RX_vect();
    friend void USART3_TX_vect();
#endif


};


#endif //ATAR_CENTRAL_UARTCONNECTION_HPP
