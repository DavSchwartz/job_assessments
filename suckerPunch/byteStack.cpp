unsigned char * create_queue();
void destroy_queue(unsigned char * q);
void enqueue_byte(unsigned char * q, unsigned char b);
unsigned char dequeue_byte(unsigned char * q);
void on_out_of_memory();
void on_illegal_operation();
void init();

#define POINTER_SPACING 9
#define DATA_LEN 2048
#define VALID_BIT_LENGTH 228
#define VALID_BIT_INDEX 1818
#define META_DATA_LEN 230
#define INIT_BIT_INDEX 2046
#define AVAIL_DATA_POINTER_INDEX 2047

/*

{} = number of occurances

data: [(data_entry{8}, uint8_pointer){202}, valid_bits{228}, unused, init_bit, avail_data_pointer]

data_entry: unsigned char
uint8_pointer: data index of next data_entry
valid_bits: memory availability indicator; 0: available; 1: in use
init_bit: initialization indicator: 0: uninitialized; 1: initialized
avail_data_pointer: data index available memory

*/
static unsigned char data[DATA_LEN] = {0};


unsigned char * create_queue()
{
	init();
}

void destroy_queue(unsigned char * q)
{
	init();
}

void enqueue_byte(unsigned char * q, unsigned char b)
{
	init();
}

unsigned char dequeue_byte(unsigned char * q)
{
	init();
}

void on_out_of_memory()
{
	
}

void on_illegal_operation()
{

}

void init()
{
	// init bit not set
	if (data[INIT_BIT_INDEX] == 0)
	{
		for (int i = 0; i < (DATA_LEN - META_DATA_LEN); i++)
		{
			// Make pointers point to next available data
			if ((i % POINTER_SPACING) == (POINTER_SPACING - 1))
			{
				data[i] = i+1;
			}
		}

		// point to first available data index
		data[AVAIL_DATA_POINTER_INDEX] = 0;
		// set init bit
		data[INIT_BIT_INDEX] = 1;
	}
}