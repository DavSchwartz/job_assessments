#include <math.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char * create_queue();
void destroy_queue(unsigned char * q);
void enqueue_byte(unsigned char * q, unsigned char b);
unsigned char dequeue_byte(unsigned char * q);
void on_out_of_memory();
void on_illegal_operation();
unsigned short queueStartIndex(unsigned char queue_num);
unsigned short queueEndIndex(unsigned char queue_num);
void setQueueStartIndex(unsigned char queue_num, unsigned short val);
void setQueueEndIndex(unsigned char queue_num, unsigned short val);
unsigned char allocQueue();
void freeQueue(unsigned char queue_num);
unsigned char allocChunk();
void freeChunk(unsigned char chunk_num);
unsigned char chunkNum(unsigned short index);
unsigned short nextChunkNumIndex(unsigned char chunk_num);
unsigned short chunkIndex(unsigned char chunk_num);
unsigned char queueNum(unsigned short index);
unsigned short queueIndex(unsigned char queue_num);
void init();
int main(int argc, char **argv);

#define ALL_CHUNKS_INDEX 0
#define ALL_CHUNKS_LEN 1848
#define ALL_QUEUES_INDEX 1848
#define ALL_QUEUES_LEN 192
#define AVAIL_QUEUE_NUM_INDEX 2040
#define AVAIL_QUEUE_NUM_LEN 1
#define AVAIL_CHUNK_NUM_INDEX 2041
#define AVAIL_CHUNK_NUM_LEN 1
#define INIT_INDEX 2042
#define CHUNK_LEN 8
#define QUEUE_LEN 3
#define NUM_CHUNKS 231
#define NUM_QUEUES 64
#define DATA_LEN 2048
#define BITS_IN_BYTE 8
#define INVALID_8 0xff
#define INVALID_12 0xfff

/*

{} = number of occurances

data: [chunk{231}: 1848 bytes, queue{64}: 192 bytes, avail_queue_num: 1 bytes, avail_chunk_num: 1 bytes, init: 1 byte, unused: 5 bytes]
 
When not in use, next_chunk_num is the number of the next unused chunk
Chunk number counted from zero
chunk = entry{7}: 7 bytes, next_chunk_num{1}: 1 byte
entry = uint8 data entry: 1 byte
next_chunk_num = uint8 chunk number containing subsequent data: 1 byte

When not in use, first byte of queue is the number of the next unused queue
Queue number counted from zero
queue = queue_start_index{1}: 1.5 bytes, queue_end_index{1}: 1.5 bytes
queue_start_index = uint12 index of start of queue: 1.5 bytes
queue_end_index = uint12 index of end of queue: 1.5 bytes

avail_queue_index = uint16 index of available queue
avail_chunk_index = uint16 index of available chunk

init: uint8 initialization indicator: 0: uninitialized; 1: initialized

*/
static unsigned char data[DATA_LEN] = {0};


unsigned char * create_queue()
{
	if (data[INIT_INDEX] == 0) { init(); }

	return data + queueIndex(allocQueue());

}

void destroy_queue(unsigned char * q)
{
	if (data[INIT_INDEX] == 0)
	{
		printf("DESTROY_BYTE FAILURE\n");
		on_illegal_operation();
	}

	unsigned char queue_num = queueNum(q - data);
	unsigned short start_index = queueStartIndex(queue_num);
	unsigned short end_index = queueEndIndex(queue_num);

	// if queue contains data
	if ((start_index != INVALID_12) && (end_index != INVALID_12))
	{
		unsigned char start_chunk_num = chunkNum(start_index);
		unsigned char end_chunk_num = chunkNum(end_index);
		unsigned char current_chunk_num = start_chunk_num;
		// if free queue data
		while ((current_chunk_num != end_chunk_num))
		{
			unsigned char next_chunk_num = data[nextChunkNumIndex(current_chunk_num)];
			freeChunk(current_chunk_num);
			current_chunk_num = next_chunk_num;
		}
		freeChunk(end_chunk_num);
	}
	freeQueue(queue_num);
}

void enqueue_byte(unsigned char * q, unsigned char b)
{
	if (data[INIT_INDEX] == 0)
	{
		printf("ENQUEUE_BYTE FAILURE\n");
		on_illegal_operation();
	}

	unsigned char queue_num = queueNum(q - data);
	unsigned short start_index = queueStartIndex(queue_num);
	unsigned short end_index = queueEndIndex(queue_num);

	// if no data exists in queue
	if ((start_index == INVALID_12) || (end_index == INVALID_12))
	{
		unsigned char new_chunk_num = allocChunk();
		data[chunkIndex(new_chunk_num)] = b;
		start_index = chunkIndex(new_chunk_num);
		end_index = start_index + 1;
	}
	// if next index represents next_chunk_num
	else if (end_index == nextChunkNumIndex(chunkNum(end_index)))
	{
		unsigned char new_chunk_num = allocChunk();
		data[end_index] = new_chunk_num;
		data[chunkIndex(new_chunk_num)] = b;
		end_index = chunkIndex(new_chunk_num) + 1;
	}
	else
	{
		data[end_index] = b;
		end_index++;
	}

	setQueueStartIndex(queue_num, start_index);
	setQueueEndIndex(queue_num, end_index);
}

unsigned char dequeue_byte(unsigned char * q)
{
	if (data[INIT_INDEX] == 0)
	{
		printf("DEQUEUE_BYTE FAILURE\n");
		on_illegal_operation();
	}

	unsigned char queue_num = queueNum(q - data);
	unsigned short start_index = queueStartIndex(queue_num);
	unsigned short end_index = queueEndIndex(queue_num);

	// if no data exists in queue
	if ((start_index == INVALID_12) || (end_index == INVALID_12))
	{
		printf("DEQUEUE_BYTE FAILURE\n"); 
		on_illegal_operation();
	}

	unsigned char out = data[start_index];
	start_index++;
	// if queue is now empty
	if (start_index == end_index)
	{
		start_index = INVALID_12;
		end_index = INVALID_12;
	}
	// if next index represents next_chunk_num
	else if (start_index == nextChunkNumIndex(chunkNum(start_index)))
	{
		unsigned char chunk_num = chunkNum(start_index);
		start_index = chunkIndex(data[start_index]);
		freeChunk(chunk_num);
	}

	setQueueStartIndex(queue_num, start_index);
	setQueueEndIndex(queue_num, end_index);

	return out;
}

// Out of memory stub, does not return
void on_out_of_memory()
{
	// TODO remove later
	printf("OUT OF MEMORY\n");
	exit(1);
	for (;;) {}
}

// Illegal operation stub, does not return
void on_illegal_operation()
{
	// TODO remove later
	printf("ILLEGAL OPERATION\n");
	exit(1);
	for (;;) {}
}

// get start index of queue, queue is 3 bytes, start index is first 12 bits
unsigned short queueStartIndex(unsigned char queue_num)
{
	unsigned short queue_index = queueIndex(queue_num);

	unsigned char four_least_sig_bits = (data[queue_index + 1] & 0xf0) >> 4;
	unsigned char eight_most_sig_bits = data[queue_index];
	return (eight_most_sig_bits << 4) | four_least_sig_bits;
}

// get end index of queue, queue is 3 bytes, end index last first 12 bits
unsigned short queueEndIndex(unsigned char queue_num)
{
	unsigned short queue_index = queueIndex(queue_num);

	unsigned char four_most_sig_bits = (data[queue_index + 1] & 0x0f);
	unsigned char eight_least_sig_bits = data[queue_index + 2];
	return (four_most_sig_bits << 8) | eight_least_sig_bits;
}

// set start index of queue, queue is 3 bytes, start index is first 12 bits
void setQueueStartIndex(unsigned char queue_num, unsigned short val)
{
	unsigned short queue_index = queueIndex(queue_num);

	unsigned char four_least_sig_bits = val & 0x000f;
	unsigned char eight_most_sig_bits = (val & 0x0ff0) >> 4;
	data[queue_index] = eight_most_sig_bits;
	data[queue_index + 1] = (data[queue_index + 1] & 0x0f) | (four_least_sig_bits << 4);
}

// set end index of queue, queue is 3 bytes, end index last first 12 bits
void setQueueEndIndex(unsigned char queue_num, unsigned short val)
{
	unsigned short queue_index = queueIndex(queue_num);

	unsigned char four_most_sig_bits = (val & 0x0f00) >> 8;
	unsigned char eight_least_sig_bits = (val & 0x0ff);
	data[queue_index + 1] = (data[queue_index + 1] & 0xf0) | four_most_sig_bits;
	data[queue_index + 2] = eight_least_sig_bits;
}

// remove queue from linked list of free queues, return number of queue
unsigned char allocQueue()
{
	unsigned char* avail_queue_num = &data[AVAIL_QUEUE_NUM_INDEX];
	if (*avail_queue_num == INVALID_8)
	{
		printf("ALLOC_QUEUE FAILURE\n");
		on_out_of_memory();
	}

	unsigned char* avail_queue_next_num = &data[queueIndex(*avail_queue_num)];
	unsigned char* next_queue_next_num = &data[queueIndex(*avail_queue_next_num)];
	unsigned char out = *avail_queue_next_num;

	// if avail queue points to itself
	if (*avail_queue_num == *avail_queue_next_num)
	{
		*avail_queue_num = INVALID_8;
	}
	else
	{
		*avail_queue_next_num = *next_queue_next_num;
	}

	setQueueStartIndex(out, INVALID_12);
	setQueueEndIndex(out, INVALID_12);
	
	return out;
}

// add queue to linked list of free queues
void freeQueue(unsigned char queue_num)
{
	unsigned char new_queue_num = queue_num;
	unsigned char* avail_queue_num = &data[AVAIL_QUEUE_NUM_INDEX];
	unsigned char* new_queue_next_num = &data[queueIndex(new_queue_num)];
	if (*avail_queue_num == INVALID_8)
	{
		*avail_queue_num = new_queue_num;
		*new_queue_next_num = new_queue_num;

	}
	else
	{
		unsigned char* avail_queue_next_num = &data[queueIndex(*avail_queue_num)];
		// if avail queue points to itself
		if (*avail_queue_num == *avail_queue_next_num)
		{
			*avail_queue_next_num = new_queue_num;
			*new_queue_next_num = *avail_queue_num;
		}
		else
		{
			*new_queue_next_num = *avail_queue_next_num;
			*avail_queue_next_num = new_queue_num;
		}
	}
}

// remove chunk from linked list of free chunks, return number of chunk
unsigned char allocChunk()
{
	unsigned char* avail_chunk_num = &data[AVAIL_CHUNK_NUM_INDEX];
	if (*avail_chunk_num == INVALID_8)
	{
		printf("ALLOC_CHUNK FAILURE\n");
		on_out_of_memory();
	}

	unsigned char* avail_chunk_next_num = &data[nextChunkNumIndex(*avail_chunk_num)];
	unsigned char* next_chunk_next_num = &data[nextChunkNumIndex(*avail_chunk_next_num)];
	unsigned char out = *avail_chunk_next_num;

	// if avail chunk points to itself
	if (*avail_chunk_num == *avail_chunk_next_num)
	{
		*avail_chunk_num = INVALID_8;
	}
	else
	{
		*avail_chunk_next_num = *next_chunk_next_num;
	}
	
	return out;
}

// add chunk to linked list of free chunks
void freeChunk(unsigned char chunk_num)
{
	unsigned char new_chunk_num = chunk_num;
	unsigned char* avail_chunk_num = &data[AVAIL_CHUNK_NUM_INDEX];
	unsigned char* new_chunk_next_num = &data[nextChunkNumIndex(new_chunk_num)];
	if (*avail_chunk_num == INVALID_8)
	{
		*avail_chunk_num = new_chunk_num;
		*new_chunk_next_num = new_chunk_num;

	}
	else {
		unsigned char* avail_chunk_next_num = &data[nextChunkNumIndex(*avail_chunk_num)];
		// if avail chunk points to itself
		if (*avail_chunk_num == *avail_chunk_next_num)
		{
			*avail_chunk_next_num = new_chunk_num;
			*new_chunk_next_num = *avail_chunk_num;
		}
		else
		{
			*new_chunk_next_num = *avail_chunk_next_num;
			*avail_chunk_next_num = new_chunk_num;
		}
	}
}

// return chunk number of data index
unsigned char chunkNum(unsigned short index)
{
	if ((index < ALL_CHUNKS_INDEX) || (index >= (ALL_CHUNKS_INDEX + ALL_CHUNKS_LEN)))
	{
		printf("CHUNK_NUM FAILURE\n");
		on_illegal_operation();
	}
	
	return floor((index - ALL_CHUNKS_INDEX) / CHUNK_LEN);
}

// return the data index of the byte representing this chunk's next_chunk_num
unsigned short nextChunkNumIndex(unsigned char chunk_num)
{
	return chunkIndex(chunk_num) + (CHUNK_LEN - 1);
}

// return the first data index of the designated chunk
unsigned short chunkIndex(unsigned char chunk_num)
{
	if (chunk_num == INVALID_8)
	{
		printf("CHUNK_INDEX FAILURE\n");
		on_illegal_operation();
	}

	return ALL_CHUNKS_INDEX + (chunk_num * CHUNK_LEN);
}

// return queue number of data index
unsigned char queueNum(unsigned short index)
{
	if ((index < ALL_QUEUES_INDEX) || (index >= (ALL_QUEUES_INDEX + ALL_QUEUES_LEN)))
	{
		printf("CHUNK_NUM FAILURE\n");
		on_illegal_operation();
	}
	
	return floor((index - ALL_QUEUES_INDEX) / QUEUE_LEN);
}

// return the first data index of the designated queue
unsigned short queueIndex(unsigned char queue_num)
{
	if (queue_num == INVALID_8)
	{
		printf("QUEUE_INDEX FAILURE\n");
		on_illegal_operation();
	}

	return ALL_QUEUES_INDEX + (queue_num * QUEUE_LEN);
}

void init()
{
	// Make chunks point to next available chunk
	for (int i = 0; i < (NUM_CHUNKS - 1); i++)
	{
		data[nextChunkNumIndex(i)] = i + 1;
	}
	data[nextChunkNumIndex(NUM_CHUNKS - 1)] = 0;

	// Make queues point to next available queue
	for (int i = 0; i < (NUM_QUEUES - 1); i++)
	{
		data[queueIndex(i)] = i + 1;
	}
	data[queueIndex(NUM_QUEUES - 1)] = 0;

	data[AVAIL_QUEUE_NUM_INDEX] = 0;
	data[AVAIL_CHUNK_NUM_INDEX] = 0;

	data[INIT_INDEX] = 1;
}

int main(int argc, char **argv)
{
	int max_queues = 64;
	int max_chunks = 21;
	unsigned char* q[max_queues];
	unsigned char great = 1;
	for (int i = 0; i < max_queues; i++)
	{
		q[i] = create_queue();
		for (int j = 0; j < max_chunks; j++)
		{
			enqueue_byte(q[i], j);
		}
	}

	for (int i = 0; i < max_queues; i++)
	{
		for (int j = 0; j < max_chunks; j++)
		{
			unsigned char y = dequeue_byte(q[i]);
			if (y != j && great) {great = 0;}
		}
		destroy_queue(q[i]);
	}
	if (great) {printf("MISSION ACCOMPLISHED\n");}

	unsigned char* q0 = create_queue();
	unsigned char x = 0;
	great = 1;
	for (int i = 0; i < 219; i++)
	{
		enqueue_byte(q0, x);
		x = (x+1) % 256;
	}
	x = 0;
	for (int i = 0; i < 50; i++)
	{
		unsigned char y = dequeue_byte(q0);
		if (y != x && great) { great = 0; }
		x = (x+1) % 256;
	}
	printf("end\n");
	destroy_queue(q0);
	printf("destroyed\n");

	if (great) {printf("MISSION ACCOMPLISHED (AGAIN)\n");}

	/*
	unsigned char* q0 = create_queue();
	enqueue_byte(q0, 0);
	enqueue_byte(q0, 1);
	unsigned char* q1 = create_queue();
	enqueue_byte(q1, 3);
	enqueue_byte(q0, 2);
	enqueue_byte(q1, 4);
	printf("%d ", dequeue_byte(q0));
	printf("%d\n", dequeue_byte(q0));
	enqueue_byte(q0, 5);
	enqueue_byte(q1, 6);
	printf("%d ", dequeue_byte(q0));
	printf("%d\n", dequeue_byte(q0));
	destroy_queue(q0);
	printf("%d ", dequeue_byte(q1));
	printf("%d ", dequeue_byte(q1));
	printf("%d\n", dequeue_byte(q1));
	destroy_queue(q1);
	*/

	return 0;
}