/*
 * stdint.h
 *
 *  Created on: 13/09/2008
 *      Author: Eduardo Casal, Luciano Mangiarotti, Rodrigo Rearden
 */

#ifndef STDINT_H_
#define STDINT_H_

/*Para 32 bits, en un SO host AMD64 compilar con -m32 y linkeditar con -melf_i386*/

/*Signed*/
typedef signed char             int8;
typedef short int               int16;
typedef int                     int32;
typedef long long int           int64;

/* Unsigned.  */
typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned int            uint32;
typedef unsigned long long int  uint64;

#endif /* STDINT_H_ */
