/*===========================================================
 * Tiny Stick Cylon, Version 2.
 * See https://github.com/gsalaman/cylon_v2/blob/master/README.md
 */
#if defined(__AVR_ATmega328P__)
#define PLATFORM_UNO
#else
#error "PLATFORM UNSUPPORTED"
#endif

#include <Adafruit_NeoPixel.h>

// Which pin to use for LED control
#define LED_PIN    3

// Platform specific:  button pin and POT pin.
#ifdef PLATFORM_UNO
#define BUTTON_PIN 8
#define POT_PIN    A0
#elif PLATFORM_TINY
#define BUTTON_PIN 0      
#define POT_PIN    2 
#endif

#define NUMPIXELS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB+NEO_KHZ800);

#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF



/*================================================================================
 * fillAll
 */
void fillAll( uint32_t color )
{
  int i;

  for (i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
  }
}

/*========================================================================
 * fillGradient
 *
 * This function fills an array with a gradient from start color to end color.
 */
void fillGradient(uint32_t *strip, int start_index, uint32_t start_color, int end_index, uint32_t end_color)
{
  int i;
  
  uint32_t start_red;
  uint32_t end_red;
  uint32_t start_green;
  uint32_t end_green;
  uint32_t start_blue;
  uint32_t end_blue;
  uint32_t composite_color;

  int red_step;
  int green_step;
  int blue_step;

  int num_steps = end_index - start_index;

  start_red = start_color & 0xFF0000;
  start_red = start_red >> 16;
  start_green = start_color & 0x00FF00;
  start_green = start_green >> 8;
  start_blue = start_color & 0x0000FF;

  end_red = end_color & 0xFF0000;
  end_red = end_red >> 16;
  end_green = end_color & 0x00FF00;
  end_green = end_green >> 8;
  end_blue = end_color & 0x0000FF;

  red_step = end_red - start_red;
  green_step = end_green - start_green;
  blue_step = end_blue - start_blue;

  #if 0
  Serial.print("Num steps: ");
  Serial.println(num_steps);
  Serial.print("r/g/b step: ");
  Serial.print(red_step);
  Serial.print(" ");
  Serial.print(green_step);
  Serial.print(" ");
  Serial.println(blue_step);
  #endif

  
  for (i = 0; i <= num_steps; i++)
  {
    #if 0
    Serial.print("i=");
    Serial.print(i);
    Serial.print(", r:");
    Serial.print(start_red+ ((i*red_step)/num_steps));
    Serial.print(" b:");
    Serial.println(start_blue + ((i*blue_step)/num_steps));
    #endif
    
    composite_color = 
      (start_blue + ((i*blue_step)/num_steps));
    composite_color |=
      (start_green + ((i*green_step)/num_steps)) << 8;
    composite_color |= 
      (start_red + ((i*red_step)/num_steps)) << 16;

    strip[start_index + i] = composite_color;
  }

}

/*================================================
 * Debounce function.
 * 
 * Only count a button press on release, and only if it's been down for a sufficient 
 * amount of time.
 *===============================================*/ 
#define DEBOUNCE_TIME_MS 50
bool buttonPressed( void )
{
  static int last_state=HIGH;
  int current_state;
  static unsigned long down_start_time=0;
  unsigned long current_time;

  current_state = digitalRead(BUTTON_PIN);

  /* Look for high-to-low transistions */
  if (last_state == HIGH)
  {
    if (current_state == LOW)
    {
      last_state = current_state;
      down_start_time = millis();
    }

    return(false);
  }
  
  else
  {
    /* look for the release with "enough time" to count a button press. */
    if (current_state == HIGH)
    {
      /* button went from low to high.  Was it down long enough? */
      current_time = millis();
      if (current_time - down_start_time > DEBOUNCE_TIME_MS)
      {
        last_state = current_state;
        return(true);
      }
      else
      {
        /* went up too quick...this is a bounce.  */
        last_state = current_state;
        return(false);
      }
    }
    else
    {
      /* We're still "low".  Waiting for release.  */
      return(false);
    }
  }  
}

void setup()
{
    int i;

    #ifdef PLATFORM_UNO
    Serial.begin(9600);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    #endif
    
    pixels.begin();

    // Power on self-test...see if all the pixels are working.
    for (i=0; i<NUMPIXELS; i++)
    {
      pixels.setPixelColor(i,COLOR_BLUE);
      delay(50);
      pixels.show();
    }

    #ifdef PLATFORM_UNO
    Serial.println("initialized");
    #endif
    
}

void loop()
{
  
}
