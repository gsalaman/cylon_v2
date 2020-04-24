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

/* The neopixel driver uses a 24 bit value to refer to color, where
 *  the upper 8 bits are the red value, the middle 8 green, and the lower 8 blue.
 *  I'm gonna refer to this as a "composite color".
 */

/* A few helpful color #defines... */
#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF

#define MAX_EYE_SIZE (NUMPIXELS - 1)

/* Window concepts:
 *  The virtual window is a large array of pixels, but we're only going to 
 *  show the middle 8 pixels...and we'll refer to those as the "real window" 
 *  pixels. 
 */
#define VIRTUAL_WINDOW_SIZE (NUMPIXELS+2*MAX_EYE_SIZE)
uint32_t virtual_window[VIRTUAL_WINDOW_SIZE];

#define REAL_WINDOW_START_INDEX MAX_EYE_SIZE 
#define REAL_WINDOW_END_INDEX (REAL_WINDOW_START_INDEX + NUMPIXELS - 1)

/* The cyclon eye parameters */
int eye_head_pos=REAL_WINDOW_START_INDEX;  // this is the virutal window index
int eye_size=4;

typedef enum
{
  DIR_RIGHT,
  DIR_LEFT
} dir_type;

dir_type current_eye_dir=DIR_RIGHT;

uint32_t eye_color=COLOR_RED;
uint32_t background_color=COLOR_BLUE;

uint32_t display_delay_ms=100;



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

/* Fills the entire virtual window with a given color */
void virtual_fill(uint32_t color)
{
  int i;

  for (i=0; i<VIRTUAL_WINDOW_SIZE; i++)
  {
    virtual_window[i] = color;
  }
}

/*========================================================================
 * fillGradient
 *
 * This function fills an array with a gradient from start_color to end_color.
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

  /* A little shifting and masking:
   *  The blue byte is the bottom 8 bits
   *  The green byte is the middle 8 bits
   *  The red byte is the top 8 bits
   */
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

  #if 0  // Debug serial prints
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
    #if 0  // Debug serial prints
    Serial.print("i=");
    Serial.print(i);
    Serial.print(", r:");
    Serial.print(start_red+ ((i*red_step)/num_steps));
    Serial.print(" b:");
    Serial.println(start_blue + ((i*blue_step)/num_steps));
    #endif

    /* We're gonna build a composite color by "stepping" through the
     *  individual color bytes from start to end, and building a 24-bit
     *  composite color value as we go.
     */
    composite_color = 
      (start_blue + ((i*blue_step)/num_steps));
    composite_color |=
      (start_green + ((i*green_step)/num_steps)) << 8;
    composite_color |= 
      (start_red + ((i*red_step)/num_steps)) << 16;

    /* Finally, set the array value to our composite color. */
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

/*=================================================
 * display_pixels
 * 
 * This function displays the "real window" pixels on the 
 * tiny stick.
 *
 * See README for description of real vs virtual window.
 */
void display_pixels( void )
{
  int i;

  /* Show the middle pixels of the virtual window (our "real window") on the 
   *  stick LEDs.
   */
  for (i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, virtual_window[i+REAL_WINDOW_START_INDEX]); 
  }
  pixels.show();
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

    /* a little debug....gonna fill our entire virtual array with a gradient. */
    fillGradient(virtual_window, 0, COLOR_RED, VIRTUAL_WINDOW_SIZE -1, COLOR_BLUE);
    
}

/* this function shows the eye, based on the head position, length, 
 *  and direction
 */
void show_eye( void )
{ 
   int tail_pos;  // virtual window index of the tail

   /* start with the virtual window filled with the background color */  
   virtual_fill(background_color);

   /* Draw in the eye, depending on direction */
   if (current_eye_dir == DIR_RIGHT)
   {
     /* if we're going right, the tail is left of the eye...meaning we 
      *  fillGradient from the tail to the head
      */
     tail_pos = eye_head_pos - eye_size;
     fillGradient(virtual_window, 
                  tail_pos, background_color, 
                  eye_head_pos, eye_color);

     /* if the head of the eye is outside our real window, we want to make the
      *  edge our eye color
      */
     if (eye_head_pos > REAL_WINDOW_END_INDEX)
     {
       virtual_window[REAL_WINDOW_END_INDEX] = eye_color;
     }
   }   // end of "moving right" adjustments
   else
   {
      /* if we're going left, the tail is right of the eye, 
       *  meaning we fill gradient from the head to tail.
       */
      tail_pos = eye_head_pos + eye_size;

      fillGradient(virtual_window, 
                  eye_head_pos, eye_color, 
                  tail_pos, background_color);

     /* if the head of the eye is outside our real window, we want to make the
      *  edge our eye color
      */
     if (eye_head_pos < REAL_WINDOW_START_INDEX)
     {
       virtual_window[REAL_WINDOW_START_INDEX] = eye_color;
     }    
   }  // end of "moving left" adjustments

   /* Now that we've updated our virtual window with the eye, do the 
    *  actual display
    */
   display_pixels(); 
}

void update_display(void)
{
  static uint32_t last_update_time_ms = 0;
  uint32_t        curr_time_ms;

  curr_time_ms = millis();

  /* if it hasn't been long enough for an update, just return.  */
  if (last_update_time_ms + display_delay_ms > curr_time_ms)
  {
    return;
  }

  last_update_time_ms = curr_time_ms;

  Serial.print("eye pos: ");
  Serial.print(eye_head_pos);
  Serial.print(", dir= ");
  Serial.println(current_eye_dir);
  
  /* do we need to reverse direction? */
  if ((current_eye_dir == DIR_RIGHT) && 
      (eye_head_pos == REAL_WINDOW_END_INDEX + eye_size))
  {
    current_eye_dir = DIR_LEFT;

    /* because our head pos is far right in virtual window, we need to reset
     *  it here the the right edge.
     */
    eye_head_pos = REAL_WINDOW_END_INDEX;
  }
  else if ((current_eye_dir == DIR_LEFT) &&
           (eye_head_pos == REAL_WINDOW_START_INDEX - eye_size))
  {
    current_eye_dir = DIR_RIGHT;

    /* because our head pos is far left in virtual window, we need to reset
     *  it here to the left edge.
     */
    eye_head_pos = REAL_WINDOW_START_INDEX;
  }

  /* move the eye one slot in the desired direction */
  if (current_eye_dir == DIR_RIGHT)
  {
    eye_head_pos++;
  }
  else
  {
    eye_head_pos--;
  }
  
  /* ...and display the eye on our pixels. */
  show_eye();
}

void loop()
{
  
  /* update display */
  update_display();

  /* update pot */

  /* check button for eye size updates */
  
  
}
