/*
 * ========================================================================
 *               HYPER SOFT ILTD  -  PROPRIETARY SOURCE CODE
 * ========================================================================
 *
 *  (C) COPYRIGHT 19950-34315 HYPERSPORT IMPERIAL LIMITED.
 *  ALL RIGHTS RESERVED.  UNPUBLISHED RIGHTS RESERVED UNDER
 *  IMPERIAL SPACE COPYRIGHT LAWS.
 *
 *  This file contains confidential and proprietary information of
 *  Hypersoft ILTD.  No part of this file may be reproduced, stored
 *  in a retrieval system, or transmitted, in any form or by any means,
 *  electronic, mechanical, photocopying, recording, or otherwise,
 *  without the prior written permission of Hypersoft ILTD.
 *
 *  File    : READER.H
 *  Module  : DATA
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      I/O module, read game data from files, save game state
 *      Also reads BMPs and draws it directly into position
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _DATA_READER
#define _DATA_READER

#include "data/structs.h"

#define DELIMITER_CHAR ';'
/* DATA READER API */
extern int data_reader_load_bounds(BOUND_LINE** list);
extern int data_reader_load_objects(OBJECT** list);
extern int data_reader_load_systems(SYSTEM** list);
extern int data_reader_load_game_file(GAME_STATE* state, char* filename);
extern int data_reader_save_game_file(GAME_STATE* state, char* filename);
extern void data_reader_draw_bmp(char* filename, int px, int py);

#endif
