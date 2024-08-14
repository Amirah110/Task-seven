/* ESP32 HTTP IoT Server Example for Wokwi.com

  https://wokwi.com/arduino/projects/320964045035274834

  When running it on Wokwi for VSCode, you can connect to the 
  simulated ESP32 server by opening http://localhost:8180
  in your browser. This is configured by wokwi.toml.
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

#include <ESP32Servo.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

WebServer server(80);
//new 
Servo myServo;

const int LED1 = 26;
const int LED2 = 27;

bool led1State = false;
bool led2State = false;

void sendHtml() {
  String response = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Servo Control</title>
  <style>
    /* CSS styles go here */
  </style>
</head>
<body>
  <h1>Servo Control</h1>
  <input type="range" id="servo-angle" min="0" max="180" value="90" oninput='sendAngle()'>
  <p>Angle: <span id="angle-value">90</span> degrees</p>

  <script>
    const servoAngle = document.getElementById('servo-angle');
    const angleValue = document.getElementById('angle-value');

    function sendAngle() {
      const angle = servoAngle.value;
      angleValue.textContent = angle;

      // Send the angle value to the ESP32
      fetch(`/set-angle?angle=${angle}`)
        .then(response => {
          if (response.ok) {
            console.log(`Angle set to ${angle} degrees`);
          } else {
            console.error(`Error setting angle: ${response.status} - ${response.statusText}`);
          }
        })
        .catch(error => {
          console.error('Error sending angle:', error);
        });
    }
  </script>
</body>
</html>
)";

  server.send(200, "text/html", response);
}

void handleSetAngle() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    myServo.write(angle);
    server.send(200, "text/plain", "Angle set successfully");
    Serial.printf("Angle set to %d degrees\n", angle);
  } else {
    server.send(400, "text/plain", "Missing angle parameter");
  }
}

void setup(void) {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
// Attach servo to GPIO pin
  myServo.attach(18);
  server.on("/", sendHtml);
// Set up web server
  server.on("/set-angle", handleSetAngle);
  server.begin();
 

  sendHtml();


  server.begin();
  Serial.println("HTTP server started (http://localhost:18080)");
}

void loop(void) {
  server.handleClient();
}
