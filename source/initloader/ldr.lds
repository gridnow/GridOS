/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

ENTRY(_start)

SECTIONS {
    . = 0x110000;

    .text : {*(.text)}
    .rodata : {*(.rodata)}
    .data : {*(.data)}
    .bss : {*(.bss)}
    .comm : {*(.comm)}
}
