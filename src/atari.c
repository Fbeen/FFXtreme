/*
 * atari.c
 * 
 * Inspect the Atari ST keyboard serial protocol and remember key presses.
 * 
 * Written by Frank Beentjes <frankbeen@gmail.com> & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

/* Atari keyboard:
 *  B11: Keyboard RX
 */

uint8_t stKeyboardState = 0; // keep track on the left Control and Alternate keys

void st_init(void)
{
    /* Turn on the clocks. */
    rcc->apb1enr |= RCC_APB1ENR_USART3EN;

    /* Enable RX pin (PB11) as input. */
    gpio_configure_pin(gpiob, 11, GPI_pull_up);

    /* BAUD, 8n1. */
    usart3->brr = 4608; // 36.000.000 / 7812.5
    usart3->cr1 = (USART_CR1_UE | USART_CR1_RE /* | USART_CR1_RXNEIE */ );
}

uint8_t st_check(void) 
{
    uint8_t key;
    // char msg[10];
    
    // Check if RXNE (Read data register not empty) is set
    if(usart3->sr & USART_SR_RXNE) {
    
        key = usart3->dr;
        
        // snprintf(msg, 10, "code: %u", key);
        // printk(msg);
        
	switch(key) {
	case 29: // Control pressed
		stKeyboardState |= 1 << 1;
		break;
	case 157: // Control released
		stKeyboardState &= ~(1 << 1);
		break;
	case 56: // Alternate pressed
		stKeyboardState |= 1 << 2;
		break;
	case 184: // Alternate released
		stKeyboardState &= ~(1 << 2);
		break;
	}

	// return the pressed key only when Control and Alternate are held
	if(stKeyboardState < 3) {
		return FALSE;
	}
	
	return key;

    }
    
    return FALSE;
}

bool_t readSTmode(void) 
{
    // gpio_configure_pin(gpiob, 13, GPI_pull_up);
    delay_us(5);
    
    return 1; // !gpio_read_pin(gpiob, 13);
}


