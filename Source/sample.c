/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT buttons and manage the bouncing effect
 *        	- key1 switches on the led at the left of the current led on, 
 *					- it implements a circular led effect. 	
  * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "GLCD/GLCD.h"
#include "joystick/joystick.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
//mapping dei pixel dell display in scala al fine di disegnare e gestire il labirinto (0 vuoto, 1 muro , 2 pillola, 3 power pillola, 4 Pac_Man) 
volatile uint8_t MAZE[23][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,1,1,2,2,2,2,2,2,2,2,1,1,2,1,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,2,1,1,1,2,2,2,2,1,1,1,2,2,1,2,1},
    {1,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,1},
		{1,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,1},
    {1,2,1,2,2,1,2,1,1,1,1,1,1,2,1,2,2,1,2,1},
    {2,2,2,2,2,2,2,1,0,0,0,0,1,2,2,2,2,2,2,2},
    {1,2,2,2,2,1,2,1,0,0,0,0,1,2,1,2,2,2,2,1},
    {1,2,2,2,2,1,2,1,1,1,1,1,1,2,1,2,2,2,2,1},
    {1,2,2,2,2,1,2,2,2,2,2,2,2,2,1,2,2,2,2,1},
    {1,2,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,2,1},
    {1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1},
    {1,2,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,2,1},
    {1,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};
// Stato di Pac-Man

volatile uint8_t pacman_row = 19;   // Posizione iniziale riga
volatile uint8_t pacman_col = 1;   // Posizione iniziale colonna
volatile uint8_t current_direction = 0; // Direzione corrente (0=Nessuna, 1=UP, 2=DOWN, 3=LEFT, 4=RIGHT)

// Punteggio e vite
volatile uint32_t score=0; //Puntrggio iniziale
volatile uint8_t lives=1; //Vite iniziali

//Flag game running
volatile int game_running= 0; //1 running 0 pause

volatile uint8_t countdown=60; //Timer in secondi
volatile uint8_t remaining_pills=0; // pillole rimanenti
volatile uint8_t game_state=0; // 0=Gicando, 1=Victory 2=GameOver

void count_initial_pills(void){
int row;
int col;
remaining_pills=0;
	for (row=0; row<23; row++){
		for (col=0; col<20; col++){
			if(MAZE[row][col]== 2 || MAZE[row][col]== 3){
				remaining_pills++;
			}
		
		}
	}
}



	


/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LCD_Initialization();									/* DISPLAY Initialization */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_timer(0,0,0,3,0x004C4B42);       /* Timer 0 aggironamento posizione 52ms MR0*/
	init_timer(1,0,0,3,0x005F5E10);				/* Timer 1 countdown Initialization 1s MR0*/
	init_timer(2,0,0,3,0x00BEBC20);				/* Timer 2 power pillole Initialization 2s MR0*/
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	enable_RIT();													/* enable RIT to count 50ms				 */	
	enable_timer(0);
	enable_timer(1);
	enable_timer(2);
	LCD_Clear(Black);
	count_initial_pills();                 /*conta le pillole totali */
	draw_maze(&MAZE);												/* disegna labirinto*/
	display_game_stats();										/*diesgna score*/
	display_countdown();
	GUI_Text(102, 150, (unsigned char*)"PAUSE", White, Black);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}




