/*
 * ul_rbuf.h
 *
 *  Created on: Oct 3, 2011
 *      Author: qwer1
 */

#ifndef UL_RBUF_H_
#define UL_RBUF_H_

//declares ring buffer buf_type with elements type buf_type and count buf_count
#define RBUF_DECLARE( buf_name, buf_type, buf_count) \
    struct { \
        volatile buf_type *rd; \
        volatile buf_type *wr; \
        volatile buf_type buf[buf_count]; \
    } buf_name

//initializes read and write pointers
#define RBUF_INIT( buf_ptr )  { (buf_ptr)->rd = (buf_ptr)->wr = RBUF_START(buf_ptr); }
//returns pointer to first element
#define RBUF_START( buf_ptr ) ( &(buf_ptr)->buf[0] )
//returns pointer to last element
#define RBUF_END( buf_ptr ) ( &(buf_ptr)->buf[ RBUF_COUNT(buf_ptr) - 1 ] )
//returns buffer suze
#define RBUF_SIZE( buf_ptr )  ( sizeof((buf_ptr)->buf) )
//returns elements count
#define RBUF_COUNT( buf_ptr ) ( sizeof((buf_ptr)->buf) / sizeof((buf_ptr)->buf[0]) )
//checks if buffer is empty
#define RBUF_EMPTY( buf_ptr ) ( (buf_ptr)->rd == (buf_ptr)->wr )

//increment read pointer
#define RBUF_INC_RD(buf_ptr) { ((buf_ptr)->rd)++; if ((buf_ptr)->rd > RBUF_END(buf_ptr)) (buf_ptr)->rd = RBUF_START(buf_ptr); }
//read without increment
#define RBUF_GET(buf_ptr) (*((buf_ptr)->rd))
//read with increment
#define RBUF_RD( buf_ptr  ) RBUF_GET(buf_ptr); RBUF_INC_RD(buf_ptr);

//increment write pointer
#define RBUF_INC_WR(buf_ptr) { ((buf_ptr)->wr)++; if ((buf_ptr)->wr > RBUF_END(buf_ptr)) (buf_ptr)->wr = RBUF_START(buf_ptr); }
//write last element without increment
#define RBUF_SET(buf_ptr, data) *((buf_ptr)->wr) = data;
//write to buffer with increment
#define RBUF_WR( buf_ptr, data  ) { RBUF_SET(buf_ptr, data); RBUF_INC_WR(buf_ptr); }
//write string str to buffer, write0==TRUE to write last '0'
#define RBUF_WR_STR( buf_ptr, str, write0 ) { \
	for (U8 *s1=(U8 *)str; *s1!=0; s1++) RBUF_WR( buf_ptr, *s1 ); \
	if (write0) RBUF_WR( buf_ptr, 0 ); \
}

#endif /* UL_RBUF_H_ */
