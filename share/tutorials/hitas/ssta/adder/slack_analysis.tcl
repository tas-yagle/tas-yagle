#!/usr/bin/env avt_shell

ttv_SetupReport ps

set ofile [fopen "slack_report.log" w]
ssta_SlackReport -display "slacks.ssta" $ofile -storedir store
fclose $ofile

ssta_SlackReport -plot "slacks.ssta" "distrib"
