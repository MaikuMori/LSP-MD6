# Copyright (C) 2011 Miks Kalnins
# This work is licensed under the Creative Commons Attribution 3.0
# Unported License. To view a copy of this license, visit
# http://creativecommons.org/licenses/by/3.0/ or send a letter to
# Creative Commons, 444 Castro Street, Suite 900,
# Mountain View, California, 94041, USA.

from os import system
from string import printable
import re;

OUTPUT_FILE = "bmp_font_data.c"
#Substitution: (max_char_ordinal, max_byte_count)
TMPL_START = "static char bmp_font_data[%d][%d] = {"
#Substitution: (char_ordinal, width, height, byte_count, data)
TMPL_LETTER = "\n [%s] = {\n  %s, %s, %s,%s},"
TMPL_END = "\n};\n"
#Example .xbm file

# #define 0_width 12
# #define 0_height 24
# static char 0_bits[] = {
#   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#   0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0x86, 0x01, 0x86, 0x01, 0x86, 0x01,
#   0x86, 0x01, 0x86, 0x01, 0x86, 0x01, 0x86, 0x01, 0x7E, 0x01, 0x7C, 0x00,
#   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#   };
TMPL_REGEX = ("#define (\d+?)_width (\d+?).#define (.+?) "
              "(\d+?).static char (.+?)\[\] = {(.+?)};")

print("Generating xbm ...")
for letter in printable:
    nr = ord(letter)

    #Skip over letters we don't need.
    if 9 < nr < 14:
        continue

    #Escape some chars.
    if letter == "\\":
        letter = "\\\\"
    elif letter == "\"":
        letter = "\\\""
    elif letter == "`":
        letter = "\`"
    elif letter == "$":
        letter = "\\\$"

    system(('echo -n "%s" | convert -font "MARKEN__.TTF" '
            '-pointsize 8 label:@-  %d.xbm') % (letter, nr))

print("Merging ...")
system("find *.xbm -exec cat {} >> all.tmp \;")

print("Building font file ...")
with open('all.tmp', 'r') as f:
    regex = re.compile(TMPL_REGEX, re.MULTILINE | re.DOTALL)
    r = regex.findall(f.read())

    #Find max byte count and highest character ordinal.
    max_count = 0
    max_size = 0
    for match in r:
        count = match[5].count(",")
        if count > max_count:
            max_count = count

        size = int(match[0])
        if size > max_size:
            max_size = size

    with open(OUTPUT_FILE, 'w') as o:
        o.write(TMPL_START % (max_size + 1, max_count + 3))
        for match in r:
            o.write(TMPL_LETTER % (match[0], match[1], match[3],
                                       match[5].count(","), match[5][:-1]))
        o.write(TMPL_END)

print("Cleaning up ...")
system("rm all.tmp")
system("rm *.xbm")

print("Done!")

