/*
  LoRa Duplex communication wth callback

  Sends a message every half second, and uses callback
  for new incoming messages. Implements a one-uint8_t addressing scheme,
  with 0xFF as the broadcast address.

  Note: while sending, LoRa radio is not listening for incoming messages.
  Note2: when using the callback method, you can't use any of the Stream
  functions that rely on the timeout, such as readString, parseInt(), etc.

  created 28 April 2017
  by Tom Igoe

  translated by Akshaya Bali
*/
#include "stdlib.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "stdio.h"
#include "string"
#include "string.h"

//Inlude Library
#include "pico-lora/lora.h"

using std::string;

string outgoing;                 // outgoing message
uint8_t msgCount = 0;            // count of outgoing messages
uint8_t localAddress = 0xBB;     // address of this device
uint8_t destination = 0xFF;      // destination to send to - 0xFF is the broadcast address
long lastSendTime = 0;           // last send time
int interval = 2000;             // interval between sends

void sendMessage(string outgoing) {
  int n = outgoing.length();

  // convert string to char array
  char send[n+1];
  strcpy(send,outgoing.c_str());

  printf("Sending: %s, msg count: %d\n",send, msgCount);

  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(n+1);           // add payload length
  LoRa.print(send);                     // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header uint8_ts:
  uint8_t recipient = LoRa.read();         // recipient address
  uint8_t sender = LoRa.read();            // sender address
  uint8_t incomingMsgId = LoRa.read();     // incoming msg ID
  uint8_t incomingLength = LoRa.read();    // incoming msg length

  string incoming;                 // payload of packet
  char incomingByte;
  
  while (LoRa.available() && incoming.length() <= incomingLength) {            // can't use readString() in callback, so
    incomingByte = (char)LoRa.read();      // add uint8_ts one by one
    incoming += incomingByte;
    // printf("%c", incomingByte);
  }
  // printf("\n");

  // if message is for this device, or broadcast, print details:
  printf("Received from: 0x%x\n", sender);
  printf("Sent to: 0x%x\n", recipient);
  printf("Message ID: %d\n", incomingMsgId);
  printf("Message length: %d\n", incomingLength);
  printf("Message: %s\n", incoming.c_str());
  printf("RSSI: %s\n", LoRa.packetRssi());
  printf("Snr: %s\n", LoRa.packetSnr());
  printf("\n");

  if (incomingLength != incoming.length()) {   // check length for error
    printf("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    printf("This message is not for me.");
    return;                             // skip rest of function
  }
}

int main() {

  stdio_init_all();

  sleep_ms(5000);

  printf("LoRa Duplex with callback\n");

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    printf("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();
  printf("LoRa init succeeded.");

  while (1) {
    if (to_ms_since_boot(get_absolute_time()) - lastSendTime > interval) {
      string message = "HeLoRa World!";   // send a message
      sendMessage(message);
      lastSendTime = to_ms_since_boot(get_absolute_time());            // timestamp the message
      interval = (rand()%2000) + 1000;     // 2-3 seconds
      LoRa.receive();                     // go back into receive mode
    }
  }
  
  return 0;
}