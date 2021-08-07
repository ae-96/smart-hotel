#include "stdint.h"
#include <stdlib.h>
#include "tm4c123gh6pm.h"
void SystemInit(){}
void portf_init(void);
void portb_init(void);
void delay(void);
void uart7_init(void);
char recieve_char_uart(void);
int* first_time_config(int* no_of_rooms);
void keybad_interface_room4(int** result);
void keybad_room4(int** result);    
int main (){
	uart7_init();  // uart intialize
	int no_of_rooms;
	int* rooms = first_time_config(&no_of_rooms);  // returns a dynamic array of room numbers
	portf_init(); // intialize portf for the lock
	portb_init();  // initializr port b for the keybad
	int* status = calloc(no_of_rooms, sizeof(int)); // by default a status of each room is 0
	int* pass = malloc(no_of_rooms* sizeof(int)); 
	int current_pass;
	int* ptr_current_pass=&current_pass;
	int** pptr_current_pass=&ptr_current_pass;
	while(1){  // main loop 
		ptr_current_pass=&current_pass;
		keybad_room4(pptr_current_pass);/*we loop forever and this function will change the 
		  pointer ptr_current_pass to null untill the guest enter  any number.will wait 
		untill the user enter the whole password and set it in the vatiable current_pass */
		if(ptr_current_pass!=NULL){   // the user entered a password on keybab for room4 
			int room4_index;
			for(room4_index=0;room4_index<no_of_rooms;room4_index=room4_index+1){  
				/* search for  room 4 index on the dynamic array*/
				if(rooms[room4_index]==4) break;
			}
			if((status[room4_index]==1) & (pass[room4_index]==current_pass)){ /* open 
	               the lock if the room status is 1 and the password match the entered pass */
				GPIO_PORTF_DATA_R |= 0x2;
				delay();
				GPIO_PORTF_DATA_R &= ~(0x2);
			}
		}
		if((UART0_FR_R&0x10)==0){ // the condition will be true if the fifo is not empty
			char current_char;
			int room_number=0; 
			int j=0;
			int arr[15]; // if enter 85P1234S1 
			while(1){  /* at first iteration store 8 at arr[0],j=0
				            at second iteration store 5 at arr[1],j=1
				            at third iteration break,j=1 */
				current_char=recieve_char_uart();  // recieve from uart
				if( current_char=='D' | current_char=='S' | current_char=='P' ) break;
				int temp = current_char - '0';  // store as int 
				arr[j]=temp;
				j=j+1;
			}
			for(int k=1;j!=-1;j=j-1) {  /* at first iteration,j=1,k=1,room_number=0+1*5=5
				                           at second iteration,j=0,room_number=5+10*8=85
				                           at third iteration,j=-1,condition is false */
				room_number =room_number+ arr[j]*k;
				k=k*10;
			}
			int room_index;
			for(room_index=0;room_index<no_of_rooms;room_index=room_index+1){
				if(rooms[room_index]==room_number) break;
			}
			while(1){  // before entering this loop we already have a current_char
				if( current_char=='D') break;// loop till recive D (end of the command)
				else if( current_char=='S')  {  
				     status[room_index]= recieve_char_uart() - '0'; 
                                    int room4_index;
                                    for(room4_index=0;room4_index<no_of_rooms;room4_index=room4_index+1){ 
                                           /* search for  room 4 index on the dynamic array*/
                                           if(rooms[room4_index]==4) break;
                                    }
                                    if(status[room4_index]== 2){ /* open 
                                     the lock if the room status is 2  */
                                          GPIO_PORTF_DATA_R |= 0x2;
                                          delay();
                                          GPIO_PORTF_DATA_R &= ~(0x2);
                                    }
				      current_char=recieve_char_uart() ; // recieve new char
				}
				else if( current_char=='P'){
					int newpass=0;
					int j=1000;
					while(1){  // if the pass was 1234
						current_char=recieve_char_uart(); 
						if( current_char=='D' | current_char=='S' | current_char=='P' ) break;
						int temp = current_char - '0'; // store as int
						newpass = newpass + j*temp; /* 
                                                 first it j=1000 newpass=0+1000*1=1000
                                                second it j=100 newpass=1000+100*2=1200 
                                                third it j=10 newpass=1200+10*3=1230
                                               fourth it j=1 newpass=1230+1*4=1234 */
						j=j/10;
					}
					pass[room_index]= newpass;
				}
			}
		}
	}
}
void portf_init(void){
	SYSCTL_RCGCGPIO_R = 0x20; // enable clock for port F
	while((SYSCTL_PRGPIO_R&0x20)==0){} // waiting for the clock to be enabled 
	GPIO_PORTF_LOCK_R=0x4c4f434b;  // set the magic value to unlock commit reg
	GPIO_PORTF_CR_R=0x1f; // alow value change for the first 5 bits 
	GPIO_PORTF_AFSEL_R=0;  // use as gpio and disable alternative functions
	GPIO_PORTF_PCTL_R=0; // no alternative functions to set 
	GPIO_PORTF_AMSEL_R=0; // disable analog
	GPIO_PORTF_DIR_R=0x0E; // set pf1,pf2,pf3 as output 
	GPIO_PORTF_DEN_R=0x1f; // set the first 5 bits as digital
	GPIO_PORTF_PUR_R=0x11; // add pull up resistor to the switches "not usable"
}
void portb_init(void){
	SYSCTL_RCGCGPIO_R = 0x02; // enable clock for port B
	while((SYSCTL_PRGPIO_R&0x02)==0){} // waiting for the clock to be enabled 
	GPIO_PORTB_LOCK_R=0x4c4f434b;  // set the magic value to unlock commit reg
	GPIO_PORTB_CR_R=0x7f; // alow value change for the first 7 bits 
	GPIO_PORTB_AFSEL_R=0;  // use as gpio and disable alternative functions
	GPIO_PORTB_PCTL_R=0; // no alternative functions to set 
	GPIO_PORTB_AMSEL_R=0; // disable analog
	GPIO_PORTB_DIR_R=0x0f; // set pb0,pb1,pb2,pb3 as output and pb4,pb5,pb6 as input
	GPIO_PORTB_DEN_R=0x7f; // set the first 7 bits as digital
	GPIO_PORTB_PDR_R=0x70; // add pull down res to the input bits 
}
void delay(void){
	int i;
	for(i=0;i<4000;i=i+1);
}
void uart7_init(void){
	SYSCTL_RCGCGPIO_R |= 0x10; // enable clock for port E
	while((SYSCTL_PRGPIO_R&0x10)==0) // waiting for the clock to be enabled 
	SYSCTL_RCGCUART_R |= 0x80; // set bit7 to 1 to enable clock for uart7
	delay();
	UART7_CTL_R&=~0x01; //put 0 in bit0 to disable uart7 from the control reg to continue the configuration
	UART7_IBRD_R= 43; // assume PC(throw USB 3.0) set boud rate to max "155 200" and tiva-c clock is 80MHZ
	UART7_FBRD_R=26;
	UART7_LCRH_R=0x70; // set bit4 to 1 to enable fifo buffer , set bit5 and bit6 to 11 to choose 8bits
	UART7_CTL_R=0x301; //set bit0 to 1 to enable uart , set bit8 and bit9 to 11 to enable transmit and recieve  
	GPIO_PORTE_AFSEL_R |= 0x03; // use E0 and E1 as alternative function  
	GPIO_PORTE_PCTL_R = (GPIO_PORTF_PCTL_R&~0xff)+0x11; // use E0 and E1 as uart
	GPIO_PORTE_AMSEL_R &= ~0x03; // set bit0 and bit1 to 00 to disable analog
	GPIO_PORTE_DEN_R |= 0x03; // set bit0 and bit1 to 11 to enable digital
}
char recieve_char_uart(void){
	while((UART0_FR_R&0x10)!=0){} // make sure fifo is not empty
	return ((char) UART0_DR_R&0xff); //need only first 8 bit
}
int* first_time_config(int* no_of_rooms){ 
	int i =0;
	int arr[15];
	*no_of_rooms=0;
	while(1){
		char x=recieve_char_uart();
		if(x=='R') break;
		int temp=x-'0';
		arr[i]=temp;
		i=i+1;
	}
	for(int j=1;i!=-1;i=i-1) {
		*no_of_rooms =*no_of_rooms+ arr[i]*j;
		j=j*10;
	}
	int* rooms =malloc(*no_of_rooms* sizeof(int));  
	for(i=0;i<*no_of_rooms;i=i+1){
		int j=0;
		int arr[15];
		int room_number=0;
		while(1){
			char x=recieve_char_uart();
			if(x=='R' || x=='D') break;
			int temp=x-'0';
			arr[j]=temp;
			j=j+1;
		}
		for(int k=1;j!=-1;j=j-1) {
			room_number =room_number + arr[j]*k;
			k=k*10;
	  }
		rooms[i]=room_number;
	}
	return rooms;
}	
void keybad_interface_room4(int** result){
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&~0xf) + 0x1;
	if( (GPIO_PORTB_DATA_R & 0x10) !=0)  {**result=1; while( (GPIO_PORTB_DATA_R & 0x10) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x20) !=0)  {**result=2; while( (GPIO_PORTB_DATA_R & 0x20) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x40) !=0)  {**result=3; while( (GPIO_PORTB_DATA_R & 0x40) !=0){} return;}
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&~0xf) + 0x2;
	if( (GPIO_PORTB_DATA_R & 0x10) !=0)  {**result=4; while( (GPIO_PORTB_DATA_R & 0x10) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x20) !=0)  {**result=5; while( (GPIO_PORTB_DATA_R & 0x20) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x40) !=0)  {**result=6; while( (GPIO_PORTB_DATA_R & 0x40) !=0){} return;}
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&~0xf) + 0x4;
	if( (GPIO_PORTB_DATA_R & 0x10) !=0)  {**result=7; while( (GPIO_PORTB_DATA_R & 0x10) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x20) !=0)  {**result=8; while( (GPIO_PORTB_DATA_R & 0x20) !=0){} return;}
	if( (GPIO_PORTB_DATA_R & 0x40) !=0)  {**result=9; while( (GPIO_PORTB_DATA_R & 0x40) !=0){} return;}
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R&~0xf) + 0x8;
	if( (GPIO_PORTB_DATA_R & 0x20) !=0)  {**result=0; while( (GPIO_PORTB_DATA_R & 0x20) !=0){} return;}
	*result= NULL;
}
void keybad_room4(int** result){
	keybad_interface_room4(result);
	if(*result==NULL) return;
	**result=**result*1000;
	int j=100;
	for(int i=0; i<3 ; i=i+1){
		int temp ;
		int* temp_ptr=&temp;
		int** p_temp_ptr=&temp_ptr;
		while(1){
			temp_ptr=&temp;
			keybad_interface_room4(p_temp_ptr);
			if(*temp_ptr!=NULL){ **result=**result+j*temp; break;}
		}
		j=j/10;
	}
}
