/* 
 * File:   errorlog.h
 * Author: Paul
 *
 * Created on 1 March 2015, 10:10 PM
 */

#ifndef ERRORLOG_H
#define	ERRORLOG_H

#ifdef	__cplusplus
extern "C" {
#endif

// 1 byte valkue ie 0 to 256 only available.

#define PGM_ERR                     10


void APP_errorlog(unsigned char);

#ifdef	__cplusplus
}
#endif

#endif	/* ERRORLOG_H */

