/**
 * @file uart_scanner.c
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Steffen Robertz
 */
 
/*! @brief  When a pin change is triggered by an alternating edge on a pin, this var counts up.*/
volatile scanner_result_t results[SCANNER_MAX_PIN_CNT] = {0};
volatile uint32_t shortest_lowtime;

volatile uint8_t port_b_old_in;
volatile uint8_t port_c_old_in;
volatile uint8_t pin_cnt=0;

/**
 * @brief Function for initialization of the UART scanner interrupts.
 */
static inline void initScan(uint8_t pin_num)
{
	/* init results struct */
	for(uint8_t i=0; i<pin_num; ++i){
		results[i].shortest_lowtime = 0xffff;
	}

	pin_cnt = pin_num;
	cli();
	
	PORTB.DIRCLR = 0xFF; //clear whole port
	if(pin_num >8){
		PORTC.DIRCLR = 0xFF;
	}
	
	/* enable interrupt 0 on all address ports */
	PORTB.INT0MASK = 0x0f;
	if(pin_num >4){
		PORTB.INT1MASK = 0xf0;
	}
	if(pin_num >8){
		PORTC.INT0MASK = 0x0f;
	}
	if(pin_num >12){
		PORTC.INT1MASK = 0xf0;
	}

	/* configure totem-pole on all address ports and sens both edges*/
	PORTCFG.MPCMASK = 0xFF;
	PORTB.PIN0CTRL = 0x0;	
	if(pin_num >8){
		PORTCFG.MPCMASK = 0xFF;
		PORTC.PIN0CTRL = 0x0;	
	}
	
	/* Set interrupt level to low for PORTB_INT0*/
	if(pin_num >4){
		PORTB.INTCTRL |= PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;
	}
	if(pin_num >8){
		PORTC.INTCTRL = PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;
	}

	//low level interrupts freigeben
    PMIC.CTRL |= PMIC_LOLVLEN_bm;
    sei();

	PORTD.DIRCLR = PIN0_bm;
	PORTD.INT0MASK = PIN0_bm;
	PORTD.INTCTRL |= PORT_INT0LVL_HI_gc;
	PORTD.PIN0CTRL |= PIN0_bm;
	shortest_lowtime = 0xffffffff;
}

/**
 * @brief Will test which of the 4 pins caused the interrupt and then increase the right result var
 */
static inline void count_change(volatile scanner_result_t* results, uint8_t port_value, uint8_t pin_mask)
{
	uint64_t curr_time = getTime() + 6;
	if (port_value & pin_mask) {
		uint64_t low_time = curr_time - results->last_change;
		results->time_low += low_time;
		if(low_time < results->shortest_lowtime){
			results->shortest_lowtime = low_time;
		}
	}else{
		results->time_high += curr_time - results->last_change;
	}
	results->pin_changes++;
	results->last_change = curr_time;
}

/**
 * @brief Interrupt Service Routine for A0-A3
 */
ISR(PORTB_INT0_vect){
	uint8_t new_in = PORTB.IN;
	for(uint8_t i=0; i<4; ++i ){
		if((pin_cnt >=i+1) && ((port_b_old_in ^ new_in) & (1<<i))){
			count_change(&results[i], new_in, (1<<i));
		}
	}				
	port_b_old_in = (new_in&0x0f)|(port_b_old_in&0xf0);
}

/**
 * @brief Interrupt Service Routine for A3-A7
 */
ISR(PORTB_INT1_vect){
	uint8_t new_in = PORTB.IN;
	for(uint8_t i=4; i<8; ++i ){
		if((pin_cnt >=i+1) && ((port_b_old_in ^ new_in) & (1<<i))){
			count_change(&results[i], new_in, (1<<i));
		}
	}
	port_b_old_in = (new_in&0xf0)|(port_b_old_in&0x0f);
}	

/**
 * @brief Interrupt Service Routine for A7-A11
 */
ISR(PORTC_INT0_vect){
	uint8_t new_in = PORTC.IN;
	for(uint8_t i=8; i<12; ++i ){
		if((pin_cnt >=i+1) && ((port_c_old_in ^ new_in) & (1<<i-8))){
			count_change(&results[i], new_in, (1<<i-8));
		}
	}				
	port_c_old_in = (new_in&0x0f)|(port_c_old_in&0xf0);
}

/**
 * @brief Interrupt Service Routine for A12-A15
 */
ISR(PORTC_INT1_vect){
	uint8_t new_in = PORTC.IN;
	for(uint8_t i=12; i<16; ++i ){
		if((pin_cnt >=i+1) && ((port_c_old_in ^ new_in) & (1<<i-8))){
			count_change(&results[i], new_in, (1<<i-8));
		}
	}
	port_c_old_in = (new_in&0xf0)|(port_c_old_in&0x0f);
}


static volatile inline scanner_result_t* getChanges(void){
	return results;
}

/**
 * @brief Interrupt Service Routine to detect the fastest pin-change time.
 *	This Interrupt is use to detect the baudrate by measuring the shortest low time.
 *	With enough samples one bit lenght will be measured.
 */
ISR(PORTD_INT0_vect){
	uint8_t in = PORTD.IN;
	uint64_t curr_time = getTime();
	static uint64_t last_change = 0;
	if(in & PIN0_bm){
		uint64_t low_time = curr_time - last_change;
		if(low_time < shortest_lowtime){
			shortest_lowtime = low_time;
		}
	}
	last_change = curr_time;
}

/**
 * @brief returns the shortest low period
 * @return	shortes lowtime
 */
static volatile inline uint32_t getShortestLowTime(void)
{
	return shortest_lowtime;
}
