/*
 **************************************************************************************
 *       Filename:  log.h
 *    Description:   header file
 *
 *        Version:  1.0
 *        Created:  2017-09-14 21:51:17
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#include <stdio.h>

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_MAX   5

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define logd(fmt, ...) printf("[D/" LOG_TAG "]" fmt "\n", ##__VA_ARGS__);
#else
#define logd(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define logi(fmt, ...) printf("[I/" LOG_TAG "]" fmt "\n", ##__VA_ARGS__);
#else
#define logi(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define logw(fmt, ...) printf("[W/" LOG_TAG "]" fmt "\n", ##__VA_ARGS__);
#else
#define logw(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define loge(fmt, ...) printf("[E/" LOG_TAG "]" fmt "\n", ##__VA_ARGS__);
#else
#define loge(fmt, ...)
#endif

#define log printf
#define logfunc() logd("enter %s(%d)", __FUNCTION__, __LINE__)

#endif /*LOG_H_INCLUDED*/
/********************************** END **********************************************/
