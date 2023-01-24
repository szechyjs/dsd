/*
 * This file is part of the dsd_inject distribution (https://github.com/peads/dsd_inject).
 * Copyright (c) 2023 Patrick Eads.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
//
// Created by Patrick Eads on 1/11/23.
//
#ifndef UTILS_H
#define UTILS_H

#include <mysql.h>
#include <pthread.h>
#include <errno.h>
#include <stdatomic.h>
#include "dsd.h"


#define INSERT_FREQUENCY_INFO "INSERT INTO frequencydata (frequency) VALUES (%s);"

#define INSERT_FREQUENCY "insert into frequencydata (`frequency`) values (?) on duplicate key update `date_modified`=NOW();"
#define FREQHIT_MOST_RECENT "SELECT MAX(`id`) from frequencyhit"
#define INSERT_HIT "insert into `frequencyhit` (`frequency`) values (?);"

#define NEW_UPDATE "update `imbedata` set `hid`= ? where `id`=(SELECT MAX(`id`) from `imbedata`);"

#define NEW_UPDATE2 "update `frequencyhit` set `date_decoded`=(" \
    "select `date_decoded` from `imbedata` where id=(" \
        "SELECT MAX(`id`) from `imbedata`" \
    ")" \
");"

//SELECT `date_modified` FROM `frequencydata` ORDER BY ABS(`date_modified` - str_to_date('2023-01-26 06:42:59', '%Y-%m-%d %H:%i:%S')) LIMIT 1;

#define IMBEDATE_MOST_RECENT  "SELECT MAX(`id`) from `imbedata`"
#define INSERT_DATA "INSERT INTO `imbedata` (`data`) VALUES (?);"

#define MAX_UCHAR 256

struct insertArgs {
    void *buf;
    size_t nbyte;
};

struct updateArgs {
    char *hid;
};

const char *db_pass;
const char *db_host;
const char *db_user;
const char *schema;

static pthread_t frequencyPid;
static pthread_t pids[MAX_UCHAR];

static int isRunning = 0;
static unsigned char pidCount = 0;
static pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static void writeUpdate(char *hid);
static char *writeFrequencyPing(char *frequency, unsigned long nbyte);
static void *runFrequencyUpdatingThread(void *ctx);
static void onExit(void);
static char *getEnvVarOrDefault(char *name, char *def);
static void doExit(MYSQL *con);
static long writeInsert(const void *buf, size_t nbyte);
static double parseRmsFloat(char *s);
static void parseFrequency(char *frequency, char *token);
static void parseLineData(char *frequency, double *avgRms, double *squelch, char *buffer);
static void *runUpdateThread(void *ctx);
static void *runInsertThread(void *ctx);
static void addPid(pthread_t pid);

void addData(void *buf, size_t nbyte); 
void initializeEnv();

extern int exitflag;
#endif //UTILS_H

