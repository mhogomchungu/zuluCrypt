 /*
 *
 *  Copyright (c) 2011
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EXECUTABLES_H

#define EXECUTABLES_H

#include "bin_path.h"

#define ZULUCRYPTmkfs           "/sbin/mkfs"
#define ZULUCRYPTmount          "/bin/mount"
#define ZULUCRYPTumount         "/bin/umount"
#define ZULUCRYPTmkfs_dir      	"/sbin"
#define ZULUCRYPTgroupadd       "/usr/sbin/groupadd"

#define ZULUCRYPTallPartitions        1
#define ZULUCRYPTsystemPartitions     2
#define ZULUCRYPTnonSystemPartitions  3

#define ZULUCRYPTshortMapperPath 1
#define ZULUCRYPTlongMapperPath  0

#define ZULUCRYPTread  1
#define ZULUCRYPTwrite 0

#define ZULUCRYPTtempFolder     "/run/zuluCrypt"
#define ZULUCRYPTmountPath      "/run/media"
#define ZULUCRYPtmountMiniPath  "/run"

#define ZULUCRYPT_LUKS2_MAX_HEADER_SIZE 33554432
/*
 * The below numbers are cryptsetup default numbers.
 */
#define ZULUCRYPT_KEY_MAX_SIZE      512
#define ZULUCRYPT_KEYFILE_MAX_SIZE  8192000
#define ZULUCRYPT_INT_MAX_KEYSIZE   8192000
#define ZULUCRYPT_CHAR_MAX_KEYSIZE "8192000"

#endif
