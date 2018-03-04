/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*		ÓÒÈËÈÒÀ ÄËß ÏÎËÓ×ÅÍÈß MD5-ÕİØÀ ÄÀÍÍÛÕ                */
/*                                                                   */
/*********************************************************************/

#define   DCL_INSIDE
#include "dcl_md5.h"

/*-------------------------------------- Ğåàëèçàöèè ñëóæåáíûõ ôóíêöèé -----------------------------*/

void md5_initialization( md5_processor *proc )
{
    proc->total[0] = 0;
    proc->total[1] = 0;

    proc->state[0] = 0x67452301;
    proc->state[1] = 0xEFCDAB89;
    proc->state[2] = 0x98BADCFE;
    proc->state[3] = 0x10325476;
}


void md5_hashing( md5_processor *proc, unsigned char data[64] )
{
    unsigned long X[16], A, B, C, D;

    UCHAR_TO_ULONG( X[ 0], data,  0 );
    UCHAR_TO_ULONG( X[ 1], data,  4 );
    UCHAR_TO_ULONG( X[ 2], data,  8 );
    UCHAR_TO_ULONG( X[ 3], data, 12 );
    UCHAR_TO_ULONG( X[ 4], data, 16 );
    UCHAR_TO_ULONG( X[ 5], data, 20 );
    UCHAR_TO_ULONG( X[ 6], data, 24 );
    UCHAR_TO_ULONG( X[ 7], data, 28 );
    UCHAR_TO_ULONG( X[ 8], data, 32 );
    UCHAR_TO_ULONG( X[ 9], data, 36 );
    UCHAR_TO_ULONG( X[10], data, 40 );
    UCHAR_TO_ULONG( X[11], data, 44 );
    UCHAR_TO_ULONG( X[12], data, 48 );
    UCHAR_TO_ULONG( X[13], data, 52 );
    UCHAR_TO_ULONG( X[14], data, 56 );
    UCHAR_TO_ULONG( X[15], data, 60 );

// Çàïèñûâàåì â A,B,C,D çíà÷åíèÿ ïî óìîë÷àíèş
    A = proc->state[0];
    B = proc->state[1];
    C = proc->state[2];
    D = proc->state[3];

// 1 ıòàï
#define F(x,y,z) (z ^ (x & (y ^ z)))

    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );

#undef F

// 2 ıòàï
#define F(x,y,z) (y ^ (z & (x ^ y)))

    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef F
  
// 3 ıòàï
#define F(x,y,z) (x ^ y ^ z)

    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef F

// 4 ıòàï
#define F(x,y,z) (y ^ (x | ~z))

    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef F

    proc->state[0] += A;
    proc->state[1] += B;
    proc->state[2] += C;
    proc->state[3] += D;
}

void md5_start( md5_processor *proc, unsigned char *input, int len )
{
    int fill;
    unsigned long left;

    if( len <= 0 )
        return;

    left = proc->total[0] & 0x3F;
    fill = 64 - left;

    proc->total[0] += len;
    proc->total[0] &= 0xFFFFFFFF;

    if( proc->total[0] < (unsigned long) len )
        proc->total[1]++;

    if( left && len >= fill )
    {
        memcpy( (void *) (proc->buffer + left), (void *) input, fill );
        md5_hashing( proc, proc->buffer );
        input += fill;
        len  -= fill;
        left = 0;
    }

    while( len >= 64 )
    {
        md5_hashing( proc, input );
        input += 64;
        len  -= 64;
    }

    if( len > 0 )
    {
        memcpy( (void *) (proc->buffer + left), (void *) input, len );
    }
}

void md5_finish( md5_processor *proc, unsigned char *output )
{
    unsigned long last, pad;
    unsigned long high, low;
    unsigned char strlen[8];

    high = ( proc->total[0] >> 29 ) | ( proc->total[1] <<  3 );
    low  = ( proc->total[0] << 3 );

    ULONG_TO_UCHAR( low,  strlen, 0 );
    ULONG_TO_UCHAR( high, strlen, 4 );

    last = proc->total[0] & 0x3F;
    pad = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    md5_start( proc, (unsigned char *) padding, pad );
    md5_start( proc, strlen, 8 );

    ULONG_TO_UCHAR( proc->state[0], output,  0 );
    ULONG_TO_UCHAR( proc->state[1], output,  4 );
    ULONG_TO_UCHAR( proc->state[2], output,  8 );
    ULONG_TO_UCHAR( proc->state[3], output, 12 );
}

void md5_format( unsigned char *input, unsigned char *output, int format )
{
	switch (format)
	{
	case 16:
		{
			unsigned char *digest_hex = new unsigned char[ strlen((char *)input) * 2 + 1 ];
			unsigned char tmp16;
			int j=0;
			for (int i=0; i < strlen((char *)input); i++)
			{
				tmp16 = input[i];
				tmp16>>=4;

				if (int(tmp16) > 9)
					digest_hex[j] = 'a' + int(tmp16) - 10;
				else
					digest_hex[j] = '0' + int(tmp16);

				j++;
				tmp16 = input[i];
				tmp16<<=4; tmp16>>=4;
				if (int(tmp16) > 9)
					digest_hex[j] = 'a' + int(tmp16) - 10;
				else
					digest_hex[j] = '0' + int(tmp16);
				j++;
			}
			digest_hex[j] = 0;	
			memcpy( output, digest_hex, (strlen((char *)digest_hex) + 1) ) ;
		} break;
	case 2:
		{		
			unsigned char *digest_bin = new unsigned char[ strlen((char *)input) * 8 + 1 ];
			unsigned char tmp2[8];
			unsigned int tmp10 = 0;
			int ost = 0;
			int i=0, j=0, n=0;	
			for ( i=0; i < strlen((char *)input); i++ )
			{
				tmp10 = (unsigned int)(input[i]);
				for( j=0; tmp10 > 0; j++ )  
 				{
					ost = tmp10 % 2;
					tmp10 = (tmp10 - ost)/2;
					if ( ost ) 
						tmp2[j] = '1';
					else
						tmp2[j] = '0';
				}
				for( ; j<8; j++ ) { tmp2[j] = '0'; };
				for( j=7; j >= 0; n++, j-- )
				{
					digest_bin[n] = tmp2[j];
				}
			}
			digest_bin[n] = '\0';
			memcpy( output, digest_bin, ( strlen((char *)digest_bin) + 1) ) ;
		} break;
	case 0: 
		{
                    memcpy( output, input, strlen((char *)input) );
                    output[ strlen((char *)input) ] = 0;
		} break;
	}
}

