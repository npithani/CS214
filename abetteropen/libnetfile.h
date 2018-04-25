//
//  libnetfile.h
//  libnetfiles.c
//
//  Created by Priya Pithani on 4/25/18.
//  Copyright © 2018 Priya Pithani. All rights reserved.
//

#ifndef libnetfile_h
#define libnetfile_h

#include <stdio.h>
#include <unistd.h>

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

#define OPEN 1
#define CLOSE 2
#define READ 3
#define WRITE 4

int state;

int netserverinit(const char *pathname);

#endif /* libnetfile_h */
