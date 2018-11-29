/*
    This sketch shows the Ethernet event usage
*/

#include <ETH.h>
#include <MLX90393.h>

#define N_Mag 4
MLX90393 mlx[N_Mag];

//Network related
static bool eth_connected = false;
IPAddress ip(10, 0, 0, 40);
IPAddress gate(10, 0, 0, 1);
IPAddress mask(255, 255, 255, 0);

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
  ETH.config(ip,gate,mask,gate);

  server.begin();
  server.setNoDelay(true);

  Serial.println("hello");
  
  Wire.begin();

  delay(1000);
  //uint8_t status;
  for(uint8_t j = 0; j < 3; j++){
    for(uint8_t i = 0; i < N_Mag; i++){
      Serial.println(mlx[i].begin(i/2, i%2));
      delay(500);
    }
  }
  
  Serial.println("setup done");
}

// function to record measurements from a magnetometer
void measurement(WiFiClient *client, MLX90393 *mlx)
{   
  MLX90393::txyz data;
  mlx->readData(data);
  delay(500);
  client->println("Magnetometer Readings:");
  client->print(data.x);
  client->print(" ");
  client->print(data.y);
  client->print(" ");
  client->print(data.z);
  client->print(" ");
  client->println(data.t);
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
          if (serverClients[i]) serverClients[i].stop();
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
        if (serverClients[i].available()){
          char input = serverClients[i].read();

          switch(input) {
              case '0':
                measurement(&serverClients[i], &mlx[0]);
                break;
              case '1':
                measurement(&serverClients[i], &mlx[1]);
                break;
              case '2':
                measurement(&serverClients[i], &mlx[2]);
                break;
              case '3':
                measurement(&serverClients[i], &mlx[3]);
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
