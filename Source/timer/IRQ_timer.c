/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char led_value;					/* defined in funct_led								*/

unsigned char ledval = 0xA5;
extern uint8_t pacman_row;
extern uint8_t pacman_col;
extern uint8_t current_direction;// Direzione corrente (0=Nessuna, 1=UP, 2=DOWN, 3=LEFT, 4=RIGHT)
extern uint32_t score; //Puntrggio iniziale
extern uint8_t lives; //Vite iniziali
uint32_t next_extra_life_score = 1000;
extern uint8_t MAZE[23][20];
extern int game_running;
extern uint8_t remaining_pills;
extern uint8_t game_state;

/*funzione per far muovere pacman, aggiornando la matrice e la posizione a schermo(vine richiamata in Timer0) */
void upadate_pacman_position(void){
	if(!game_running || game_state==1 || game_state==2 )return;
	
	int next_row= pacman_row;
	int next_col=	pacman_col;
	switch(current_direction){
				case 1: // UP
            next_row--;
            break;
        case 2: // DOWN
            next_row++;
            break;
        case 3: // LEFT
            next_col--;
            break;
        case 4: // RIGHT
            next_col++;
            break;
	}
	//Gestione tp
	if (pacman_row==10){
		
			if (next_col < 0 && pacman_col == 0) {  
				next_col = 19; // Teleporta a destra
        } else if (next_col > 19 && pacman_col == 19) {
            next_col = 0;  // Teleporta a sinistra
        }
	}
	//Controlla se la posizione è valida o meno
	if (next_row >=0 && next_row < 22 && next_col >=0 && next_col<20){
		if(MAZE[next_row][next_col]!=1){
			//Non è un muro 
			
			if(MAZE[next_row][next_col] == 2 || MAZE[next_row][next_col] == 3) {
        if(MAZE[next_row][next_col] == 2) {
            score += 10;  // Standard pill score
        } else {
            score += 50;  // Power pill score
        }
        MAZE[pacman_row][pacman_col] = 0;  // Rimuovi pillola
        remaining_pills--;
        
        display_game_stats();
				
			}
			//pulisci la posizione vecchia
			clear_cell(pacman_row, pacman_col);
			
			
			//Vita extra		
				if (score>=next_extra_life_score){
						lives++;
						next_extra_life_score+=next_extra_life_score;
						display_game_stats();
				}
				
						
			// Aggiorna posizione
			pacman_row = next_row;
      pacman_col = next_col;
      MAZE[pacman_row][pacman_col] = 4;
      draw_cell(pacman_row, pacman_col, 4);
				
				// Check for victory
        if(remaining_pills == 0) {
            game_state = 1;  // Victory
            game_running = 0;
						display_game_over();
        }
				
		}else{
		
			current_direction=0; //fermati (muro)
		}
	
	}
	
}

void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		upadate_pacman_position();
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern uint8_t countdown;

void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) // MR0
	{ 
		// your code
		if(game_running && game_state == 0) {  // Se il gioco è in corso e non è game over
			countdown--;
			display_countdown();
			
			if(countdown == 0) {
				game_state = 2;  // Game Over
				game_running = 0;
				display_game_over();
			}
		}
		
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		// your code
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

static uint32_t sRandomSeed = 0;
static uint8_t gGeneratedPills = 0; //costante per assicurarsi che il numero totale di power pillole generate non sia mai >6
volatile int valid_positions[23*20][2];
//inizializza il seed iniziale(basandoti sul valore del timer e la posizione di Pac_Man)
void init_random_seed(uint8_t pacman_row, uint8_t pacman_col){
sRandomSeed = (LPC_TIM2->TC << 16) ^ ((pacman_row << 8) + pacman_col);
}
//generatore lineare congruenziale
static uint32_t my_rand(void) {
  sRandomSeed = (sRandomSeed * 1103515245U + 12345U) & 0x7FFFFFFF;
  return sRandomSeed;
}
//la funzione verra chiamata più volte con una data frequenza
void generate_power_pill(uint8_t pacman_row, uint8_t pacman_col) {
  // Aggiorna il seed 
  init_random_seed(pacman_row, pacman_col);

  // Se abbiamo già 6 power pills, niente
  if(gGeneratedPills >= 6) return;
	int r;
	int c;
  int count_positions = 0;
// Trova tutte le posizioni per pillole standard 
	for( r = 0; r < 23; r++){
    for( c = 0; c < 20; c++){
      if(MAZE[r][c] == 2) { // pillola standard
        valid_positions[count_positions][0] = r;
        valid_positions[count_positions][1] = c;
        count_positions++;
      }
    }
  }

  // Quante pillole possiamo ancora aggiungere?
// Se esiste almeno una posizione disponibile
  if(count_positions > 0) {
    int chosen = my_rand() % count_positions;
    int row = valid_positions[chosen][0];
    int col = valid_positions[chosen][1];
    // Converti la pillola standard in power pill (valore 3)
    MAZE[row][col] = 3;
    draw_cell(row, col, 3);
    gGeneratedPills++;
  }
}


void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1) // MR0
	{ 
		// your code
		if(game_running && game_state == 0){
		generate_power_pill(pacman_row,pacman_col);	
		}
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		// your code
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	
		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 
	return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
	if(LPC_TIM3->IR & 1) // MR0
	{ 
		// your code
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		// your code
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	} 
	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

/*
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}*/