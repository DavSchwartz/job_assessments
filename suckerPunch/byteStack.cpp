unsigned char * create_queue();
void destroy_queue(unsigned char * q);
void enqueue_byte(unsigned char * q, unsigned char b);
unsigned char dequeue_byte(unsigned char * q);
void on_out_of_memory();
void on_illegal_operation();
void init();

#define CHUNK_LEN 8
#define TOTAL_LEN 2048
#define DYNAMIC_MEMORY_INDEX 0
#define DYNAMIC_MEMORY_LEN 1815
#define DATA_INDEX 0
#define DATA_LEN 1624
#define QUEUE_INDICES_INDEX 1624
#define QUEUE_INDICES_LEN 192
#define QUEUE_INDEX_LEN 3
#define VALID_BITS_INDEX 1816
#define VALID_BITS_LEN 227
#define AVAIL_QUEUE_INDICES_INDEX 2043
#define AVAIL_QUEUE_INDICES_LEN 2
#define AVAIL_CHUNK_INDICES_INDEX 2035
#define AVAIL_CHUNK_INDICES_LEN 2
#define INIT_INDEX 2048
#define INIT_LEN 1

/*

{} = number of occurances

data: [dynamic_memory{1}: 1815 byte, avail_queue_indices{1}: 2 bytes, avail_chunk_indices{1}: 2 bytes, init{1}: 1 byte]

dynamic_memory = data_chunk{203}: 1624 bytes, queue_indices{64}: 192 bytes

data_chunk length must be divisible by 8
data_chunk = data_entry{7}: 7 bytes, chunk_index{1}: 1 byte
data_entry = uint8 data entry: 1 byte
chunk_pointer = uint8 index of next data_chunk: 1 byte

queue_indices length must be divisible by 3
queue_indices = queue_start_index{1}: 1.5 bytes, queue_end_index{1}: 1.5 bytes
queue_start_index = uint12 index of start of queue: 1.5 bytes
queue_end_index = uint12 index of end of queue: 1.5 bytes

valid_bits = boolean indicator of memory usage: 1 bit; 0: available; 1: in use

avail_queue_indices = prev_avail_index{1}: 1 byte, next_avail_index{1}: 1 byte
avail_chunk_indices = prev_avail_index{1}: 1 byte, next_avail_index{1}: 1 byte
prev_avail_index = uint8 index of available memory, pointing to next_avail_index: 1 byte
next_avail_index = uint8 index of available memory: 1 byte

init: uint8 initialization indicator: 0: uninitialized; 1: initialized

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
	if (data[INIT_INDEX] == 0)
	{
		// Make available chunks point to next available chunk
		for (int i = (CHUNK_LEN - 1); i < (DATA_LEN - 1); i += CHUNK_LEN)
		{
			data[DATA_INDEX + i] = (int)((i + 1) / CHUNK_LEN);
		}
		data[DATA_INDEX + DATA_LEN - 1] = 0;

		// Make available queue index locations point to next available available locations
		for (int i = (QUEUE_INDEX_LEN - 1); i < (QUEUE_INDICES_LEN - QUEUE_INDEX_LEN); i += QUEUE_INDEX_LEN)
		{
			data[QUEUE_INDICES_INDEX + i] = i + QUEUE_INDEX_LEN;
		}
		data[QUEUE_INDICES_INDEX + QUEUE_INDICES_LEN - QUEUE_INDEX_LEN] = 0;

		// point to last and first available index
		data[AVAIL_CHUNK_INDICES_INDEX] = DATA_INDEX + DATA_LEN - 1;
		data[AVAIL_CHUNK_INDICES_INDEX + 1] = DATA_INDEX;
		data[AVAIL_QUEUE_INDICES_INDEX] = QUEUE_INDICES_INDEX + QUEUE_INDICES_LEN - 1;
		data[AVAIL_QUEUE_INDICES_INDEX + 1] = QUEUE_INDICES_INDEX;

		// set init bit
		data[INIT_INDEX] = 1;
	}
}