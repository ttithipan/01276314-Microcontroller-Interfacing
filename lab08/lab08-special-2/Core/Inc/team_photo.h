/**
  ******************************************************************************
  * @file    team_photo.h
  * @brief   Geometry of the Screen 2 team photo bitmap.
  ******************************************************************************
  *
  * The pixels live in team_photo.c, which is generated and git-ignored.
  * This header only carries the dimensions, so the layout constants in
  * screen2.c can be checked against the bitmap at compile time.
  */

#ifndef __TEAM_PHOTO_H__
#define __TEAM_PHOTO_H__

#include <stdint.h>

#define TEAM_PHOTO_WIDTH   132U
#define TEAM_PHOTO_HEIGHT  176U

/* Big-endian RGB565, TEAM_PHOTO_WIDTH * TEAM_PHOTO_HEIGHT pixels */
extern const uint8_t team_photo_rgb565[TEAM_PHOTO_WIDTH * TEAM_PHOTO_HEIGHT * 2U];

#endif /* __TEAM_PHOTO_H__ */
