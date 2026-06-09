@echo off
set PATH=C:\Program Files\Beyond Compare 4;%PATH%

bcompare vivado_proj\system.tcl bd\system.tcl
bcompare vivado_proj\vivado_proj.srcs\constrs_1\imports\xdc\system.xdc xdc\system.xdc
