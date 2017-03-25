/**************************************************************************
 *  altera.h                                                              *
 *                                                                        *
 *  Skyelar Craver 2017                                                   *
 *                                                                        *
 *  This header file aims to make developing simple C applications        *
 *  for the embedded peripherals of the altera DE0-CV.                    *
 *                                                                        *
 *  This program may have severe conflicts with other altera headers!     *
 *                                                                        *
 *  Licensed under GNU GPL v3.0                                           *
 **************************************************************************/


/*
this check is not bulletproof!
practice caution when using with other headers
*/

#ifndef ALTERA_H
#define ALTERA_H

//TODO: add interupt service routine dependencies

/**************************************************************************
 *  Constant definition block                                             *
 *    This block of definitions gives some very useful memory addresses   *
 *    for embedded peripherals, which are used in many of the functions   *
 *    defined in this file and are globally available for any other use.  *
 **************************************************************************/

#define PIXEL_BUFFER 0x08000000
#define CHARACTER_BUFFER 0x09000000
#define HEX03_PTR 0xFF200020
#define HEX45_PTR 0xFF200030
#define TIMER_PTR 0xFF202000
#define SWITCH_PTR 0xFF200010
#define ONE_SECOND 1000
//TODO: add colors to constants


/*
very fast absolute value function
*/

int abs(int x) {
    int y = (x >> 31);
    return (x ^ y) - y;
}


/*
very fast integer sign function
*/

int sgn(int x) {
  return (x>0) - (x<0);
}


/**************************************************************************
 *  print_segment                                                         *
 *    this function will define the pointer for the embedded hex displays *
 *    and write a prepared 64bit number to it                             *
 **************************************************************************/

void print_segment(long long unsigned int x){
        volatile int* hex03_ptr = (int*) HEX03_PTR;
        volatile int* hex45_ptr = (int*) HEX45_PTR;
        *(hex03_ptr) = 0;
        *(hex45_ptr) = 0;
        *(hex03_ptr) = (int)x;
        if (x > 0xffffffff) { // for DE0-CV additional segment displays
                *(hex45_ptr) = x>>32;
        }
}


/**************************************************************************
 *  board_timer                                                           *
 *    configures and sets the built in interval timer to a time           *
 *    approximately in miliseconds and sleeps until the timer runs out    *
 **************************************************************************/

void board_timer(int x) {//TODO: re-write using interupt services
        int timer = 50000*x;
        volatile int*timer_low = (int*)TIMER_PTR+0x8;
        volatile int*timer_high = (int*)TIMER_PTR+0xc;
        volatile int*timer_ctrl = (int*)TIMER_PTR+0x4;
        volatile int*timer_status = (int*)TIMER_PTR;

        *(timer_low) = timer;
        *(timer_high)= timer>>16;
        *(timer_ctrl) = 0x4;

        while (*(timer_status) != 0x1) ;

        *(timer_status) = 0;
}


/**************************************************************************
 *  to_segment                                                            *
 *    takes a string up to length 6 and uses the largest case statement   *
 *    to evaluate the character and translate it to a visually similar    *
 *    arrangement to be used with print_segment                           *
 **************************************************************************/

long long unsigned int to_segment(char* x) {//TODO: make work with arbitrary
        long long unsigned int y = 0;
        for (int count = 0; count < 12; count++)
        {
                switch (x[count]) {
                case '0': y +=  0x3F;
                        break;
                case '1': y +=  0x06;
                        break;
                case '2': y +=  0x5B;
                        break;
                case '3': y +=  0x4F;
                        break;
                case '4': y +=  0x66;
                        break;
                case '5': y +=  0x6D;
                        break;
                case '6': y +=  0x7d;
                        break;
                case '7': y +=  0x27;
                        break;
                case '8': y +=  0x7F;
                        break;
                case '9': y +=  0x6F;
                        break;
                case 'A': y +=  0x77;
                        break;
                case 'b': y +=  0x7c;
                        break;
                case 'C': y +=  0x39;
                        break;
                case 'c': y +=  0x58;
                        break;
                case 'd': y +=  0x5e;
                        break;
                case 'E': y +=  0x79;
                        break;
                case 'F': y +=  0x71;
                        break;
                case 'H': y +=  0x76;
                        break;
                case 'h': y +=  0x74;
                        break;
                case 'I': y +=  0x30;
                        break;
                case 'J': y +=  0x0E;
                        break;
                case 'L': y +=  0x38;
                        break;
                case 'n': y +=  0x54;
                        break;
                case 'o': y +=  0x5c;
                        break;
                case 'p': y +=  0x73;
                        break;
                case 'r': y +=  0x50;
                        break;
                case 'q': y +=  0x6F;
                        break;
                case 'S': y +=  0x6D;
                        break;
                case 'U': y +=  0x3e;
                        break;
                case 'u': y +=  0x1c;
                        break;
                case 'y': y +=  0x6e;
                        break;
                case ' ': y +=  0x00;
                        break;
                case '-': y +=  0x40;
                        break;
                case 't': y +=  0x78;
                        break;
                case '_': y +=  0x08;
                        break;
                case 'X': y +=  0x76;
                        break;
                case ',': y +=  0x10;
                        break;
                case '`': y +=  0x02;
                        break;
                case '\'': y +=  0x20;
                        break;
                case '\"': y +=  0x22;
                        break;
                case '=': y +=  0x48;
                        break;
                case '.':
                        y = y>>8;
                        y +=  0x80;
                        break;
                case '\0':
                        y = y>>8;
                        return y;
                        break;
                default:
                        break;
                }

                y = y<<8;

        }
        return y;
}


/**************************************************************************
 *  get_switch                                                            *
 *      takes the value of the switches on the board and passes it back   *
 *      into a pointer that will stay up to date                          *
 **************************************************************************/

void get_switch(int* target) {
        volatile int* sw_ptr = (int*)SWITCH_PTR;
        volatile int sw_val = *(sw_ptr);
        *(target) = sw_val;
}


/**************************************************************************
 *  ticker_segment                                                        *
 *    takes a string argument of les than 6 and will scroll the provided  *
 *    message accross the seven segment displays                          *
 **************************************************************************/

void ticker_segment(char* x, unsigned int speed) { //TODO: get working with arbitary string length
        unsigned int size = (sizeof(x)/8);
        long long unsigned int display = to_segment(x);
        for (unsigned int i = 0; i <= 6+size; i++) {
                if (i <= size) {
                        print_segment(display>>(8*(size-i)));
                } else {
                        print_segment(display<<(8*(i-size)));
                }
                board_timer(speed);
        }
}


/**************************************************************************
 * draw_pixel                                                             *
 *    defines the address space of the VGA buffer and writes a color to   *
 *    the given pixel                                                     *
 **************************************************************************/

void draw_pixel(int x, int y, int color) {
        volatile int* pixel_buffer = (volatile int*) (PIXEL_BUFFER + (y<<10) + (x<<1));
        *(pixel_buffer) = color;
}


/**************************************************************************
 *  clear_screen                                                          *
 *      fills the entire pixel buffer with the provided 16-bit color      *
 **************************************************************************/

void clear_screen(short color) {
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      draw_pixel(x,y,color);
    }
  }
}

/**************************************************************************
 *  line                                                                  *
 *    draws a line using Bresenham's line-drawing algorithm, which uses   *
 *    no multiplication or division.                                      *
 * written by David Brackeen                                              *
 * http://www.brackeen.com/home/vga/                                      *
 **************************************************************************/

void line(int x1, int y1, int x2, int y2, short color) {
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      draw_pixel(px,py,color);
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      draw_pixel(px,py,color);
    }
  }
}


/**************************************************************************
 *  clear_char                                                            *
 *      similar to clear_screen, this function will fill every space      *
 *        in the character buffer using any provided ASCII character      *
 **************************************************************************/

void clear_char(char a) {
  for (short x = 0; x < 79; x++) {
    for (short y = 0; y < 59; y++) {
      volatile char* character_buffer = (volatile char*) CHARACTER_BUFFER + (y<<7) + x;
      *(character_buffer) = a;
    }
  }
}



//all functions must come before this statement to preserve what little safety I've provided
#endif
