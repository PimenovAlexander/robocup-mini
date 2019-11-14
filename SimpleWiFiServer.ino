/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const char* ssid     = "fenom";
const char* password = "3737373737";

WiFiServer server(80);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x78);

void setup()
{
    Serial.begin(115200);
    pinMode(18, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

    Wire.begin (21, 22);   // sda= GPIO_21  /scl= GPIO_22

     pwm.begin();
     pwm.setPWMFreq(1000);  // This is the maximum PWM frequency
}




char *staticHTMLContent= 
            "HTTP/1.1 200 OK\n"
            "Content-type:text/html\n"
            "\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<style type=\"text/css\">\n"
            "td{font-size: 40pt;}\n"
            "</style>\n"
            "<body>\n"
            "<a href=\"/\" ><font size=20> Iotick based Robofootball robot </font></a><br>"
            "That's what we know how to do:<p>\n"
            "<ul>\n"
            "<li>Click <a href=\"/H\">here</a> to turn the LED on pin 18 on.</li>\n"
            "<li>Click <a href=\"/L\">here</a> to turn the LED on pin 18 off.</li>\n"
            "<li>Click <a href=\"/Scan\">here</a> for scanof I2C bus</li>\n"
            "<li>Click <a href=\"/TestMotor\">here</a> for test of motors/led/pwm</li>\n"
            "</ul>\n"
            "<p>\n"
            "<font size=24>\n"
            "<table><tbody>\n"            
            "<tr> <td><a href=\"/Motor?motor1=0&motor2=100\" >FWD-LEFT</a></td><td><a href=\"/Motor?motor1=100&motor2=100\" >   FWD</a></td> <td><a href=\"/Motor?motor1=100&motor2=0\"    >FWD-RIGHT</a></td> </tr>"
            "<tr> <td><a href=\"/Motor?motor1=-100&motor2=100\">LEFT  </a></td><td><a href=\"/Motor?motor1=0&motor2=0\" >      STOP</a></td> <td><a href=\"/Motor?motor1=100&motor2=-100\" >RIGHT</a></td></tr>"
            "<tr> <td></td>                                           <td><a href=\"/Motor?motor1=-100&motor2=-100\" >BACK</a></td> <td></td> </tr>"            
            "</tbody></table>\n"
            "</font>"
            "<p>\n"
            "<form action=\"/Motor\" method=\"get\" target=\"_self\" enctype=\"application/x-www-form-urlencoded\" >\n"
            "Motor Control<br>\n"
            "Motor1: <input type=\"text\" id=\"motor1\" name=\"motor1\" value=\"0\"><br>"
            "Motor2: <input type=\"text\" id=\"motor2\" name=\"motor2\" value=\"0\"> <br>"
            "<input type=\"submit\">"
            "</form>\n"
            "</body>\n"
            "</html>\n";
      

void loop(){
  WiFiClient client = server.available();   // listen for incoming clients
  

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    String urlLine = "";
    
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            /* Finished header */
           
            break;
          } else {    
            if (currentLine.startsWith("GET") || currentLine.startsWith("POST"))  {
              urlLine = currentLine;
            }              
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    /* Time to react*/
    Serial.print("URL:");
    Serial.println(urlLine);

    /*************************************************************/
    if (urlLine.startsWith("GET /"))
    {
         client.println(staticHTMLContent);
    }

    /*************************************************************/    
    
    // Check to see if the client request was "GET /H" or "GET /L":    
    if (urlLine.startsWith("GET /H")) {
      Serial.println("Switching ON\n");
      client.println ("Switching ON<br>\n");
      digitalWrite(18 , HIGH);               // GET /H turns the LED on
    }
    if (urlLine.startsWith("GET /L")) {
      Serial.println("Switching OFF\n");          
      client.println ("Switching OFF<br>\n");
      digitalWrite(18, LOW);                // GET /L turns the LED off
    }
    if (urlLine.startsWith("GET /Scan")) {
           Serial.println ("I2C scanner. Scanning ...");
           client.println  ("I2C scanner. Scanning ...<br>\n");
           byte count = 0;
        
           Wire.begin();
           for (byte i = 0; i < 128; i++)
           {
             Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
             if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
             {
              Serial.print ("Found address: ");
              Serial.print (i, DEC);
              Serial.print (" (0x");
              Serial.print (i, HEX);     // PCF8574 7 bit address
              Serial.println (")");

              client.print ("Found address: ");
              client.print (i, DEC);
              client.print (" (0x");
              client.print (i, HEX);     // PCF8574 7 bit address
              client.print (")<br>\n");

              
              count++;
              }
            }
            Serial.print ("Found ");      
            Serial.print (count, DEC);        // numbers of devices
            Serial.println (" device(s).");   
        
      }
      if (urlLine.startsWith("GET /TestMotor")) {
          Serial.println("");
          Serial.print  ("PWM test for device ");

          client.println("");
          client.print  ("PWM test for device <br>\n");
         
          
         
        
          // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
          // some i2c devices dont like this so much so if you're sharing the bus, watch
          // out for this!
          //Wire.setClock(400000);

          for (uint8_t pwmnum=0; pwmnum < 16; pwmnum++) 
          {
              Serial.print   ("Probing LED/PWM: ");
              Serial.println (pwmnum);

              client.print    ("Probing LED/PWM:");
              client.println  (pwmnum);
              client.print    ("<br>\n");
                            
              delay(400);
              Serial.println("Toggle ON");              
              client.println ("-Toggle ON<br>\n");
              
              pwm.setPWM(pwmnum, 4096, 0);   
                             
              delay(400);                 
              Serial.println("Toggle OFF");
              client.println ("-Toggle OFF<br>\n");              
              pwm.setPWM(pwmnum, 0, 4096);
          }
    }    
    if (urlLine.startsWith("GET /Motor")) {
          Serial.println("");
          Serial.print  ("Motor speed setup: ");
          Serial.println("");
          int motor1 = urlLine.indexOf("motor1");          
          int motor2 = urlLine.indexOf("motor2");

          Serial.println(motor1);
          int len1 = urlLine.indexOf("&", motor1 + 7);
          Serial.println(motor2);
          int len2 = urlLine.indexOf(" ", motor2 + 7);
          
          String s1 = urlLine.substring(motor1 + 7, len1);
          String s2 = urlLine.substring(motor2 + 7, len2); 

          Serial.println(s1);
          Serial.println(s2);          

          int v1 = s1.toInt();
          int v2 = s2.toInt();

          Serial.println("Values:");
          Serial.println(v1);
          Serial.println(v2);          

          if (v1 > 0) {            
              pwm.setPWM(8, v1*40, 0);
              pwm.setPWM(9, 0, v1*40); 
          } else {
              v1 = -v1;
              pwm.setPWM(8, 0, v1*40);
              pwm.setPWM(9, v1*40, 0);             
          }
          
          if (v2 > 0) {            
              pwm.setPWM(11, v2 * 40, 0);
              pwm.setPWM(10, 0, v2 * 40); 
          } else {
              v2 = -v2;
              pwm.setPWM(11, 0, v2 * 40);
              pwm.setPWM(10, v2 * 40, 0);             
          }      
    }


    
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
