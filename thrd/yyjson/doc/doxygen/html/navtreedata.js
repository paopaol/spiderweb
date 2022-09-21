/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "yyjson", "index.html", [
    [ "Introduction", "index.html", [
      [ "Features", "index.html#autotoc_md1", null ],
      [ "Limitations", "index.html#autotoc_md2", null ],
      [ "Performance", "index.html#autotoc_md3", null ],
      [ "Sample Code", "index.html#autotoc_md7", null ],
      [ "Documentation", "index.html#autotoc_md12", null ],
      [ "TODO", "index.html#autotoc_md13", null ],
      [ "License", "index.html#autotoc_md14", null ]
    ] ],
    [ "Build and Test", "md_doc__build_and_test.html", [
      [ "Source code", "md_doc__build_and_test.html#autotoc_md15", null ],
      [ "Package manager", "md_doc__build_and_test.html#autotoc_md16", null ],
      [ "CMake", "md_doc__build_and_test.html#autotoc_md17", [
        [ "Use CMake to build a library", "md_doc__build_and_test.html#autotoc_md18", null ],
        [ "Use CMake as a dependency", "md_doc__build_and_test.html#autotoc_md19", null ],
        [ "Use CMake to generate project", "md_doc__build_and_test.html#autotoc_md20", null ],
        [ "Use CMake to generate documentation", "md_doc__build_and_test.html#autotoc_md21", null ],
        [ "Testing With CMake and CTest", "md_doc__build_and_test.html#autotoc_md22", null ]
      ] ],
      [ "Compile-time Options", "md_doc__build_and_test.html#autotoc_md23", null ]
    ] ],
    [ "API", "md_doc__a_p_i.html", [
      [ "API Design", "md_doc__a_p_i.html#autotoc_md24", [
        [ "API prefix", "md_doc__a_p_i.html#autotoc_md25", null ],
        [ "API for immutable/mutable data", "md_doc__a_p_i.html#autotoc_md26", null ],
        [ "API for string", "md_doc__a_p_i.html#autotoc_md27", null ]
      ] ],
      [ "Read JSON", "md_doc__a_p_i.html#autotoc_md29", [
        [ "Read JSON from string", "md_doc__a_p_i.html#autotoc_md30", null ],
        [ "Read JSON from file", "md_doc__a_p_i.html#autotoc_md31", null ],
        [ "Read JSON with options", "md_doc__a_p_i.html#autotoc_md32", null ],
        [ "Reader flag", "md_doc__a_p_i.html#autotoc_md33", null ]
      ] ],
      [ "Write JSON", "md_doc__a_p_i.html#autotoc_md35", [
        [ "Write JSON to string", "md_doc__a_p_i.html#autotoc_md36", null ],
        [ "Write JSON to file", "md_doc__a_p_i.html#autotoc_md37", null ],
        [ "Write JSON with options", "md_doc__a_p_i.html#autotoc_md38", null ],
        [ "Writer flag", "md_doc__a_p_i.html#autotoc_md39", null ]
      ] ],
      [ "Access JSON Document", "md_doc__a_p_i.html#autotoc_md41", [
        [ "JSON Document API", "md_doc__a_p_i.html#autotoc_md42", null ],
        [ "JSON Value API", "md_doc__a_p_i.html#autotoc_md43", null ],
        [ "JSON Value Content API", "md_doc__a_p_i.html#autotoc_md44", null ],
        [ "JSON Array API", "md_doc__a_p_i.html#autotoc_md45", null ],
        [ "JSON Array Iterator API", "md_doc__a_p_i.html#autotoc_md46", null ],
        [ "JSON Object API", "md_doc__a_p_i.html#autotoc_md47", null ],
        [ "JSON Object Iterator API", "md_doc__a_p_i.html#autotoc_md48", null ],
        [ "JSON Pointer", "md_doc__a_p_i.html#autotoc_md49", null ]
      ] ],
      [ "Create JSON Document", "md_doc__a_p_i.html#autotoc_md51", [
        [ "Mutable Document API", "md_doc__a_p_i.html#autotoc_md52", null ],
        [ "Mutable JSON Value Creation API", "md_doc__a_p_i.html#autotoc_md53", null ],
        [ "Mutable JSON Array Creation API", "md_doc__a_p_i.html#autotoc_md54", null ],
        [ "Mutable JSON Array Modification API", "md_doc__a_p_i.html#autotoc_md55", null ],
        [ "Mutable JSON Array Modification Convenience API", "md_doc__a_p_i.html#autotoc_md56", null ],
        [ "Mutable JSON Object Creation API", "md_doc__a_p_i.html#autotoc_md57", null ],
        [ "Mutable JSON Object Modification API", "md_doc__a_p_i.html#autotoc_md58", null ],
        [ "Mutable JSON Object Modification Convenience API", "md_doc__a_p_i.html#autotoc_md59", null ],
        [ "JSON Merge Patch", "md_doc__a_p_i.html#autotoc_md60", null ]
      ] ],
      [ "Number Processing", "md_doc__a_p_i.html#autotoc_md62", [
        [ "Number reader", "md_doc__a_p_i.html#autotoc_md63", null ],
        [ "Number writer", "md_doc__a_p_i.html#autotoc_md64", null ]
      ] ],
      [ "Text Processing", "md_doc__a_p_i.html#autotoc_md65", [
        [ "Character Encoding", "md_doc__a_p_i.html#autotoc_md66", null ],
        [ "NUL Character", "md_doc__a_p_i.html#autotoc_md67", null ]
      ] ],
      [ "Memory Allocator", "md_doc__a_p_i.html#autotoc_md68", [
        [ "Single allocator for multiple JSON", "md_doc__a_p_i.html#autotoc_md69", null ],
        [ "Stack memory allocator", "md_doc__a_p_i.html#autotoc_md70", null ],
        [ "Use third-party allocator library", "md_doc__a_p_i.html#autotoc_md71", null ]
      ] ],
      [ "Null Check", "md_doc__a_p_i.html#autotoc_md72", null ],
      [ "Thread Safe", "md_doc__a_p_i.html#autotoc_md73", null ],
      [ "Locale Dependent", "md_doc__a_p_i.html#autotoc_md74", null ]
    ] ],
    [ "Data Structures", "md_doc__data_structure.html", [
      [ "Immutable Value", "md_doc__data_structure.html#autotoc_md76", null ],
      [ "Immutable Document", "md_doc__data_structure.html#autotoc_md77", null ],
      [ "Mutable Value", "md_doc__data_structure.html#autotoc_md79", null ],
      [ "Mutable Document", "md_doc__data_structure.html#autotoc_md80", null ]
    ] ],
    [ "Changelog", "md__c_h_a_n_g_e_l_o_g.html", [
      [ "0.5.1 (2022-06-17)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md82", null ],
      [ "0.5.0 (2022-05-25)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md84", null ],
      [ "0.4.0 (2021-12-12)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md88", null ],
      [ "0.3.0 (2021-05-25)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md92", null ],
      [ "0.2.0 (2020-12-12)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md96", null ],
      [ "0.1.0 (2020-10-26)", "md__c_h_a_n_g_e_l_o_g.html#autotoc_md101", null ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ],
      [ "Data Structures", "annotated.html", [
        [ "Data Structures", "annotated.html", "annotated_dup" ],
        [ "Data Structure Index", "classes.html", null ],
        [ "Data Fields", "functions.html", [
          [ "All", "functions.html", null ],
          [ "Variables", "functions_vars.html", null ]
        ] ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"yyjson_8h.html#a6efba736a610baa629bf2a0b0a41d4a9"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';