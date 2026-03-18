/*
 * 7_segment_4_digit.h
 *
 *      Author: hardware
 */

#ifndef 7_SEGMENT_4_DIGIT_H_
#define 7_SEGMENT_4_DIGIT_H_

#define  S_A				4
#define  S_B				5
#define  S_C				6
#define  S_D				7
#define  S_E				8
#define  S_F				9
#define  S_G				10
#define  S_DP				11
#define  CA_ALL_SEG_OFF		0x07F0
#define  CA_ALL_SEG			0x07F0
#define  CA_ALL_SEG_OFF_ALT	((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F)|(1<<S_G)|(1<<S_DP))

#define  CA_DIGIT_0			((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F))
#define  CA_DIGIT_1			((1<<S_B)|(1<<S_C))

#define  CK_ALL_SEG			0x07F0
#define  CK_DIGIT_0			((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F))
#define  CK_DIGIT_1			((1<<S_B)|(1<<S_C))

#define  ALL_DISPLAYS		0x0F // Alternative ((1<<0)|(1<<1)|(1<<2)|(1<<3)
#define  D_0				0x01 // Alternative ((1<<0))
#define  D_1				0x02 // Alternative ((1<<1))
#define	 D_2				0x04 // Alternative ((1<<2))
#define  D_3				0x08 // Alternative ((1<<3))




#endif /* 7_SEGMENT_4_DIGIT_H_ */
