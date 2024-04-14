#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Configurable layout variables
int circleDiameter = 8;
int spacingBetweenElements = 20;
int centerlineWidth = 20;

bool goingDown = true;
int positionPercentage = 0;

void drawVerticalPattern() {
  int totalElements = 5; // Four circles and one vertical line
  int totalSpacing = (totalElements - 1) * spacingBetweenElements;
  int totalWidth = 4 * circleDiameter + totalSpacing;

  int startX = (SCREEN_WIDTH - totalWidth) / 2; // Horizontal margin

  // Draw a vertical separator line in the middle of the screen
  display.drawFastVLine(SCREEN_WIDTH / 2,
                        SCREEN_HEIGHT / 2 - centerlineWidth +
                            3, // fugly magic number fix later
                        centerlineWidth, WHITE);

  // Draw the circles
  for (int i = 0; i < 4; i++) {
    // Calculate the horizontal offset
    int xOffset = startX + i * (circleDiameter + spacingBetweenElements);

    // Check position to adjust for the line in the middle
    if (i >= 2) {
      xOffset += spacingBetweenElements; // Adjust position to move it right
                                         // after the line
    }

    display.drawCircle(xOffset + (circleDiameter / 2),
                       SCREEN_HEIGHT / 2 - circleDiameter, circleDiameter / 2,
                       WHITE);
  }
}

void drawIndicatorTriangle(int positionPercentage) {
  // Clear previous drawings but keep the static elements
  display.clearDisplay(); // Optionally, clear only the part of the display
                          // where the triangle moves
  drawVerticalPattern();  // Redraw the static pattern if needed

  // Dimensions and position calculation for the triangle
  int triangleBaseWidth = 10; // Width of the triangle base
  int triangleHeight = 16;    // Height of the triangle

  // Calculate the X position for the triangle based on the percentage
  int xPosition =
      map(positionPercentage, 0, 100, 0, SCREEN_WIDTH - triangleBaseWidth);

  // Coordinates for the triangle
  int x1 = xPosition;                            // Left corner of the base
  int x2 = xPosition + triangleBaseWidth;        // Right corner of the base
  int x3 = xPosition + (triangleBaseWidth / 2);  // Top point of the triangle
  int y1 = (SCREEN_HEIGHT / 2) + triangleHeight; // Base level Y coordinate
  int y2 = (SCREEN_HEIGHT / 2) + triangleHeight; // Base level Y coordinate
  int y3 = (SCREEN_HEIGHT / 2);                  // Top point Y coordinate

  // Draw the triangle
  display.fillTriangle(x1, y1, x2, y2, x3, y3, WHITE);

  // Display the updates
  display.display();
}

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);

  // Draw the initial pattern
  drawVerticalPattern();
  display.display();
}

void loop() {
  // Move the triangle indicator
  drawIndicatorTriangle(positionPercentage);

  if (goingDown) {
    positionPercentage += 1;
  } else {
    positionPercentage -= 1;
  }

  if (positionPercentage >= 100) {
    goingDown = false;
  } else if (positionPercentage <= 0) {
    goingDown = true;
  }

  delay(10); // Just a simple delay, adjust as necessary for your application
}
