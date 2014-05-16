#include <stdlib.h>
#include <stdio.h>
#include "RF24.h"

typedef struct result {
    uint8_t pass;
    uint8_t fail;
} Result;


uint8_t assert(int check, int value, Result *r){
    if (check == value){
        r->pass++;
        printf("\tPASS\n");
        return 1;
    }
    else{
        r->fail++;
        printf("\tFAIL\n");
        return 0;
    }
}

void test_data_rate(Result *r){
    printf("[TEST SUITE] Performing Data rate tests...\n");
    printf("[TEST]\tSetting data rate to 250Kbps...");
    rf24_setDataRate(RF24_250KBPS);
    printf("\t\t> Model: %s\n", 
        (assert(RF24_250KBPS, rf24_getDataRate(), r) ? "nRF24L01+" : "nRF24L01"));
    printf("[TEST]\tSetting data rate to 1Mbps...");
    rf24_setDataRate(RF24_1MBPS);
    assert(RF24_1MBPS, rf24_getDataRate(), r);
    printf("[TEST]\tSetting data rate to 2Mbps...");
    rf24_setDataRate(RF24_2MBPS);
    assert(RF24_2MBPS, rf24_getDataRate(), r);
}

void test_power_level(Result *r){
    printf("[TEST SUITE] Performing power level tests...\n");
    printf("[TEST]\tSetting power level to min...");
    rf24_setPALevel(RF24_PA_MIN);
    assert(RF24_PA_MIN, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to low...");
    rf24_setPALevel(RF24_PA_LOW);
    assert(RF24_PA_LOW, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to high...");
    rf24_setPALevel(RF24_PA_HIGH);
    assert(RF24_PA_HIGH, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to max...");
    rf24_setPALevel(RF24_PA_MAX);
    assert(RF24_PA_MAX, rf24_getPALevel(), r);
}

void test_crc_length(Result *r){
    printf("[TEST SUITE] Performing crc tests...\n");
    printf("[TEST]\tDisabling CRC...");
    rf24_setCRCLength(RF24_CRC_DISABLED);
    assert(RF24_CRC_DISABLED, rf24_getCRCLength(), r);
    printf("[TEST]\tSetting CRC length to 8bits...");
    rf24_setCRCLength(RF24_CRC_8);
    assert(RF24_CRC_8, rf24_getCRCLength(), r);
    printf("[TEST]\tSetting CRC length to 16bits...");
    rf24_setCRCLength(RF24_CRC_16);
    assert(RF24_CRC_16, rf24_getCRCLength(), r);
}

void run_test_suite(Result *r){
    printf("\n***********************************\n");
    printf("      nRF24L01(+) test suite\n\n");
    test_data_rate(r);
    test_power_level(r);
    printf("***********************************\n");
    printf("           Test summary\n\n");
    printf("   > Tests passed: %d\n", r->pass);
    printf("   > Tests failed: %d\n\n", r->fail);
    printf("***********************************\n");
}

void setup(void)
{
    Result r = {.pass = 0, .fail = 0};
    // init radio for reading
  // spi device, spi speed, ce gpio pin
    uint8_t status = rf24_init_radio("/dev/spidev0.0", 8000000, 25);
    if (status == 0) exit(-1);
    run_test_suite(&r);
    rf24_resetcfg();
    rf24_enableDynamicPayloads();
    rf24_setAutoAckOnPipe(1, 0);
    rf24_setRetries(15,15);
    rf24_setDataRate(RF24_1MBPS);
    rf24_setPALevel(RF24_PA_MAX);
    rf24_setChannel(76);
    rf24_setCRCLength(RF24_CRC_16);
    rf24_setAddressWidth(5);
    rf24_openReadingPipe(1, 0xF0F0F0F0E1LL);
    rf24_startListening();
    rf24_printDetails();
    
}
 
void loop(void)
{
    // 32 byte character array is max payload
    char receivePayload[32];
 
    while (rf24_available(NULL))
    {
        // read from radio until payload size is reached
        uint8_t len = rf24_getDynamicPayloadSize();
        rf24_read(receivePayload, len);
 
        // display payload
        printf("Recvd pkt - len: %d : %s\n", len, receivePayload);
    }
}
 
int main(int argc, char** argv) 
{
    setup();
    while(1)
        loop();
 
    return 0;
}