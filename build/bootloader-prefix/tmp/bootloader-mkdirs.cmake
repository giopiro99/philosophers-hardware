# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/giopiro/.espressif/v6.0/esp-idf/components/bootloader/subproject"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/tmp"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/src/bootloader-stamp"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/src"
  "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/giopiro/coding/embeddedProgramming/philosophers/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
