/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*		������� ��� ��������� MD5-���� ������                */
/*                              (.H)                                 */
/*                                                                   */
/*********************************************************************/

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/*-------------------------------------- ������� ����������� --------------------------------------*/

typedef struct
{
    unsigned long total[2];     /* ���������� ���� */
    unsigned long state[4];     /* ������������� ���������� ���������� */
    unsigned char buffer[64];   /* �������������� ������ */
}
md5_processor;

/* ����������� */
static const unsigned char padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#undef   _MD5_BUFF_SIZE
#define  _MD5_BUFF_SIZE   256

static unsigned char  md5_buff[_MD5_BUFF_SIZE];

/* �������������� unsigned char � unsigned long */
#define UCHAR_TO_ULONG( n,b,i )                   \
{                                                 \
    (n) = ( (unsigned long) (b)[(i)	   ]       )  \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )  \
        | ( (unsigned long) (b)[(i) + 2] << 16 )  \
        | ( (unsigned long) (b)[(i) + 3] << 24 ); \
}

/* �������������� unsigned long � unsigned char */
#define ULONG_TO_UCHAR( n,b,i )                    \
{                                                  \
    (b)[(i)    ] = (unsigned char) ( (n)       );  \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );  \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );  \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );  \
}

/* ����� */
#define S( x,n )	((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

/* ����� ��� ���������� ������� */
#define P( a,b,c,d,k,s,t )      \
{                               \
    a += F( b,c,d ) + X[k] + t;	\
	a = S( a,s ) + b;			\
}


/*-------------------------------------- ��������� ��������� ������� ------------------------------*/

void md5_initialization( md5_processor *proc );

void md5_start( md5_processor *proc, unsigned char *input, int len );

void md5_hashing( md5_processor *proc, unsigned char data[64] );

void md5_finish( md5_processor *proc, unsigned char *output );

void md5_format( unsigned char *input, unsigned char *output, int format );
