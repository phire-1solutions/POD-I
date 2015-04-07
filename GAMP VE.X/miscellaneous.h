/* 
 * File:   miscellaneous.h
 * Author: Paul
 *
 * Created on 6 April 2015, 3:27 PM
 */

#ifndef MISCELLANEOUS_H
#define	MISCELLANEOUS_H



typedef union {
    struct {
        unsigned loNib  :4;
        unsigned hiNib  :4;
    };
    unsigned char uc;
} convBCD;

unsigned char fromBCD(unsigned char in);
unsigned char toBCD(unsigned char in);


#endif	/* MISCELLANEOUS_H */

