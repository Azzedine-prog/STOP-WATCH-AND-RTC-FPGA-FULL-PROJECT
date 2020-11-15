
/*
 *                                 project stop watch and synchronised clock;
 *    -azzedine lakhdar
 *        
 *
 *    le 14 novembre 2020
 *
 *    projet : chronometre(avec start pause arret) + montre en temps 
 *    reel(synchronisation avec le temps actuel)
 *    avec possibiliter de basculer entre mode chrono et mode montre.
 */

#include <stdio.h>
#include "sys/alt_irq.h"
#include "system.h"

    volatile int * key_ptr       = (int *) KEY_BASE;
    volatile int * sw_ptr        = (int *) SW_BASE;
    volatile int edge_capture_timer;
    volatile int edge_capture_key;
    volatile int edge_capture_sw;
    volatile int * timer_ptr = (int *) TIMER_0_BASE;
	volatile int * hex_ptr        = (int *) HEX_BASE;
	unsigned char	seven_seg_table[] =
				{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07,
			  	 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
	int k = 0;//secondes du chronometre
	int min = 0;//minutes du chronometre
	int a,b,c,d;//affichage seven segments pour chronometre
	int a1,b1,c1,d1;//affichage seven segments pour montre
	int press = 0;//edge captur des keys
	int basculer =0;//edge captur des switch
	int active;//statut des keys
	int changer =0;//statut des switch
	int l=0;//conserve la derniere valeur des secondes du chronometre
	int heuremontre = 17; // heures du montre reelle
	int minmontre = 43; // minutes du montre reelle
	int secmontre = 23; // secendes du montre reelle
	void handle_timer_interrupts(void* context, alt_u32 id)
		{
		    /* Reset the Button's edge capture register. */
			*(timer_ptr) = 0; 				// Clear the interrupt
			k++;
			secmontre++;

		}
	void handle_key_interrupts(void* context, alt_u32 id)
		{
		press = * (key_ptr +3) ;
		if (press&0x2){
			k=l;
					active=1;
				}
		else if (press&0x8){
					active=2;
						}
		else if (press&0x4){
							active=3;
								}
			* (key_ptr + 3) = 0;
		}
	void handle_sw_interrupts(void* context, alt_u32 id)
		{
		basculer = * (sw_ptr +3) ;
		if (basculer&0x2){
			changer = 2;
				}
		else if(basculer&0x1)
			{changer = 1;
			active =4;
			}
		* (sw_ptr + 3) = 0;
		}
	void init_timer()
		{
		    /* Recast the edge_capture pointer to match the alt_irq_register() function
		     * prototype. */
		    void* edge_capture_ptr = (void*) &edge_capture_timer;
		    /* set the interval timer period for scrolling the HEX displays */
			*(timer_ptr + 1) = 0x7;	// STOP = 0, START = 1, CONT = 1, ITO = 1
		    alt_irq_register( TIMER_0_IRQ, edge_capture_ptr,handle_timer_interrupts );
		}
	void init_key()
		{
			void* edge_capture_ptr_key = (void*) &edge_capture_key;
			* (key_ptr + 2) = 0xe;
		    alt_irq_register( KEY_IRQ, edge_capture_ptr_key,handle_key_interrupts );
		}
	void init_sw()
			{
				void* edge_capture_ptr_sw = (void*) &edge_capture_sw;
				* (sw_ptr + 2) = 0x3;
			    alt_irq_register( SW_IRQ, edge_capture_ptr_sw,handle_sw_interrupts );
			}
/* les methodes definis dans le projets */
	void start(){
			while(k>59){
				min++;
				k = 0;
			}

			 a=k%10;
		     b=(k/10)%10;
			 c=min%10;
			 d=(min/10)%10;
	}
	void startmontre(){
				if(secmontre>59){
					minmontre++;
					secmontre = 0;
				}
				else if (minmontre>59){
									heuremontre++;
									minmontre = 0;
								}
				 a1=minmontre%10;
			     b1=(minmontre/10)%10;
				 c1=heuremontre%10;
				 d1=(heuremontre/10)%10;
		}

int main()
{
  printf("Hello from Nios AZZEDINE YOUSSEF II!\n");
  init_timer();
  init_sw();
  init_key();
  while(1){
	  startmontre();
	  if(changer == 0){
		  startmontre(); // repet� afin de garantir que la montre toujours fonctionne
		  * hex_ptr = ((seven_seg_table[d]<<24)&0xFF000000) |((seven_seg_table[c]<<16)&0x00FF0000) |((seven_seg_table[b]<<8)&0x0000FF00) | (seven_seg_table[a]&0x000000FF);
	  }
	  else if(changer == 1){
		  startmontre();
	  * hex_ptr = ((seven_seg_table[d]<<24)&0xFF000000) |((seven_seg_table[c]<<16)&0x00FF0000) |((seven_seg_table[b]<<8)&0x0000FF00) | (seven_seg_table[a]&0x000000FF);
	  if(active == 1){
		  start();
		  startmontre();
		  * hex_ptr = ((seven_seg_table[d]<<24)&0xFF000000) |((seven_seg_table[c]<<16)&0x00FF0000) |((seven_seg_table[b]<<8)&0x0000FF00) | (seven_seg_table[a]&0x000000FF);
		  l=k;
	  }


	  else if(active == 2){
    	  startmontre();
	                    k=0;
	                    l=0;
	                    min=0;
	  					a=0;
	  					b=0;
	  					c=0;
	  					d=0;
 		  * hex_ptr = ((seven_seg_table[d]<<24)&0xFF000000) |((seven_seg_table[c]<<16)&0x00FF0000) |((seven_seg_table[b]<<8)&0x0000FF00) | (seven_seg_table[a]&0x000000FF);
      }
	  else if(active == 3){
		  startmontre();
	  		  * hex_ptr = ((seven_seg_table[d]<<24)&0xFF000000) |((seven_seg_table[c]<<16)&0x00FF0000) |((seven_seg_table[b]<<8)&0x0000FF00) | (seven_seg_table[a]&0x000000FF);
	  	  }
  }
	  else if (changer == 2 ) {
	   startmontre();
	  * hex_ptr = ((seven_seg_table[d1]<<24)&0xFF000000) |((seven_seg_table[c1]<<16)&0x00FF0000) |((seven_seg_table[b1]<<8)&0x0000FF00) | (seven_seg_table[a1]&0x000000FF);
  }

  }
  return 0;
}

