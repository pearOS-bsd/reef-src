// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Tcl: scripts that go through the parser, the bytecode compiler and executor, the object types (list, dict, string, bignum), regular
// expressions, binary, encodings, zlib, TclOO, channels, files, threads and sockets of the library.
#include <stdio.h>
#include <string.h>
#include "tcl.h"

static const char *scripts[] = {
    "expr {1 + 2 * 3}",
    "expr {2 ** 100}",
    "expr {sqrt(2.0) * 1e10}",
    "expr {int(1e15) % 7919}",
    "set x [list a b {c d} e]; list [llength $x] [lindex $x 2] [lsort -decreasing $x] [lsearch $x e] [lrange $x 1 2]",
    "dict create a 1 b 2 c 3",
    "set d [dict create x 1 y 2]; dict for {k v} $d {append r $k=$v,}; set r",
    "string map {a 1 b 2} abcab",
    "format {%5.2f|%-6s|%04d|%x|%e} 3.14159 ab 42 255 12345.678",
    "scan {12 abc 3.5} {%d %s %f}",
    "regexp -inline -all {([a-z]+)([0-9]+)} {ab12 cd34 ef56}",
    "regsub -all {(a+)} {caaandaa} {<\\1>}",
    "string totitle {hello world}; string is integer 12; string first lo hello",
    "binary encode base64 {hello world}",
    "binary format {Sa3c} 258 xyz 65",
    "binary scan [binary format I 3735928559] H8 h; set h",
    "encoding convertto utf-8 \xc3\xa9",
    "string length [encoding convertfrom iso8859-1 \xe9\xe8]",
    "set s [zlib compress {hello hello hello hello}]; list [string length $s] [zlib decompress $s] [format %08x [zlib crc32 hello]] [format %08x [zlib adler32 hello]]",
    "proc fib n {if {$n < 2} {return $n}; expr {[fib [expr {$n-1}]] + [fib [expr {$n-2}]]}}; fib 20",
    "namespace eval ns {variable v 5; proc get {} {variable v; set v}}; ns::get",
    "set r {}; foreach a {1 2 3} b {x y z} {lappend r $a$b}; set r",
    "catch {error boom} m o; list $m [dict get $o -code]",
    "oo::class create C {variable n; constructor {} {set n 0}; method inc {} {incr n}}; set o [C new]; $o inc; $o inc",
    "array set a {x 1 y 2 z 3}; lsort [array names a]",
    "apply {{x y} {expr {$x * $y}}} 6 7",
    "info patchlevel",
    "info exists tcl_platform",
    "string repeat ab 3",
    "join [lmap x {1 2 3} {expr {$x * $x}}] ,",
    "set s 0; for {set i 0} {$i < 1000} {incr i} {incr s $i}; set s",
    "expr {123456789012345678901234567890 * 987654321098765432109876543210}",
    "expr {[clock scan {2020-02-29 12:00:00} -format {%Y-%m-%d %H:%M:%S} -timezone :UTC]}",
    "clock format 86400 -format {%Y-%m-%d %H:%M:%S %a %b} -timezone :UTC",
    "file join /a/b ../c d.txt; file tail /x/y/z.tcl; file extension a.tar.gz; file rootname a.b.c; file dirname /a/b/c",
    "set f [open /tmp/tclt[pid] w]; puts $f {line one}; puts $f {line two}; close $f; set f [open /tmp/tclt[pid]]; set l [gets $f]; set rest [read $f]; close $f; file delete /tmp/tclt[pid]; list $l [string trim $rest]",
    "exec echo hi from child",
    "interp create i; i eval {set q 7; expr {$q * 6}}",
    "trace add variable v write {lappend log}; set v 1; set v 2; set log",
    "after 10 {set done 1}; vwait done; set done",
    "set s [socket -server {apply {{c a p} {puts $c ok; close $c; set ::acc $a}}} 0]; set port [lindex [fconfigure $s -sockname] 2]; set c [socket 127.0.0.1 $port]; after 1000 {set ::acc timeout}; vwait ::acc; close $c; close $s; set ::acc",
    "glob -nocomplain -directory /dev null",
    "tcl::prefix match {apple banana cherry} ban",
    "lsort -integer -index 1 {{a 3} {b 1} {c 2}}",
    "string cat a b c; string reverse hello; string range abcdef 1 3",
    "subst {[expr {1+1}] \\x41 $tcl_version}",
    "unset -nocomplain nothing; info commands str*",
};

int main(void)
{
    Tcl_Interp *interp = Tcl_CreateInterp();
    if (Tcl_Init(interp) != TCL_OK) printf("init: %s\n", Tcl_GetStringResult(interp));
    Tcl_ResetResult(interp);
    for (size_t i = 0; i < sizeof scripts / sizeof *scripts; i++) {
        int code = Tcl_Eval(interp, scripts[i]);
        const char *r = Tcl_GetStringResult(interp);
        printf("%zu %d %s\n", i, code, r); fflush(stdout);
    }
    Tcl_DeleteInterp(interp);
    Tcl_Finalize();
    return 0;
}
