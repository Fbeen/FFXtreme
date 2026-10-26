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

/* USART3 global interrupt. */
#define irq_st_kbd 39
void IRQ_39(void) __attribute__((alias("IRQ_st_kbd")));

/* Left Control and Alternate keys: both must be held for our hotkeys. */
#define ST_CTRL (1u << 1)
#define ST_ALT  (1u << 2)
static uint8_t stKeyboardState; // keep track on the left Control and Alternate keys

/* Bytes received from the keyboard (IKBD). Filled by the interrupt handler, so
 * that no byte is lost while the main loop is busy (a byte takes only 1.28ms
 * at 7812.5 baud). The main loop can stall for 250ms (reset pulse after a
 * hotkey), which is ~195 bytes of mouse reports: hence 256 entries. */
static uint8_t kbd_ring[256];
static volatile uint8_t kbd_cons, kbd_prod;
static volatile bool_t kbd_overflow;
#define KBD_MASK(x) ((x) & (ARRAY_SIZE(kbd_ring)-1))

static void IRQ_st_kbd(void)
{
    uint8_t c;
    uint32_t sr;

    /* Reading SR then DR clears RXNE and any overrun error. An overrun
     * (eg. CPU stalled by a Flash erase) means a byte was lost. */
    sr = usart3->sr;
    c = usart3->dr;
    if (sr & USART_SR_ORE)
        kbd_overflow = TRUE;

    /* 8-bit indices: one entry stays unused to tell full from empty. */
    if ((uint8_t)(kbd_prod - kbd_cons) < ARRAY_SIZE(kbd_ring)-1)
        kbd_ring[KBD_MASK(kbd_prod++)] = c;
    else
        kbd_overflow = TRUE;
}

void st_init(void)
{
    /* Turn on the clocks. */
    rcc->apb1enr |= RCC_APB1ENR_USART3EN;

    /* Enable RX pin (PB11) as input. */
    gpio_configure_pin(gpiob, 11, GPI_pull_up);

    /* BAUD, 8n1. */
    usart3->brr = 4608; // 36.000.000 / 7812.5
    usart3->cr1 = (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);

    IRQx_set_prio(irq_st_kbd, STKBD_IRQ_PRI);
    IRQx_enable(irq_st_kbd);
}

/* Number of data bytes following an IKBD packet header 0xF6-0xFF. These are
 * mouse, joystick, clock and status reports, not key codes. */
static const uint8_t ikbd_packet_len[] = {
    7,          /* F6: status report */
    5,          /* F7: absolute mouse position */
    2, 2, 2, 2, /* F8-FB: relative mouse movement */
    6,          /* FC: time-of-day */
    2,          /* FD: joystick report */
    1, 1,       /* FE-FF: joystick 0/1 event */
};

/* Returns a key make code while Control+Alternate are held, any key release
 * code (so that a held key is never left latched), or FALSE. */
uint8_t st_check(void)
{
    static uint8_t skip;
    uint8_t key;

    /* Bytes were lost, so we may be out of step with the packets and may
     * have missed key releases. Drop everything and forget held keys. */
    if (kbd_overflow) {
        kbd_overflow = FALSE;
        kbd_cons = kbd_prod;
        skip = 0;
        stKeyboardState = 0;
        return 0x80; /* a key release, so the caller clears its key state */
    }

    if (kbd_cons == kbd_prod)
        return FALSE;
    key = kbd_ring[KBD_MASK(kbd_cons++)];

    /* Skip the data bytes of mouse/joystick packets: they are no key codes. */
    if (skip) {
        skip--;
        return FALSE;
    }
    if (key >= 0xf6) {
        skip = ikbd_packet_len[key - 0xf6];
        return FALSE;
    }

    switch (key) {
    case 29: // Control pressed
        stKeyboardState |= ST_CTRL;
        return FALSE;
    case 157: // Control released
        stKeyboardState &= ~ST_CTRL;
        break;
    case 56: // Alternate pressed
        stKeyboardState |= ST_ALT;
        return FALSE;
    case 184: // Alternate released
        stKeyboardState &= ~ST_ALT;
        break;
    }

    // always report key releases, so the caller can clear its key state
    if (key & 0x80)
        return key;

    // return the pressed key only when Control and Alternate are held
    if ((stKeyboardState & (ST_CTRL | ST_ALT)) != (ST_CTRL | ST_ALT))
        return FALSE;

    return key;
}

bool_t readSTmode(void)
{
    // gpio_configure_pin(gpiob, 13, GPI_pull_up);
    delay_us(5);

    return 1; // !gpio_read_pin(gpiob, 13);
}
