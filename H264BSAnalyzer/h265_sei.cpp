#include "stdafx.h" // for mfc

#include "h265_sei.h"

#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // memset


h265_sei_t* h265_sei_new()
{
    h265_sei_t* s = (h265_sei_t*)malloc(sizeof(h265_sei_t));
    memset(s, 0, sizeof(h265_sei_t));
    s->payload = NULL;
    return s;
}

void h265_sei_free(h265_sei_t* s)
{
    if (s == NULL) return;
    if ( s->payload != NULL ) free(s->payload);
    free(s);
}


void h265_read_sei_end_bits(bs_t* b )
{
    // if the message doesn't end at a byte border
    if ( !bs_byte_aligned( b ) )
    {
        if ( !bs_read_u1( b ) ) fprintf(stderr, "WARNING: bit_equal_to_one is 0!!!!\n");
        while ( ! bs_byte_aligned( b ) )
        {
            if ( bs_read_u1( b ) ) fprintf(stderr, "WARNING: bit_equal_to_zero is 1!!!!\n");
        }
    }

    h265_read_rbsp_trailing_bits(b);
}

// D.1 SEI payload syntax
void h265_read_sei_payload(h265_stream_t* h, bs_t* b, int payloadType, int payloadSize)
{
    h265_sei_t* s = h->sei;

    s->payload = (uint8_t*)malloc(payloadSize);

    int i;

    for ( i = 0; i < payloadSize; i++ )
        s->payload[i] = bs_read_u(b, 8);
        
    h265_read_sei_end_bits(b);
}

#if 0
// D.1 SEI payload syntax
void write_sei_payload(h265_stream_t* h, bs_t* b, int payloadType, int payloadSize)
{
    h265_sei_t* s = h->sei;

    int i;
    for ( i = 0; i < s->payloadSize; i++ )
        bs_write_u(b, 8, s->payload[i]);
}
#endif