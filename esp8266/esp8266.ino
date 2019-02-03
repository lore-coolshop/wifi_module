#include <ESP8266WiFi.h>
extern "C" {
#include "common.h"
}
#include "at_base.h"
#include "connection.h"
#include "serial.h"
#include "tcp_server.h"
#include "tcp_client.h"


char readBuffer[BUFFER_LENGTH];
char writeBuffer[BUFFER_LENGTH];
#define MAX_SUPPORT 3
char support[MAX_SUPPORT][BUFFER_LENGTH];

bool clientSendAtRequest = false;

WiFiClient tcpClients[MAX_CLIENTS];
bool clientConnected[MAX_CLIENTS];

WiFiServer tcpServer[MAX_SERVER];
int usedPorts[MAX_SERVER];

static const int interval = 500;
long lastTimePassed;

void setup() {
  Serial.begin(74880);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password, ap_channel, false);

  //tcpServer[1] = WiFiServer(DEFAULT_SERVER_PORT);
  //tcpServer[1].begin();
  
  for(int i=0; i<MAX_SERVER; i++){
    usedPorts[i] = -1;
  }
  for(int i=0; i<MAX_CLIENTS; i++){
    clientConnected[i] = false;
  }
}



void loop() {

  bool error = false;

  cleanAllBuffers();

  if(checkClientConnection(writeBuffer, support)){
    Serial.println(writeBuffer);
    return;
  }

  cleanAllBuffers();
  
  if(checkClientRequest(readBuffer, writeBuffer, support, &getClientAtRequest)){
    Serial.println(writeBuffer);
    return;
  }
  
  if(!clientSendAtRequest){
    int maxIndex = readFromSerial(readBuffer);
    if(maxIndex == 0)
      return;
  } else {
    clientSendAtRequest = false;
  }
    
  Serial.println();
  Serial.println(readBuffer);

  // chipsend must take priority over other commands
  checkAtCipsend(readBuffer, writeBuffer, support, &handleServerReadFromSerial, &error);

  if(millis() - lastTimePassed > interval || clientSendAtRequest){
    lastTimePassed = millis();
    
    checkAt(readBuffer, writeBuffer);
    checkAtRst(readBuffer, writeBuffer);
    checkAtGmr(readBuffer, writeBuffer);
    if(checkAtDiag(readBuffer, writeBuffer, Serial)){
      return;
    }
    checkAtCwmode(readBuffer, writeBuffer, support, &error);
  
    checkAtCwqap(readBuffer, writeBuffer);
    checkAtCwjap(readBuffer, writeBuffer, support, &error);
    checkAtCipsta(readBuffer, writeBuffer, support);
    checkAtCwsap(readBuffer, writeBuffer, support, &error);
  
    checkAtCwhostname(readBuffer, writeBuffer, support, &error);
    checkAtCipserver(readBuffer, writeBuffer, support, &error);

    checkAte(readBuffer, writeBuffer, &error);
    
    if(error) {
      _memcpy(writeBuffer, ERR, sizeof(ERR));
    }
  
    if(strlen(writeBuffer) > 0){
      Serial.println(writeBuffer);
    }
  
  }
}


char* handleServerReadFromSerial(int readLength){
  Serial.println(OKY);
  Serial.print("\n> ");
  int start = millis();
  while(readFromSerial(readBuffer) + 3 < readLength){
    if(millis() - start > 20000) {
      break; 
    }
  }
  return readBuffer;
}


void getClientAtRequest(char* request){
  Serial.print("\nNext AT is from client...");
  clientSendAtRequest = true;
  _memcpy(readBuffer, request, strlen(request));
}


void  cleanAllBuffers(){
  memset(writeBuffer, ES, BUFFER_LENGTH);
  memset(readBuffer, ES, BUFFER_LENGTH);
  cleanBidimensionalCharArray(support);
}
