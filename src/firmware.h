/**
 * @file firmware.h
 * @author Daniel Polanco (jdanypa@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if !defined(__FIRMWARE_H__)
#define __FIRMWARE_H__

#include <stdint.h>
#include <stdio.h>

/**
 * @brief 
 * 
 */
typedef struct LC3Firmware_t
{
    /**
     * @brief path/filename to the obj file
     * 
     */
    const char *filename;
    /**
     * @brief program size in words
     * 
     */
    uint16_t size;
    /**
     * @brief flag indicating if the firmware has information
     * 
     */
    uint8_t isLoaded;
    /**
     * @brief Address where the program starts
     * 
     */
    uint16_t memOrig;
    /**
     * @brief array memory
     * 
     */
    uint16_t memory[UINT16_MAX];
} LC3Firmware_t;

typedef struct LC3Instruction_t
{
    uint16_t body : 12;
    uint16_t opcode : 4;
} LC3Instruction_t;

/**
 * @brief Loads a objfile from a filename and build a instance of LC3Firmware_t
 * 
 * @param filename path/filename to the objfile
 * @param firmware Instance to build
 * @return uint8_t success flag
 */
extern uint8_t loadFirmwareFromFile(const char *filename, LC3Firmware_t *firmware);

/**
 * @brief Prints fancy program bytes to the log file
 * 
 * @param firmware Firmware instance to dump
 */
extern void dumpFirmware(LC3Firmware_t *firmware);

#endif  // __FIRMWARE_H__
