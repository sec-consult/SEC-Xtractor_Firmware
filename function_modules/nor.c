/**
 * @brief Function to dump a NOR flash memory.
 * The content is directly redirected to the main UART in hexadecimal and can be recovered with "xxd -r".
 * @param endianess This varable indicates which endianess type should be set on the outputted firmware.
 * @param word_mode If set to one Word mode will be used during transfers, otherwise Byte Mode will be used
 */
static inline void norDump(uint8_t endianess, uint8_t word_mode)
{
	DEBUG("Entering norDump");
	uartprintf("endiness:%u, word: %u"NL, endianess, word_mode);

	/* NOR flash init */
	portOut(&NOR_CTRL_PORT, NOR_CE | NOR_OE | NOR_RESET);
	portSetDirection(&NOR_CTRL_PORT, ~(NOR_RYBY | NOR_NC) & 0xFF); // set CE#, OE# and WE# to output, RY/BY# input

	/* set all address pins to output */
	portSetDirection(&NOR_ADDR_PORT0, 0xFF);
	portSetDirection(&NOR_ADDR_PORT1, 0xFF);
	portSetDirection(&NOR_ADDR_PORT2, 0xFF);
	portSetDirection(&NOR_ADDR_PORT3, 0xFF);

	/* set all data pins to input */
	portOut(&NOR_DATA_PORT0, 0x00);
	portOut(&NOR_DATA_PORT1, 0x00);
	portSetDirection(&NOR_DATA_PORT0, 0x00);
	portSetDirection(&NOR_DATA_PORT1, 0x00);

	norResetFlash();

	dumpStart();


	for (uint64_t address = 0x0; address < NOR_READ_LENGTH && !isOperationCanceled(); ++address)
	{
		uint16_t data;
		norStartReadData();
		if(word_mode){
			norModeWord();
			data = norRead16DataBits(address);
			norEndReadData();
			uint8_t data_byte1 = (uint8_t)((data & 0xFF00) >> 8);
			uint8_t data_byte2 = (uint8_t)(data & 0x00FF);
			if (endianess & 0x02 == 0x02)
			{
				dumpByte(data_byte2);
				dumpByte(data_byte1);
			}
			else
			{
				dumpByte(data_byte1);
				dumpByte(data_byte2);
			}
		}else{
			norModeByte();
			data = norRead8DataBits(address);
			norEndReadData();
			dumpByte((uint8_t) data);
		}
	}
	dumpEnd();
}