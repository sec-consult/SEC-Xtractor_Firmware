/*****  JTAG Functionality from JTAGenum *****/
/*
 * Set the JTAG TAP state machine
 */
void jtagTapState(char jtagTapState[], int tck, int tms)
{
	while (*jtagTapState)
	{ // exit when string \0 terminator encountered
		if (jtagDelay)
			_delay_us(50);
		jtagWritePin(tck, 0);
		jtagWritePin(tms, *jtagTapState - '0'); // conv from ascii pattern
		jtagWritePin(tck, 1);				 // rising edge shifts in TMS
		*jtagTapState++;
	}
}

void jtagInitPins(int tck, int tms, int tdi, int ntrst, uint8_t pinslen)
{
	// default all to INPUT state
	for (int i = 0; i < pinslen; i++)
	{
		setPinMode(i, 0);
		// internal pullups default to logic 1:
		//		if (PULLUP) jtagWritePin(i, 1);
	}
	// TCK = output
	if (tck != IGNOREPIN)
		setPinMode(tck, 1);
	// TMS = output
	if (tms != IGNOREPIN)
		setPinMode(tms, 1);
	// tdi = output
	if (tdi != IGNOREPIN)
		setPinMode(tdi, 1);
	// ntrst = output, fixed to 1
	if (ntrst != IGNOREPIN)
	{
		setPinMode(ntrst, 1);
		jtagWritePin(ntrst, 1);
	}
}

static void jtagPulseTms(int tck, int tms, int s_tms)
{
	jtagWritePin(tck, 0);
	jtagWritePin(tms, s_tms);
	jtagWritePin(tck, 1);
}

static void jtagPulseTdi(int tck, int tdi, int s_tdi)
{
	if (jtagDelay)
		_delay_us(50);
	if (tck != IGNOREPIN)
		jtagWritePin(tck, 0);
	jtagWritePin(tdi, s_tdi);
	if (tck != IGNOREPIN)
		jtagWritePin(tck, 1);
}

uint8_t jtagPulseTdo(int tck, int tdo)
{
	uint8_t tdo_read;
	if (jtagDelay)
		_delay_us(50);
	jtagWritePin(tck, 0); // read in TDO on falling edge
	tdo_read = jtagReadPin(tdo);
	jtagWritePin(tck, 1);
	return tdo_read;
}
