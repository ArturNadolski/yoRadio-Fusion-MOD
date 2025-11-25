#ifndef NAMEDAYS_H
#define NAMEDAYS_H

#include "options.h"

#ifdef NAMEDAYS_FILE

#include <Arduino.h>

#pragma once
#include <stddef.h>
#include <stdint.h>

// Egész napi névnaplista visszaadása (vesszőkkel), month=1..12, day=1..31
bool namedays_get_str(uint8_t month, uint8_t day, char* out, size_t outlen);

#endif // NAMEDAYS_FILE

#endif // NAMEDAYS_H
