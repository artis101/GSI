#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// last update timestamp
unsigned long lastUpdate = 0;

// Configurable layout variables
int circleDiameter = 8;
int spacingBetweenElements = 20;
int centerlineWidth = 20;

bool goingDown = true;
bool goingRight = true;

int verticalPositionPercentage = 50;
int horizontalPositionPercentage = 50;

void drawCenterCircle() {
  display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, circleDiameter * 1.5,
                     WHITE);
}

void drawVerticalPattern() {
  int totalElements = 5; // Four circles and one vertical line
  int totalSpacing = (totalElements - 1) * spacingBetweenElements;
  int totalWidth = 4 * circleDiameter + totalSpacing;

  int startX = (SCREEN_WIDTH - totalWidth) / 2; // Horizontal margin

  // Draw a vertical separator line in the middle of the screen
  /*display.drawFastVLine(SCREEN_WIDTH / 2,
                        SCREEN_HEIGHT / 2 - centerlineWidth +
                            3, // fugly magic number fix later
                        centerlineWidth, WHITE);*/

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

void drawHorizontalIndicator(int positionPercentage) {
  // Dimensions for the line
  int lineThickness = 1; // Thickness of the horizontal line
  int dashLength = 12;   // Length of each dash
  int gapLength = 4;     // Length of the gap between dashes

  // Calculate the Y position for the line based on the percentage
  int yPos = map(positionPercentage, 0, 100, 0, SCREEN_HEIGHT - lineThickness);

  // // Draw the dashed horizontal line
  for (int x = 0; x < SCREEN_WIDTH; x += (dashLength + gapLength)) {
    // Draw each segment of the dash
    display.drawFastHLine(x, yPos, dashLength, WHITE);
  }
}

void drawVerticalIndicator(int positionPercentage) {
  // Dimensions and position calculation for the triangle
  int triangleBaseWidth = 8; // Width of the triangle base
  int triangleHeight = 12;   // Height of the triangle
  int offsetY = 16;          // Offset to adjust the triangle position

  // Calculate the X position for the triangle based on the percentage
  int xPosition =
      map(positionPercentage, 0, 100, 0, SCREEN_WIDTH - triangleBaseWidth);

  // Coordinates for the triangle pointing upwards
  int x1 = xPosition;                           // Left corner of the base
  int x2 = xPosition + triangleBaseWidth;       // Right corner of the base
  int x3 = xPosition + (triangleBaseWidth / 2); // Top point of the triangle
  int y1 = (SCREEN_HEIGHT / 2) - triangleHeight -
           offsetY; // Base level Y coordinate, adjusted
                    // for upward orientation
  int y2 = (SCREEN_HEIGHT / 2) - triangleHeight -
           offsetY; // Base level Y coordinate, adjusted
                    // for upward orientation
  int y3 = (SCREEN_HEIGHT / 2) -
           offsetY; // Top point Y coordinate, remains the centerline Y

  // Draw the triangle
  display.fillTriangle(x1, y1, x2, y2, x3, y3, WHITE);
}

void drawAirplane() {
  // Placeholder for the airplane drawing
  // display.drawBitmap(0, 0, airplane, 128, 64, WHITE);
}

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.setTextColor(WHITE);
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0x8F);
}

void drawUI() {
  display.clearDisplay();
  drawVerticalPattern();
  drawCenterCircle();
  drawVerticalIndicator(verticalPositionPercentage);
  drawHorizontalIndicator(horizontalPositionPercentage);
  display.display();
}

void fakeMovement() {
  if (goingDown) {
    verticalPositionPercentage--;
  } else {
    verticalPositionPercentage++;
  }

  if (verticalPositionPercentage <= 40) {
    goingDown = false;
  } else if (verticalPositionPercentage >= 60) {
    goingDown = true;
  }

  if (goingRight) {
    horizontalPositionPercentage++;
  } else {
    horizontalPositionPercentage--;
  }

  // Check if the triangle reached the edges
  if (horizontalPositionPercentage <= 30) {
    goingRight = true;
  } else if (horizontalPositionPercentage >= 70) {
    goingRight = false;
  }
}

void loop() {
  if (millis() - lastUpdate > (1000 / 30)) {
    fakeMovement();
    drawUI();
    lastUpdate = millis();
  }
}
