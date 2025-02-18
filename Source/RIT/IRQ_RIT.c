/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h" 

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
//Button variable 
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
extern uint8_t current_direction;// Direzione corrente (0=Nessuna, 1=UP, 2=DOWN, 3=LEFT, 4=RIGHT)
extern int game_running; //1 running 0 pause
extern uint8_t game_state; // 0=Gicando, 1=Victory 2=GameOver

void RIT_IRQHandler (void)
{		
//Joystick variable 	
	static int j_up=0;
	static int j_down=0;	
	static int j_left = 0;
	static int j_right = 0; 
	static int j_select = 0;
	

	if(game_running){
/*************************Joytick UP***************************/
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick UP pressed */
		j_up++;
		switch(j_up){
			case 1://si triggera dopo 50ms
			//code here
			if(current_direction!=1){
				current_direction=1;
			}
			break;
			case 60:	//3sec = 3000ms/50ms = 60
				break;
			default:
			//continuo a tenere il joistick verso l'alto(per aumento velocità dopo tot secondi)
			// es ogni due 4s secondi rifai azione
				/*if(j_up%8 ==0){
					value++;
					LED_Out(value);
				}*/
				break;
		}
	}
	else{
			j_up=0;
	}
	
/*************************Joytick DOWN***************************/
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick DOWN pressed */
		j_down++;
		switch(j_down){
			case 1://si triggera dopo 50ms
			//code here
		if(current_direction!=2){
				current_direction=2;
			}
			break;
			case 60:	//3sec = 3000ms/50ms = 60
				break;
			default:
				//aumento velocità
			
				break;
		}
	}
	else{
			j_down=0;
	}
	
/*************************Joytick LEFT***************************/
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
			/* Joytick LEFT pressed */
			j_left++;
			switch(j_left){
				case 1://si triggera dopo 50ms
				//code here
				if(current_direction!=3){
				current_direction=3;
			}
				break;
				case 60:	//3sec = 3000ms/50ms = 60
					break;
				default:
					//aumento velocità
					
					break;
			}
		}
		else{
				j_left=0;
		}	
	
/*************************Joytick RIGHT***************************/		
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick RIGHT pressed */
		j_right++;
		switch(j_right){
			case 1://si triggera dopo 50ms
		//code here
			if(current_direction!=4){
				current_direction=4;
			}
			break;
			case 60:	//3sec = 3000ms/50ms = 60
				break;
			default:
			//aumento velocità
				break;
		}
	}
	else{
			j_right=0;
	}

/*************************Joytick SELECT***************************/
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
			/* Joytick SELECT pressed */
			j_select++;
			switch(j_select){
				case 2://si triggera dopo 100ms
			//code here
				break;
				case 60:	//3sec = 3000ms/50ms = 60
					break;
				default:
		
					break;
			}
		}
		else{
				j_select=0;
		}
	}
	
/*************************INT0***************************/
if(down_0 !=0){
	down_0++;
	if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
		switch(down_0){
			case 2:
				// scrivi qua
			//controlla che il gioco non sia terminato
			if(game_state==0){
	     game_running = !game_running; //Cambia lo stato del gioco(0 pausa, 1 running)
       if (game_running) {
        // Elimina la scritta pausa
				GUI_Text(102, 150, (unsigned char*)"     ", Black, Black);
         } else {
        // Mostra la scritta pausa
				GUI_Text(102, 150, (unsigned char*)"PAUSE", White, Black);
            }
				}
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_0=0;			
		NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
	}
} // end INT0

/*************************KEY1***************************/
if(down_1 !=0){
	down_1++;
	if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		switch(down_1){
			case 2:
				// scrivi qua
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_1=0;			
		NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
	}
} // end KEY1

/*************************KEY2***************************/
if(down_2 !=0){
	down_2++;
	if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
		switch(down_2){
			case 2:
			// scrivi qua
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_2=0;		
		NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
	}
} // end KEY2
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
