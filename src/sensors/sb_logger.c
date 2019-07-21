/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sb_logger.h"

void sb_logger(const char* tag, const char* message)
{
   time_t now;
   time(&now);
   printf("%s [%s]: %s\n", ctime(&now), tag, message);
}
