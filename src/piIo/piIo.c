
//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013


// Access from ARM Running Linux

#include "piIo.h"



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


//#define BCM2708_PERI_BASE        0x20000000
#define BCM2708_PERI_BASE  0x3F000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) /* (*(gpio+13)&(1<<g)) */ // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock



// Access from ARM Running Linux

//#define BCM2708_PERI_BASE        0x20000000
#define BCM2708_PERI_BASE  0x3F000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)



//
// Set up a memory regions to access GPIO
//
void io_setup()
{
  /* open /dev/mem */
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
    printf("can't open /dev/mem \n");
    exit(-1);
  }

  /* mmap GPIO */
  gpio_map = mmap(
		  NULL,             //Any adddress in our space will do
		  BLOCK_SIZE,       //Map length
		  PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
		  MAP_SHARED,       //Shared with other processes
		  mem_fd,           //File to map
		  GPIO_BASE         //Offset to GPIO peripheral
		  );

  close(mem_fd); //No need to keep mem_fd open after mmap

  if (gpio_map == MAP_FAILED) {
    printf("mmap error %d\n", (int)(intptr_t)gpio_map);//errno also set!
    exit(-1);
  }

  // Always use volatile pointer!
  gpio = (volatile unsigned *)gpio_map;


} // setup_io

void io_pin_set(uint8_t g){
  *(gpio+7)=1<<g;
}


void io_pin_clr(uint8_t g){
  *(gpio+10)=1<<g;
}



void io_pin_setVal(uint8_t g,bool v){
  *(gpio+(v?7:10) )=1<<g;
}

bool io_pin_get(uint8_t g){
  return  (*(gpio+13)&(1<<g));
}

void io_pin_setDirIn(uint8_t g){
  *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3));
}

void io_pin_setDirOut(uint8_t g){
  *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3));
  *(gpio+((g)/10)) |=  (1<<(((g)%10)*3));
}

void io_bus_setVal(uint32_t mask,uint32_t val){
  *(gpio+7)=mask&val;
  *(gpio+10)=mask&~val;
}

uint32_t io_bus_get(){
  return *(gpio+13);
}
