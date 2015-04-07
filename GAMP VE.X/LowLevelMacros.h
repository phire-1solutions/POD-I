/* 
 * File:   LowLevelMacros.h
 * Author: phamilt5
 *
 * Created on February 25, 2015, 1:07 PM
 */

#ifndef LOWLEVELMACROS_H
#define	LOWLEVELMACROS_H

#define SUCCESS 1

#define ERROR 0

/************************************************************************
* Macro: Lo                                                             *
* Preconditions: None                                                   *
* Overview: This macro extracts a low byte from a 2 byte word.          *
* Input: None.                                                          *
* Output: None.                                                         *
************************************************************************/
#define Lo(X)   (unsigned char)(X&0x00ff)

/************************************************************************
* Macro: Hi                                                             *
* Preconditions: None                                                   *
* Overview: This macro extracts a high byte from a 2 byte word.         *
* Input: None.                                                          *
* Output: None.                                                         *
*                                                                       *
************************************************************************/
#define Hi(X)   (unsigned char)((X>>8)&0x00ff)


/************************************************************************
* Macro: bitset                                                         *
* Preconditions: None                                                   *
* Overview: Returns true if there is a matching bit between inputs      *
* Input: 2 unsigned                                                     *
* Output: None.                                                         *
*                                                                       *
************************************************************************/
#define bitset(A,B) ((A&B)? 1 : 0)



#endif	/* LOWLEVELMACROS_H */

