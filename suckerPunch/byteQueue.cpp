#include <math.h>

unsigned char * create_queue();
void destroy_queue(unsigned char * q);
void enqueue_byte(unsigned char * q, unsigned char b);
unsigned char dequeue_byte(unsigned char * q);
void on_out_of_memory();
void on_illegal_operation();
void setQueueIndicesUsageBit(int queue_indices_index, bool set);
void setUsageBit(int dynamic_mem_index, bool set);
bool isUsageBitSet(int dynamic_mem_index);
unsigned short getQueueStartIndex(unsigned char * q);
unsigned short getQueueEndIndex(unsigned char * q);
void setQueueStartIndex(unsigned char * q, unsigned short val);
void setQueueEndIndex(unsigned char * q, unsigned short val);
unsigned char * getAvailQueueIndex();
void freeQueueIndex(int queue_index);
unsigned short globalChunkIndex(unsigned char chunk_index);
void init();
int main(int argc, char **argv);

#define CHUNK_LEN 8
#define TOTAL_LEN 2048
#define DYNAMIC_MEMORY_INDEX 0
#define DYNAMIC_MEMORY_LEN 1815
#define DATA_INDEX 0
#define DATA_LEN 1624
#define QUEUE_INDICES_INDEX 1624
#define QUEUE_INDICES_LEN 192
#define QUEUE_INDEX_LEN 3
#define USAGE_BITS_INDEX 1816
#define USAGE_BITS_LEN 227
#define AVAIL_QUEUE_INDICES_INDEX 2043
#define AVAIL_QUEUE_INDICES_LEN 2
#define AVAIL_CHUNK_INDICES_INDEX 2035
#define AVAIL_CHUNK_INDICES_LEN 2
#define INIT_INDEX 2048
#define INIT_LEN 1
#define BITS_IN_BYTE 8
#define INVALID 0xff

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

usage_bits = boolean indicator of memory usage: 1 bit; 0: available; 1: in use

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

	return getAvailQueueIndex();
}

void destroy_queue(unsigned char * q)
{
	if (data[INIT_INDEX] == 0)
	{
		on_illegal_operation();
	}

}

void enqueue_byte(unsigned char * q, unsigned char b)
{
	if (data[INIT_INDEX] == 0)
	{
		on_illegal_operation();
	}

	unsigned short start_index = getQueueStartIndex(q);
	unsigned short end_index = getQueueStartIndex(q);
}

unsigned char dequeue_byte(unsigned char * q)
{
	if ((data[INIT_INDEX] == 0) ||
		!isUsageBitSet(q - data - DYNAMIC_MEMORY_INDEX) ||
		!isUsageBitSet(q - data - DYNAMIC_MEMORY_INDEX)
	)
	{
		on_illegal_operation();
	}

	unsigned short start_index = getQueueStartIndex(q);
	unsigned short end_index = getQueueStartIndex(q);

	setUsageBit((start_index - DYNAMIC_MEMORY_INDEX), false);

	if ((start_index % CHUNK_LEN) = (CHUNK_LEN - 1))
	{
		return data[data[start_index - DYNAMIC_MEMORY_INDEX] * CHUNK_LEN];
	}
	else
	{

	}


	return data[start_index - DYNAMIC_MEMORY_INDEX];
}

void on_out_of_memory()
{
	for (;;) {}
}

void on_illegal_operation()
{
	for (;;) {}
}

void setQueueIndicesUsageBit(int queue_indices_index, bool set)
{
	int dynamic_mem_index = QUEUE_INDICES_INDEX - DYNAMIC_MEMORY_INDEX + queue_indices_index;
	setUsageBit(dynamic_mem_index, set);
	setUsageBit(dynamic_mem_index + 1, set);
	setUsageBit(dynamic_mem_index + 2, set);
}

void setUsageBit(int dynamic_mem_index, bool set)
{
	int usage_bit_index = (int)floor(dynamic_mem_index / BITS_IN_BYTE);
	int usage_bit_number = (int)(dynamic_mem_index % BITS_IN_BYTE);
	unsigned char usage_bits = data[USAGE_BITS_INDEX + usage_bit_index];
	if (set)
	{
		unsigned char new_usage_bits = 0x80 >> usage_bit_number;
		data[USAGE_BITS_INDEX + usage_bit_index] = new_usage_bits | new_usage_bits;
	}
	else
	{
		unsigned char new_usage_bits = (unsigned char)(0xff7f >> usage_bit_number);
		data[USAGE_BITS_INDEX + usage_bit_index] = new_usage_bits & new_usage_bits;
	}
}

bool isUsageBitSet(int dynamic_mem_index)
{
	int usage_bit_index = (int)floor(dynamic_mem_index / BITS_IN_BYTE);
	int usage_bit_number = (int)(dynamic_mem_index % BITS_IN_BYTE);
	unsigned char usage_bits = data[USAGE_BITS_INDEX + usage_bit_index];
	unsigned char new_usage_bits = usage_bits >> (BITS_IN_BYTE - 1 - usage_bit_number);
	new_usage_bits = new_usage_bits & 0x01;
	return new_usage_bits;
}

unsigned short getQueueStartIndex(unsigned char * q)
{
	return (*q << 4) | ((*(q + 1) & 0xf0) >> 4);
}

unsigned short getQueueEndIndex(unsigned char * q)
{
	return ((*(q + 1) & 0x0f) << 8) | *(q+2);
}

void setQueueStartIndex(unsigned char * q, unsigned short val)
{
	unsigned char val_4_least_sig = val & 0x000f;
	unsigned char val_8_most_sig = (val & 0x0ff0) >> 4;
	*q = val_8_most_sig;
	*(q + 1) = (val_4_least_sig << 4) | (*(q + 1) & 0x0f);
}

void setQueueEndIndex(unsigned char * q, unsigned short val)
{
	unsigned char val_4_most_sig = (val & 0x0f00) >> 8;
	unsigned char val_8_least_sig = (val & 0x0ff);
	*(q + 1) = (*(q + 1) & 0xf0) | val_4_most_sig;
	*(q + 2) = val_8_least_sig;
}

unsigned char availQueueIndex()
{
	unsigned char* avail_queue_indices_index = &data[AVAIL_QUEUE_INDICES_INDEX];
	if (*avail_queue_indices_index == INVALID)
	{
		on_out_of_memory();
	}

	unsigned char* avail_chunk = &data[globalChunkIndex(*avail_chunk_index)];
	unsigned char* next_avail_chunk = &data[globalChunkIndex(*avail_chunk)];
	unsigned char out = *avail_chunk;

	if (*avail_chunk_index == *avail_chunk)
	{
		*avail_chunk_index = INVALID;
	}
	else
	{
		*avail_chunk = *next_avail_chunk;
	}
	
	return out;
}

void freeQueueIndex(int new_chunk_index)
{
	unsigned char* avail_chunk_index = &data[AVAIL_CHUNK_INDEX];
	unsigned char* new_chunk = &data[globalChunkIndex(new_chunk_index)];
	if (*avail_chunk_index == INVALID)
	{
		*avail_chunk_index = new_chunk_index;
		*new_chunk = new_chunk_index;

	}

	unsigned char* avail_chunk = &data[globalChunkIndex(*avail_chunk_index)];

	if (*avail_chunk_index == *avail_chunk)
	{
		*avail_chunk = new_chunk_index;
		*new_chunk = *avail_chunk_index;
	}
	else
	{
		*new_chunk = *avail_chunk;
		*avail_chunk = new_chunk_index;
	}
}

unsigned char availChunk()
{
	unsigned char* avail_chunk_index = &data[AVAIL_CHUNK_INDEX];
	if (*avail_chunk_index == INVALID)
	{
		on_out_of_memory();
	}

	unsigned char* avail_chunk = &data[globalChunkIndex(*avail_chunk_index)];
	unsigned char* next_avail_chunk = &data[globalChunkIndex(*avail_chunk)];
	unsigned char out = *avail_chunk;

	if (*avail_chunk_index == *avail_chunk)
	{
		*avail_chunk_index = INVALID;
	}
	else
	{
		*avail_chunk = *next_avail_chunk;
	}
	
	return out;
}

void freeChunk(int new_chunk_index)
{
	unsigned char* avail_chunk_index = &data[AVAIL_CHUNK_INDEX];
	unsigned char* new_chunk = &data[globalChunkIndex(new_chunk_index)];
	if (*avail_chunk_index == INVALID)
	{
		*avail_chunk_index = new_chunk_index;
		*new_chunk = new_chunk_index;

	}

	unsigned char* avail_chunk = &data[globalChunkIndex(*avail_chunk_index)];

	if (*avail_chunk_index == *avail_chunk)
	{
		*avail_chunk = new_chunk_index;
		*new_chunk = *avail_chunk_index;
	}
	else
	{
		*new_chunk = *avail_chunk;
		*avail_chunk = new_chunk_index;
	}
}

unsigned short globalChunkIndex(unsigned char chunk_index)
{
	if (chunk_index == INVALID)
	{
		on_illegal_operation();
	}

	return DATA_INDEX + (chunk_index * CHUNK_LEN) + (CHUNK_LEN - 1);
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
		data[AVAIL_CHUNK_INDICES_INDEX] = (DATA_LEN / CHUNK_LEN) - 1;
		data[AVAIL_CHUNK_INDICES_INDEX + 1] = 0;
		data[AVAIL_QUEUE_INDICES_INDEX] = QUEUE_INDICES_LEN - 1;
		data[AVAIL_QUEUE_INDICES_INDEX + 1] = 0;

		// set init bit
		data[INIT_INDEX] = 1;
	}
}

int main(int argc, char **argv)
{
	return 0;
}