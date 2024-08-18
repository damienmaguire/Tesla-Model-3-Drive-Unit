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

// Pump temperatures seem to be offset by 40 degrees Celsius
// This makes sense given typical automotive temp ranges
static const int8_t TempOffset = 40;

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

static void PrintHeader()
{
    Serial.print("\r\n\nTime [ms], ");

    // PumpMessages::Command
    Serial.print("Speed_Request, ");

    // PumpMessages::Status1
    Serial.print("2A_Byte0, 2A_Byte1, 2A_Byte2, Fluid_Temp, ");
    Serial.print("2A_Byte4, Pump_Temp, 2A_Byte6, 2A_Byte7, ");

    // PumpMessages::Status2
    Serial.print("32_Byte0, 32_Byte1, 32_Byte2, 32_Byte3, ");
    Serial.print("32_Byte4, 32_Byte5, 32_Byte6, 32_Byte7, ");

    // PumpMessages::Status3
    Serial.print("Supply_Voltage, 30_Byte1, 30_Byte2, 30_Byte3, ");
    Serial.print("30_Byte4, 30_Byte5, 30_Byte6, 30_Byte7, ");

    // PumpMessages::Status4
    Serial.print("31_Byte0, Supply_Current, 31_Byte2, Temp3, ");
    Serial.print("Temp4, Temp5, Temp6, 31_Byte7");
    Serial.println();
}

static void PrintMessage(PumpMessages id, const MessageData& data)
{
    switch (id)
    {
    case PumpMessages::Command:
        Serial.print(data[1]);
        Serial.print(", ");
        break;

    case PumpMessages::Status1:
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.print(data[1]);
        Serial.print(", ");
        Serial.print(data[2]);
        Serial.print(", ");
        Serial.print(data[3] - TempOffset); // Fluid_Temp
        Serial.print(", ");
        Serial.print(data[4]);
        Serial.print(", ");
        Serial.print(data[5] - TempOffset); // Pump_Temp
        Serial.print(", ");
        Serial.print(data[6]);
        Serial.print(", ");
        Serial.print(data[7]);
        Serial.print(", ");
        break;

    case PumpMessages::Status2:
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.print(data[1]);
        Serial.print(", ");
        Serial.print(data[2]);
        Serial.print(", ");
        Serial.print(data[3]);
        Serial.print(", ");
        Serial.print(data[4]);
        Serial.print(", ");
        Serial.print(data[5]);
        Serial.print(", ");
        Serial.print(data[6]);
        Serial.print(", ");
        Serial.print(data[7]);
        Serial.print(", ");
        break;

    case PumpMessages::Status3:
        Serial.print(data[0] * 0.1); // Supply_Voltage
        Serial.print(", ");
        Serial.print(data[1]);
        Serial.print(", ");
        Serial.print(data[2]);
        Serial.print(", ");
        Serial.print(data[3]);
        Serial.print(", ");
        Serial.print(data[4]);
        Serial.print(", ");
        Serial.print(data[5]);
        Serial.print(", ");
        Serial.print(data[6]);
        Serial.print(", ");
        Serial.print(data[7]);
        Serial.print(", ");
        break;

    case PumpMessages::Status4:
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.print(data[1] * 0.1); // Supply_Current
        Serial.print(", ");
        Serial.print(data[2]);
        Serial.print(", ");
        Serial.print(data[3] - TempOffset); // Temp3
        Serial.print(", ");
        Serial.print(data[4] - TempOffset); // Temp4
        Serial.print(", ");
        Serial.print(data[5] - TempOffset); // Temp5
        Serial.print(", ");
        Serial.print(data[6] - TempOffset); // Temp6
        Serial.print(", ");
        Serial.print(data[7]);
        break;
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

    PrintHeader();
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

    Serial.print(millis());
    Serial.print(", ");

    for (auto id : AllPumpMessages)
    {
        LIN_Master::error_t error;
        MessageData         data;
        if (id == PumpMessages::Command)
        {
            // The first byte of the command is always 0xff in Tesla captures
            // changing it seems to do nothing
            data[0] = 0xff;
            data[1] = speed;
            error = LIN.sendMasterRequestBlocking(
                LIN_Master::LIN_V2, static_cast<uint8_t>(id), 2, data.data());

            if (error)
            {
                PrintError(error);
            }
            else
            {
                PrintMessage(id, data);
            }
        }
        else
        {
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
                PrintMessage(id, data);
            }
        }

        // indicate status via pin
        digitalWrite(PIN_ERROR, error);

        if (id == PumpMessages::Status4)
            Serial.println();

        // reset state machine & error
        LIN.resetStateMachine();
        LIN.resetError();
    }

    // wait a bit. Toggle pin to show CPU load
    uint32_t delayStart = millis();
    while (millis() - delayStart < LIN_PAUSE)
    {
        digitalWrite(PIN_TOGGLE, !digitalRead(PIN_TOGGLE));
    }
} // loop()
