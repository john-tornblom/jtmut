#   Copyright (C) 2019 John Törnblom
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING. If not see
# <http://www.gnu.org/licenses/>.


MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR  := $(dir $(MAKEFILE_PATH))


RM      ?= rm
CC      ?= cc
CFLAGS  := -Wall -Wextra -Werror -std=gnu99 -c


SOURCE := $(MAKEFILE_DIR)/jtmut.c
BINARY := $(MAKEFILE_DIR)/jtmut.o


all: $(BINARY)

$(BINARY): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $(SOURCE) $(LDFLAGS)

clean:
	$(RM) -f $(BINARY)
