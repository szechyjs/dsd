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
// Created by Patrick Eads on 1/15/23.
//
#include "utils.h"

static void onExit(void) {
    isRunning = -1;

    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    fprintf(stderr, "%s\n", "Awaiting quiescence");
    if (frequencyPid) {
        pthread_join(frequencyPid, NULL);
    }
    frequencyPid = 0;
    fprintf(stderr, "%s\n", "Killed update thread");
    int i = 0;
    for (; i < MAX_UCHAR; ++i) {
        pthread_t pid = pids[i];
        if (pid > 0) {
            fprintf(stderr, "Awaiting: %lu\n", pid);
            pthread_join(pid, NULL);
        }
    }
    fprintf(stderr, "%s", "\n");
}

static char *getEnvVarOrDefault(char *name, char *def) {

    char *result = getenv(name);

    if (result) {
        return result;
    }
    return def;
}

static void doExit(MYSQL *con) {

    fprintf(stderr, "Error Message: %s\n", mysql_sqlstate(con));
    fprintf(stderr, "MY_SQL error: %s\n", mysql_error(con));
    if (con != NULL) {
        mysql_close(con);
    }
    exit(-1);
}

static char *writeFrequencyPing(char *frequency, unsigned long nbyte) {
    fprintf(stderr, "%s\n", "UPSERTING FREQUENCY PING");

    int status;
    MYSQL_BIND bind[1];
    MYSQL *conn = mysql_init(NULL);
    mysql_real_connect(conn, db_host, db_user, db_pass, schema, 0, NULL, 0);

    memset(bind, 0, sizeof(bind));

    unsigned long length = sizeof(INSERT_FREQUENCY) / sizeof(*(INSERT_FREQUENCY));
    fprintf(stderr, "writeFrequencyPing :: "  INSERT_FREQUENCY_INFO "\n", frequency);
    
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    mysql_stmt_prepare(stmt, INSERT_FREQUENCY, length);    


    bind[0].buffer_type = MYSQL_TYPE_DECIMAL;
    bind[0].buffer = frequency;
    bind[0].buffer_length = nbyte;
    bind[0].length = &nbyte;
    bind[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, bind);
    if (status != 0) {
        doExit(conn);
    }

    status = mysql_stmt_execute(stmt);
    if (status != 0) {
        doExit(conn);
    }
    mysql_stmt_close(stmt);
    
    length = sizeof(INSERT_HIT) / sizeof(*(INSERT_HIT));
    stmt = mysql_stmt_init(conn); 
    mysql_stmt_prepare(stmt, INSERT_HIT, length);

    //bind[0].buffer_type = MYSQL_TYPE_DECIMAL;
    //bind[0].buffer = frequency;
    //bind[0].buffer_length = nbyte;
    //bind[0].length = &nbyte;
    //bind[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, bind);
    if (status != 0) {
        doExit(conn);
    }

    status = mysql_stmt_execute(stmt);
    if (status != 0) {
        doExit(conn);
    }

    mysql_stmt_close(stmt);

    fprintf(stderr, "%s\n", "Attempting to find the most recently inserted row time");
    length = sizeof(FREQHIT_MOST_RECENT) / sizeof(*FREQHIT_MOST_RECENT);
    status = mysql_real_query(conn, FREQHIT_MOST_RECENT, length);
    if (status != 0) {
        doExit(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row;
    fprintf(stderr, "%s\n", "Reading result");

    mysql_free_result(result);
    mysql_close(conn);

    if (NULL == (row = mysql_fetch_row(result))) {
        return NULL;
    }

    return (char *) row[0];
}

static void writeUpdate(char *hid) {
    
    long hidL = atoll(hid);
    fprintf(stderr, "UPDATING FREQUENCY with id: %ld", hidL);
    int status;
    MYSQL_BIND bind[1];
    MYSQL *conn = mysql_init(NULL);
    mysql_real_connect(conn, db_host, db_user, db_pass, schema, 0, NULL, 0);
    unsigned long length;
    MYSQL_STMT *stmt;

    length = sizeof(NEW_UPDATE) / sizeof(*NEW_UPDATE);
    stmt = mysql_stmt_init(conn);
    mysql_stmt_prepare(stmt, NEW_UPDATE, length);

    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &hidL;
    bind[0].length = 0;
    bind[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, bind);
    if (status != 0) {
        doExit(conn);
    }

    status = mysql_stmt_execute(stmt);
    if (status != 0) {
        doExit(conn);
    }

    mysql_stmt_close(stmt);

    length = sizeof(NEW_UPDATE2) / sizeof(*NEW_UPDATE2);
    status = mysql_real_query(conn, NEW_UPDATE2, length);
    if (status != 0) {
        doExit(conn);
    }

    mysql_close(conn);
}

static long writeInsert(const void *buf, size_t nbyte) {
    fprintf(stderr, "%s\n", "INSERTING DATA");

    int status;
    unsigned long length;
    MYSQL_BIND bind[1];
    MYSQL *conn = mysql_init(NULL);
    
    fprintf(stderr, "%s\n", "writeInsert :: Connecting to database");    
    if (NULL == mysql_real_connect(conn, db_host, db_user, db_pass, schema, 0, NULL, 0)) {
        exit(-1);
    }

    memset(bind, 0, sizeof(bind));

    fprintf(stderr, "%s\n", "writeInsert :: Binding binary data");    
    bind[0].buffer_type = MYSQL_TYPE_BLOB;
    bind[0].buffer = (char *) &buf;
    bind[0].buffer_length = nbyte;
    bind[0].length = &nbyte;
    bind[0].is_null = 0;

    fprintf(stderr, "%s\n", "writeInsert :: Preparing statment");
    length = sizeof(INSERT_DATA) / sizeof(*(INSERT_DATA)); 
    MYSQL_STMT *stmt = mysql_stmt_init(conn); 
    mysql_stmt_prepare(stmt, INSERT_DATA, length);

    status = mysql_stmt_bind_param(stmt, bind);
    if (status != 0) {
        doExit(conn);
    }
    fprintf(stderr, "%s\n", "writeInsert :: Parameters bound");
    status = mysql_stmt_execute(stmt);
    if (status != 0) {
        doExit(conn);
    }
    fprintf(stderr, "%s\n", "writeInsert :: Statment executed. Closing it...");

    mysql_stmt_close(stmt);

    fprintf(stderr, "%s\n", "Attempting to find the most recently inserted row id");    
    length = sizeof(IMBEDATE_MOST_RECENT) / sizeof(*IMBEDATE_MOST_RECENT);
    status = mysql_real_query(conn, IMBEDATE_MOST_RECENT, length);
    if (status != 0) {
        doExit(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row;
    fprintf(stderr, "%s\n", "Reading result");
    if (NULL == (row = mysql_fetch_row(result))) {
        doExit(conn);
    }

    mysql_free_result(result);
    mysql_close(conn);

    return atol(row[0]);
}

static void sshiftLeft(char *s, int n)
{
   char* s2 = s + n;
   while ( *s2 )
   {
      *s = *s2;
      ++s;
      ++s2;
   }
   *s = '\0';
}

static double parseRmsFloat(char *s) {
    unsigned long nbyte = 1 + strchr(strchr(s, ' ') + 1, ' ') - s;
    s[nbyte - 1] = '\0';
    double result = atof(s);

    return result;
}

static void parseFrequency(char *frequency, char *token) {
    char characteristic[7];

    token[6] = '\0';
    unsigned long nbyte = 1 + strchr(token, '\0') - token;

    strcpy(characteristic, token);
    characteristic[(nbyte - 1) / 2] = '\0'; 

    sshiftLeft(token, 3);
    sprintf(frequency, "%s.%s", characteristic, token);
}

static void parseLineData(char *frequency, double *avgRms, double *squelch, char *buffer) {
    int i;
    char *token = strtok(buffer, ",");

    for (i = 0; token != NULL; i++) {
        switch (i) {
            case 0:
                parseFrequency(frequency, token);
                break;
            case 1:
                *avgRms = parseRmsFloat(token);
                break;
            case 4:
                if (*squelch < 0.0) {
                    *squelch = parseRmsFloat(token);
                }
                break;
            case 2:
            case 3:
            default:
                break;                        
        }
        token = strtok(NULL, ",");
    }
}

static void *runUpdateThread(void *ctx) {
    struct updateArgs *args = (struct updateArgs *) ctx;

    pthread_mutex_lock(&mutex);

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    spec.tv_sec += 50;

    if (0 == pthread_cond_timedwait(&cond, &mutex, &spec)) {
        writeUpdate(args->hid);
    }

    pthread_mutex_unlock(&mutex);

    free(args->hid);
    free(args);

    return NULL;
}

static void *runInsertThread(void *ctx) {
    struct insertArgs *args = (struct insertArgs *) ctx;

    pthread_mutex_lock(&mutex);
    
    writeInsert(args->buf, args->nbyte);
    
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    free(args->buf);
    free(args);
    return NULL;
}

static void *runFrequencyUpdatingThread(void *ctx) {

    fprintf(stderr, "%s\n", "Starting rtl_fm read thread");
    char *portname = (char *) ctx;
    char buffer[MAX_UCHAR];
    unsigned long last = strchr(portname, '\0') - portname;
    unsigned long nbyte = 1 + last;
    char filename[nbyte];

    strcpy(filename, portname);
    filename[last] = '\0';

    fprintf(stderr, " %s\n", filename);

    FILE *fd = fopen(filename, "r");
    unsigned char bufSize = 0;
    unsigned char ret;
    char frequency[8];
    double squelch = -1.0;
    double avgRms = 0.0;
    char *hid = NULL;
    double freq;
    double lastFrequency = 0.0;

    fprintf(stderr, "%s\n", "Starting rtl_fm read loop");
    while (!(feof(fd))) {
        if ((ret = fgetc(fd)) != '\n') {
            buffer[bufSize++] = ret;
        } else {
            buffer[bufSize] = '\0';
            bufSize = 0;

            parseLineData(frequency, &avgRms, &squelch, buffer);
            freq = atof(frequency);
            if (freq > 0.0  && avgRms >= squelch) {
                pthread_t pid = 0;

                hid = writeFrequencyPing(frequency, 8);

                fprintf(stderr, "runFrequencyUpdatingThread :: id: %s\n", hid);
                if (freq != lastFrequency) {
                    struct updateArgs *args = malloc(sizeof(struct updateArgs));
                    args->hid = malloc(sizeof(ret));
                    strcpy(args->hid, hid);
                    pid = 0;
                    pthread_create(&pid, NULL, runUpdateThread, args);
                    addPid(pid);
                }
                lastFrequency = freq;
            }
        }
    }

    fprintf(stderr, "%s\n", "Broke rtl_fm read loop");
    fclose(fd);
    isRunning = 0;
    frequencyPid = 0;
    return NULL;
}

void initializeEnv() {

    if (isRunning > 0 && !exitflag) {
        return;
    }

    atexit(onExit);

    db_pass = getenv("DB_PASS");
    if (db_pass) {
        db_host = getEnvVarOrDefault("DB_HOST", "127.0.0.1");
        db_user = getEnvVarOrDefault("DB_USER", "root");
        schema = getEnvVarOrDefault("SCHEMA", "scanner");
    } else {
        fprintf(stderr, "%s\n", "No database user password defined in envionment.");
        exit(-1);
    }

    pthread_t pid = 0;
    char *fileDes = "/home/peads/fm-err-out";
    pthread_create(&pid, NULL, runFrequencyUpdatingThread, (void *) fileDes);
    frequencyPid = pid; // special handling and preservation for update pid
    isRunning = 1;
}

void addData(void *buf, size_t nbyte) {  
    fprintf(stderr, "%s\n", "Checking if environment is initialized");
    initializeEnv();

    fprintf(stderr, "%s\n", "Allocating insert struct"); 
    struct insertArgs *args = malloc(sizeof(struct insertArgs));
    args->buf = buf;
    args->nbyte = nbyte;
    
    fprintf(stderr, "%s\n", "Spawning insert thread");
    pthread_t pid = 0;
    pthread_create(&pid, NULL, runInsertThread, args);
    addPid(pid);
    fprintf(stderr, "%s\n", "Returning from dsd_file::insertData");
}

static void addPid(pthread_t pid) {
    fprintf(stderr, "Added pid: %lu @ index: %d\n", pid, pidCount);
    pids[pidCount++] = pid;
}

