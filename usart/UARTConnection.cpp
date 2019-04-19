//
// Created by Brendan Berg on 14.02.19.
//

#include <mcu/usart/UARTConnection.hpp>


UARTConnection* UARTConnection::instancePtr[UART_NUM] = {nullptr};


UARTConnection::UARTConnection (uint8_t uartID, uint32_t baud)
    : m_OngoingTransmission(false)
{
    UARTProtocolConfig uartPrtclCfg = {};
    uartPrtclCfg.cpuFreq       = 16000000;
    uartPrtclCfg.baudFreq      = baud;
    uartPrtclCfg.characterSize = UARTProtocolConfig::characterSize8;
    uartPrtclCfg.parityBit     = UARTProtocolConfig::parityBitOdd;
    uartPrtclCfg.stopBits      = UARTProtocolConfig::stopBits1;
    uartPrtclCfg.doubleSpeedOp = UARTProtocolConfig::doubleSpeedOpDisabled;

    m_UART = UART(uartPrtclCfg, uartID);

    m_UART.EnableReceiveCompleteInterrupt();
    m_UART.EnableTransmitCompleteInterrupt();

    UARTConnection::instancePtr[uartID] = this;
}


bool UARTConnection::DataAvailable ()
{
    return !m_ReceiveBuffer.IsEmpty();
}


uint8_t UARTConnection::ReadByte ()
{
    auto data = m_ReceiveBuffer.Read();
    m_ReceiveBuffer.Pop();
    return data;
}


void UARTConnection::WriteByte (uint8_t data)
{
    m_TransmitBuffer.Push(data);
    if (!m_OngoingTransmission)
    {
        m_OngoingTransmission = true;
        StartTransmission();
    }
}


void UARTConnection::WriteString (const char data[])
{
    auto len = strlen(data);
    for (int i = 0; i < len; ++i)
    {
        WriteByte(static_cast<uint8_t>(data[i]));
    }
}


void UARTConnection::StartTransmission ()
{
    TransmitCompleteInterruptServiceRoutine();
}


void UARTConnection::ReceiveCompleteInterruptServiceRoutine ()
{
    auto data = static_cast<uint8_t>(m_UART.Receive());
    m_ReceiveBuffer.Push(data);
}


void UARTConnection::TransmitCompleteInterruptServiceRoutine ()
{
    if (m_TransmitBuffer.IsEmpty())
    {
        m_OngoingTransmission = false;
        return;
    }

    if (!m_UART.DataRegisterEmpty())
    {
        return;
    }

    auto data = m_TransmitBuffer.Read();
    m_TransmitBuffer.Pop();
    m_UART.Transmit(data);
}


#define RCISR(vect, i) ISR (vect)\
{\
    UARTConnection::instancePtr[0]->ReceiveCompleteInterruptServiceRoutine();\
}

#define TCISR(vect, i) ISR (vect)\
{\
    UARTConnection::instancePtr[0]->TransmitCompleteInterruptServiceRoutine();\
}

#if defined(__AVR_ATmega328P__)
RCISR(USART_RX_vect, 0)
TCISR(USART_TX_vect, 0)
#endif

#if defined(__AVR__ATmega1280__) || defined(__AVR_ATmega2560__)
RCISR(USART0_RX_vect, 0)
TCISR(USART0_TX_vect, 0)
RCISR(USART1_RX_vect, 1)
TCISR(USART1_TX_vect, 1)
RCISR(USART2_RX_vect, 2)
TCISR(USART2_TX_vect, 2)
RCISR(USART3_RX_vect, 3)
TCISR(USART3_TX_vect, 3)
#endif
