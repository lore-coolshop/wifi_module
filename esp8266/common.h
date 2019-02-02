//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef interface_h
#define interface_h

#ifdef __cplusplus
extern "C" {
#endif

typedef int bool;
#define true 1
#define false 0

#define BUFFER_LENGTH 100

#define EQ '='
#define ASK '?'
#define NL '\n'
#define CR '\r'
#define ES '\0'

void strappend(char str[], char* append);
void split(char* data, char separator, char support[][BUFFER_LENGTH]);
bool strstart(char str[], char check[]);
bool streq(char str[], char check[]);
void charremove(char str[], char rem);
void substr(char* string, char buff[], int start, int finish);
void _memcpy(char buff[], char* start, int len);
char _charAt(char* string, int index);
bool isNumber(char* string);
void cleanBidimensionalCharArray(char support[][BUFFER_LENGTH]);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* interface_h */
