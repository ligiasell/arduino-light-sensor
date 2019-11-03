//Importing wifi lib
#include <ESP8266WiFi.h>
#include <math.h>

int light;
int x=0;

//Setting variables to wifi credentials
const char* ssid = "AndroidAPL";
const char* password = "nfzo4139";

WiFiServer server(80);

void setup()
{
  Serial.begin(9600);
  setupLightSensor();
  setupLed();
  setupWifi();
}

void setupLightSensor() {
  Serial.println("Setup light sensor");
  pinMode(A0,INPUT);
}

void setupLed() {
    // initialize digital pin LED_BUILTIN as an output.
  Serial.println("Setup led");
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWifi() {
  Serial.println("Setup Wifi");
  IPAddress staticIP(192, 168, 43, 191);
  IPAddress gateway(192, 168, 43, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
 
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);   
  } 
  server.begin();   
  Serial.println("Conected");
  Serial.println(WiFi.localIP());
}

void loop()
{
  light=analogRead(A0);
  Serial.println("Light = " + String(light));

  delay(500);
  if (light < 50) {
    blink();
   }
   webServer();
}

void webServer() {
   // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (light < 50) {
    val = 0;
  } else {
    val = 1;
  }

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r"));
  client.println("Content-Type: text/html");
  client.println();
  //meta-refresh page every 2 seconds
  x=x+1; //page upload counter
          client.println("<HTML>");
          client.print("<HEAD>");
          client.print("<meta http-equiv=\"refresh\" content=\"1\">");
          client.print("<meta charset=\"UTF-8\">");
          client.print("<TITLE />Teste de iluminação</title>");
          client.print("</head>");
          client.println("<BODY>");
          client.println("<br />");
          client.println("<br />");
          client.print("A iluminação está ");
          client.print((val) ? F("maior do que 50") : F("menor ou igual a 50"));
          client.println("<br />");
          client.println("<br />");
          client.print("Page refresh number: ");
          client.println(x); //current refresh count
          client.println("</BODY>");
          client.println("</HTML>");

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}

void blink() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
