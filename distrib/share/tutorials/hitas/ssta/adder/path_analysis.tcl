#!/usr/bin/env avt_shell

ttv_SetupReport ps

set ofile [fopen "path_report.log" w]
ssta_PathReport -display "paths.ssta" $ofile -storedir store_paths
fclose $ofile

