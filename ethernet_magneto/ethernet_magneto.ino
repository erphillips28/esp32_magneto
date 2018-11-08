/*
    This sketch shows the Ethernet event usage
*/

#include <ETH.h>
#include <MLX90393.h>


MLX90393 mlx1;
MLX90393 mlx2;


//Network related
static bool eth_connected = false;
IPAddress ip(10, 0, 0, 40);
IPAddress gate(10, 0, 0, 1);
IPAddress mask(255, 255, 255, 0);
IPAddress dns(10, 0, 0, 1);

//Server related
#define MAX_SRV_CLIENTS 1
WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void setup()
{
  Serial.begin(115200);

  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  ETH.config(ip,gate,mask,dns);

  server.begin();
  server.setNoDelay(true);

  Serial.println("hello");
  
  Wire.begin();

  delay(1000);
  uint8_t status = mlx1.begin(0, 0);
  Serial.println(status);
  uint8_t status2 = mlx2.begin(1, 0);
  Serial.println(status2);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  delay(1000);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  delay(1000);
  Serial.println( mlx1.begin(0, 0));
  Serial.println(mlx2.begin(1, 0));
  //uint8_t status = mlx.begin();
  
  Serial.println("setup done");
}

void measurement(WiFiClient *client)
{ 
  MLX90393::txyz data1;
  MLX90393::txyz data2;
  Serial.println("hello");
  //if (Serial.available()){
  //  Serial.read();
    mlx1.readData(data1);
    mlx2.readData(data2);
    client->println("Magnetometer 1 Readings:");
    client->print(data1.x);
    client->print(" ");
    client->print(data1.y);
    client->print(" ");
    client->print(data1.z);
    client->print(" ");
    client->println(data1.t);
    client->println("Magnetometer 2 Readings:");
    client->print(data2.x);
    client->print(" ");
    client->print(data2.y);
    client->print(" ");
    client->print(data2.z);
    client->print(" ");
    client->println(data2.t); 
    
   /* if (debug) {
      Serial.println("Magnetometer 1 Readings:");
      Serial.print(data1.x);
      Serial.print(" ");
      Serial.print(data1.y);
      Serial.print(" ");
      Serial.print(data1.z);
      Serial.print(" ");
      Serial.println(data1.t);
      Serial.println("Magnetometer 2 Readings:");
      Serial.print(data2.x);
      Serial.print(" ");
      Serial.print(data2.y);
      Serial.print(" ");
      Serial.print(data2.z);
      Serial.print(" ");
      Serial.println(data2.t); 
      }*/
  delay(3000);
}

void loop()
{
  uint8_t i;
  if (eth_connected) {
    //check if there are any new clients
    WiFiClient client = server.available();
    if (server.hasClient()){
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          if (!serverClients[i]) Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].remoteIP());
          serverClients[i].println("Hello client!");
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        server.available().stop();
      }
    }
    //check clients for data
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        if(serverClients[i].available()){
          char input = serverClients[i].read();

          switch(input) {
              case '1':
                break;
              case '2':
                Serial.println("Hi Daniel");
                //serverClients[i].print(measurement(&client));
                measurement(&serverClients[i]);
                break;
            }
        }
      }
      else {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
      }
    }
  } else {
    Serial.println("ETH not connected!");
    delay(100);
    for(i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i]) serverClients[i].stop();
    }
  }
}
