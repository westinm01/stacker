/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: https://youtu.be/-EK-06-ae3k
 */
#include <avr/io.h>

#include "../header/timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
unsigned char pattern=0x81;
unsigned char level;
typedef struct _task{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

unsigned char oneDirection=1;
enum oneBlock{uno,dos,tres,quatro,cinco};

int oneTickSM(int state){
	switch(state){
		case uno:
			state=dos;
		break;
		case dos:
			if(oneDirection){
				state=uno;
			}
			else{
				state=tres;
			}
		break;
		case tres:
			if(oneDirection){
				state=dos;
			}
			else{
				state=quatro;
			}
		break;
		case quatro:
			if(oneDirection){
				state=tres;
			}
			else{
				state=cinco;
			}
		break;
		case cinco:
			state=quatro;
		break;
		default:
			state=tres;
		break;
	}
	switch(state){
		case uno:
			pattern=0x0F;
			oneDirection=0;
		break;
		case dos:
			pattern=0x17;
		break;
		case tres:
			pattern=0x1B;
		break;
		case quatro:
			pattern=0x1D;
		break;
		case cinco:
			pattern=0x1E;
			oneDirection=1;
		break;
		default:
		break;
	}
	PORTC=level;
	PORTD=pattern;
	return state;
}

//////////////////////////////////////////////
unsigned char twoDirection=1;
enum twoBlock{oneTwo,twoThree,threeFour,fourFive};
int twoTickSM(int state){
	switch(state){
		case oneTwo:
			state=twoThree;
		break;
		case twoThree:
			if(twoDirection){
				state=oneTwo;
			}
			else{
				state=threeFour;
			}
		break;
		case threeFour:
			if(twoDirection){
				state=twoThree;
			}
			else{
				state=fourFive;
			}
		break;
		case fourFive:
			state=threeFour;
		break;
		default:
			state=twoThree;
		break;
	}
	switch(state){
		case oneTwo:
			pattern=0x07;
			twoDirection=0;
		break;
		case twoThree:
			pattern=0x13;
		break;
		case threeFour:
			pattern=0x19;
		break;
		case fourFive:
			pattern=0x1C;
			twoDirection=1;
		break;
		default:
		break;
	}
	PORTC=level;
	PORTD=pattern;
	return state;
}
			


//////////////////////////////////////////////
unsigned char direction=1;
enum threeBlock{left,mid,right};
int threeTickSM(int state){
	switch(state){
		case left:
			state=mid;
			
		break;
		case mid:
			if(!direction){
				state=right;
			}
			else{
				state=left;
			}
		break;
		case right:
			state=mid;
			
		break;
		default:
			state=mid;
		break;

	}
	switch(state){
		case left:
			pattern=0x03;
			direction=0;
		break;
		case mid:
			pattern=0x11;
		break;
		case right:
			pattern=0x18;
			direction=1;
		break;
		default:
		break;
	}
	PORTD=pattern;
	
	//TimerFlag=0;
	//while(!TimerFlag);
	
	return state;
}
unsigned char gameOver;
unsigned char sz;
//unsigned char win;
//unsigned char lose;
unsigned char spd;
unsigned char tower []={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char story;
unsigned char pa0;
enum lockStates{wait,lock};
unsigned char lockedIn;
int lockTickSM(int state){
	switch(state){
		case wait:
			if(pa0){
				state=lock;
				if(story<8){
					level=(level>>1);
					tower[story]=pattern;
					//story++;
					spd=7*spd/8;
					lockedIn=0x01;
				}
			}
		break;
		case lock:
		if(!pa0){
			state=wait;
		}
		break;
		default:
		state=wait;
		break;
	}
	PORTC=level;
	return state;
}
enum displayPast{display};
int displayPastTickSM(int state){
	unsigned char pcd=0x80;
	switch(state){
		case display:
			
			for(unsigned char x=0;x<8;x++){
				if(x<story){
					PORTC=pcd;
					pcd=pcd>>1;
					PORTD=tower[x];
				}
				TimerFlag=0;
				while(!TimerFlag);
				PORTC=level;
				PORTD=pattern;
			}
		break;
		default:
		state=display;
		break;
	}
	PORTC=level;
	PORTD=pattern;
	return state;
}

enum loseBitStates{listen, loseBit};
int loseBitTickSM(int state){
	switch(state){
		case listen:
			if(lockedIn){
				
				if(story<1){
				}
				else{
					unsigned char tsm=tower[story-1];
					unsigned char chk=pattern&tsm;
					if(0){
					}
					else{
						state=loseBit;
						unsigned char mg=0x01;
						while(mg){
							if((chk&mg)!=(tsm&mg)){
								sz=sz-1;
							}
							mg=mg<<1;
						}
					}
					if(sz<=0x00){
						gameOver=0x01;
					}
					tower[story]=pattern|tsm;
				}
				
				lockedIn=0x00;
				story++;
			}
		break;
		case loseBit:
			state=listen;
		break;
		default:
		state=listen;
		break;
	}
	switch(state){
		case listen:
		break;
		case loseBit:
			//animation?   
		break;
		default:
		break;
	}
	if(story>=8 && !gameOver){
		PORTB=0x07;
	}
	return state;
	
}

//need to add Loss SM
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC=0xFF; PORTC=0x00;
	DDRD=0xFF; PORTC=0x00;
	DDRB=0xFF; PORTB=0x00;
	DDRA=0x00; PORTA=0xFF;
	
	TimerSet(1);
	TimerOn();
	PORTC=0x80;
	level=0x80;

	static task task1, task2, task3,task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks=sizeof(tasks)/sizeof(task*);

	const char start=-1;
	task1.state=start;
	task1.period=25;//translates to 200
	task1.elapsedTime=task1.period;
	task1.TickFct=threeTickSM;
	spd=25;
	task2.state=start;
	task2.period=2;// translates to 16
	task2.elapsedTime=task2.period;
	task2.TickFct=lockTickSM;
	
	task3.state=start;
	task3.period=1;
	task3.elapsedTime=task3.period;
	task4.TickFct=displayPastTickSM;

	task4.state=start;
	task4.period=1;
	task4.elapsedTime=task3.period;
	task3.TickFct=loseBitTickSM;
	unsigned short i;
	story=0;
	sz=0x03;
	gameOver=0;
	lockedIn=0;
    /* Insert your solution below */
    while (1) {
	if(sz==0x03){
		task1.TickFct=threeTickSM;
	}
	else if(sz==0x02){
		task1.TickFct=twoTickSM;
	}
	else if(sz==0x01){
		task1.TickFct=oneTickSM;
	}
	task1.period=spd;
	pa0=(~PINA)&0x01;
	PORTC=level;
	PORTD=pattern;
	
	if((~PINA>>1)&0x01 || gameOver){//resets the game
		level=0x80;
		for(unsigned tow=0;tow<story;tow++){
			tower[tow]=0x00;
		}
		story=0;
		spd=25;
		sz=0x03;
		gameOver=0;
		PORTB=0x00;
		
	}
	for(i=0;i<numTasks;i++){
   		 if(tasks[i]->elapsedTime>=tasks[i]->period){
	    		tasks[i]->state=tasks[i]->TickFct(tasks[i]->state);
	    		tasks[i]->elapsedTime=0;
    		}
		 tasks[i]->elapsedTime+=1;
	}
	while(!TimerFlag);
	TimerFlag=0;
    }
    return 1;
}
