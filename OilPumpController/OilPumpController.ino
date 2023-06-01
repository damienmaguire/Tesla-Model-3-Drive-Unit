//
// Exercise the Tesla Model 3 oil pump over LIN
//
// Requires the LIN_master_portable_Arduino library to be installed
//
// Tested with a WEMOS LOLIN32 Lite connected to an MCP2003 LIN transceiver. The
// ~WAKE~ line is strapped to GND. RXD has 4K7 pull up to 3V3. LBUS has 1K
// pull-up to 12V via diode.
//

#include "LIN_master_HardwareSerial_ESP32.h"

// board pin definitions (GPIOn is referred to as n)
#define PIN_TOGGLE LED_BUILTIN // pin to demonstrate background operation
#define PIN_ERROR  33          // indicate LIN return status
#define PIN_LIN_RX 16          // receive pin for LIN
#define PIN_LIN_TX 17          // transmit pin for LIN
#define PIN_LIN_CS 23          // LIN transceiver chip select pin

// pause between LIN frames
#define LIN_PAUSE 100

// setup LIN node
LIN_Master_HardwareSerial_ESP32 LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, "LIN_HW");

// Tesla Oil Pump command and data request Ids in the order they appear in the
// captures
enum class PumpMessages : uint8_t
{
    Command = 0x0a,
    Status1 = 0x2a,
    Status2 = 0x32,
    Status3 = 0x30,
    Status4 = 0x31,
};

static const PumpMessages AllPumpMessages[] = { PumpMessages::Command,
                                                PumpMessages::Status1,
                                                PumpMessages::Status2,
                                                PumpMessages::Status3,
                                                PumpMessages::Status4 };

typedef std::array<uint8_t, 8> MessageData;

static void PrintError(LIN_Master::error_t error)
{
    if (error == LIN_Master::NO_ERROR)
        return;

    Serial.print("LIN Error: ");

    if (error & LIN_Master::ERROR_STATE)
        Serial.print("state ");

    if (error & LIN_Master::ERROR_ECHO)
        Serial.print("echo ");

    if (error & LIN_Master::ERROR_TIMEOUT)
        Serial.print("timeout ");

    if (error & LIN_Master::ERROR_CHK)
        Serial.print("checksum ");

    if (error & LIN_Master::ERROR_MISC)
        Serial.print("misc ");
}

static void PrintMessage(const MessageData& data)
{
    for (auto b : data)
    {
        Serial.printf("%#.2x ", b);
    }
}

// call once
void setup()
{
    // indicate background operation
    pinMode(PIN_TOGGLE, OUTPUT);

    // indicate LIN status via pin
    pinMode(PIN_ERROR, OUTPUT);

    // Turn on the transceiver
    pinMode(PIN_LIN_CS, OUTPUT);
    digitalWrite(PIN_LIN_CS, 1);

    // open LIN interface
    LIN.begin(19200);

    // for output (only) to console
    Serial.begin(115200);
    while (!Serial)
        ;

} // setup()

// call repeatedly
void loop()
{
    static uint8_t speed = 0x00;
    static String  inputBuffer;

    int newChar = Serial.read();
    while (newChar >= 0)
    {
        if (newChar == '\r' || newChar == '\n')
        {
            if (inputBuffer.length() > 0)
            {
                speed = constrain(inputBuffer.toInt(), 0, 255);
                inputBuffer.clear();
            }
        }
        else
        {
            inputBuffer += char(newChar);
        }

        newChar = Serial.read();
    }

    for (auto id : AllPumpMessages)
    {
        Serial.printf("%.8u ID: %#.2x ", millis(), id);
        LIN_Master::error_t error;
        if (id == PumpMessages::Command)
        {
            Serial.printf("Speed: %.3u ", speed);
            uint8_t Tx[2] = { 0xff, speed };
            error = LIN.sendMasterRequestBlocking(
                LIN_Master::LIN_V2, static_cast<uint8_t>(id), 2, Tx);
            PrintError(error);
        }
        else
        {
            MessageData data;
            error = LIN.receiveSlaveResponseBlocking(
                LIN_Master::LIN_V2,
                static_cast<uint8_t>(id),
                data.size(),
                data.data());

            if (error)
            {
                PrintError(error);
            }
            else
            {
                PrintMessage(data);
            }
        }

        // indicate status via pin
        digitalWrite(PIN_ERROR, error);

        Serial.println();

        // reset state machine & error
        LIN.resetStateMachine();
        LIN.resetError();

        // wait a bit. Toggle pin to show CPU load
        uint32_t delayStart = millis();
        while (millis() - delayStart < LIN_PAUSE)
        {
            digitalWrite(PIN_TOGGLE, !digitalRead(PIN_TOGGLE));
        }
    }
} // loop()
