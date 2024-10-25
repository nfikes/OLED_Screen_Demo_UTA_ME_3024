// By Nathan Fikes, UTA ME 3024 Design & Economics
// Adapted from an examples by Rui Santos in https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/

// The wire header file allows I2C communication between the Arduino R3 and OLED.
#include <Wire.h>

// The GFX library provides genaric draw commands for use on any type of display.
#include <Adafruit_GFX.h>
// This library provides functions to control OLED displays using the SSD1306 driver chip.
#include <Adafruit_SSD1306.h>

// These are some standard C++ headers that are used throughout the program.
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Declaring the screen width and height to get the correct aspect ratio
// We are using a 128 x 32 pixel screen. Keep this in mind when drawing.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// This line instatnitates an OLED Object with several characteristics.
// Height and Width are passed in, &Wire indicates typical SDA SCL connections.
// The -1 indicates that there is no reset pin attached.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Later on in the program we will have a use for this static declaration.
static String receivedData;

// In the setup loop we can do a couple important items.
void setup()
{
  // Set serial to communicate on this BAUD specified here.
  Serial.begin(115200);

  // A guard statement that detects if initialization fails and if true stalls in an empty for loop.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();

  // Set parameters for text size, color, cursor position, and text content.
  // The display works by queueing several commands and then pushing them
  // to the display by calling the display() function.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 16);
  String Oled_Text = "OLED Demo Text";
  display.println(Oled_Text);

  // These are two custom functions that will be explained further in detail
  // later on in the program.
  // The parameters are empty as you invoke the function for queueing display commands.
  UI_components();

  // The custom parameters are (shift, expansion, amt)
  UI_boarder(2, 5, 23);

  display.display(); 
  delay(100);

  // When the setup is complete, we can show an initialization string onto Serial
  // to show that the process has completed.
  Serial.print("Screen Successfully Initialized.\n");
}

// This is a custom function that can queue up several commands related to the UI
// parts on the screen.
void UI_components()
{
  // Prints the Initials VT for [Virginia Tech] on the upper right.
  display.setTextSize(0.5);
  display.setTextColor(WHITE);
  display.setCursor(108, 8);
  display.println("VT");
  
  // Adds a rounded rectangle to the screen.
  // The parameters are (x0, y0, w, h, radius, color)
  display.drawRoundRect(5, 5, 118, 22, 3, WHITE);

  // Drawing a filled circle to serve as the back of the clock.
  // The parameters are (x0, y0, r, color)
  display.fillCircle(121, 25, 6, BLACK);
}

// A custom function to handle drawing the clock arm and the clock boarder.
void UI_clock(int angle, int rate, int arm_length)
{
  // Two lines can be drawn to make the arm of the clock spin.
  // A black line is drawn a few steps behind a white line.
  // At a fast enough screen refresh rate, this will look like it is spinning.
  // The parameters are (x0, y0, x1, y1, color)
  display.drawLine(121, 25, 121+arm_length*cos((angle-rate)*(M_PI/180)), 25+arm_length*sin((angle-rate)*(M_PI/180)), BLACK);
  display.drawLine(121, 25, 121+arm_length*cos(angle*(M_PI/180)), 25+arm_length*sin(angle*(M_PI/180)), WHITE);

  // Like before a plain circle can be drawn for the clock boarder.
  // The parameters are (x0, y0, r, color)
  display.drawCircle(121, 25, 6, WHITE);

  // Internally we want to update the clock every time this command is called.
  display.display();
}

void UI_boarder(int shift, int expansion, int amt)
{
  // For more procedural ways of making boarders, we can use for loops
  // to queue many commands in a sequence.
  for(int i = 1; i < amt; i++)
  {
    // We can draw lines with rules governed by the parent function parameters (shift, expansion, amt)
    // Below we are drawing lines at an angle. (x0, y0, x1, y1, color)
    display.drawLine(5+i*expansion, 0, 5+i*expansion+shift, 5, WHITE);
  }

  for(int j = 0; j < (amt/2)-1; j++)
  {
    // We can also be more decorative with triangles.
    // The parameters for triangles are (x0, y0, x1, y1, x2, y2, color)
    display.fillTriangle(9+j*expansion*2, 28, 13+j*expansion*2, 28, 11+j*expansion*2, 30, WHITE);
    display.drawTriangle(14+j*expansion*2, 30, 18+j*expansion*2, 30, 16+j*expansion*2, 28, WHITE);
  }
}

void loop()
{
  // Inside here we can run things in a couple loops.
  // Generally the void loop is sufficient but for loops can give you
  // a consise way to indicate the manipulation of variables.
  
  // Here we can define the rate in degrees at which the clock will spin.
  int rate = 2;
  for(int angle = 0; angle < 360; angle = angle + rate)
  {
    // To animate the clock we can invoke the special function we created before to handle
    // the clock UI.
    // The custom parameters here are (angle, rate, arm_length)
    UI_clock(angle, rate, 4);

    // On the deepest nested loop ask if serial is avaliable.
    if (Serial.available())
    {
      // If it is, we can take it's contents and put it into p for package.
      int p = Serial.read();

      // p is an array of byte data that cannot be printed.
      // Each time you input text into the Serial line it has a new line
      // Attached to it, we can use this to populate a variable defined
      // on the static global scope to hold the string.
      if (p != '\n') 
      { 

        receivedData += (char)p;

      } else 
      { 
        // Like normally this string, or integer if you chose, can be manipulated
        // just like any other variable.
        if (receivedData == "Hello") 
        {
            Serial.println("You've typed \"Hello\"");
        }

        Serial.print(receivedData);
        Serial.print("\n");
        
        // But it can also be printed to our OLED screen.
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(10, 16);
        display.println(receivedData);

        // Since the display was cleared, replace the UI components.
        UI_components();
        UI_boarder(2, 5, 23);

        display.display();

        // Since we are done with the data, we can clear this accumulator.
        receivedData = "";
      }
    }
  }
}