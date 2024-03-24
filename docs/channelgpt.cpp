#include <stdint.h>
#include <stdio.h>

// Annahme: Jeder Kanal wird mit 11 Bit dargestellt.
#define CHANNEL_BITS 11

void readCRSFChannels(const uint8_t* payload, uint16_t* channels, int numChannels) {
    // Sicherstellen, dass genügend Kanäle gelesen werden können
    if(numChannels > 12) numChannels = 12;

    // Initialisiere den Bit-Zähler
    int bitPos = 0;

    for(int channel = 0; channel < numChannels; ++channel) {
        // Extrahiere die Kanalwerte bitweise
        uint32_t value = 0;
        for(int bit = 0; bit < CHANNEL_BITS; ++bit) {
            int bytePos = (bitPos + bit) / 8;
            int bitInByte = (bitPos + bit) % 8;
            int bitValue = (payload[bytePos] >> bitInByte) & 1;
            value |= (uint32_t)bitValue << bit;
        }

        channels[channel] = value;

        // Update Bit-Position für den nächsten Kanal
        bitPos += CHANNEL_BITS;
    }
}

int main() {
    // Beispiel-CRSF-Telemetriedaten (Dies sind nur Dummy-Daten)
    uint8_t crsfPayload[] = {0xFF, 0xC0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint16_t channels[12];

    // Funktion aufrufen
    readCRSFChannels(crsfPayload, channels, 12);

    // Kanalwerte ausgeben
    for(int i = 0; i < 12; ++i) {
        printf("Kanal %d: %u\n", i + 1, channels[i]);
    }

    return 0;
}
