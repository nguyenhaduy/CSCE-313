#!/bin/bash
# READ!!
# Cannot use the ps command (just figured this out) need to parse files in /proc and /proc/[pid]
# Can use these ps commands to check your values
echo "`ps -p $$ -o pid,ppid,euid,egid,ruid,rgid,fsuid,fsgid`"
# Identifiers
echo "=== Identifiers ==="
echo "PID     PPID"
echo $$"  `ps -p $$ -o ppid=`"
echo "EUID    EGID"
echo "`ps -p $$ -o euid=`"" `ps -p $$ -o egid=`"
echo "RUID    RGID"
echo "`ps -p $$ -o ruid=`"" `ps -p $$ -o rgid=`"
echo "FSUID   FSGID"
echo "`ps -p $$ -o fsuid=`"" `ps -p $$ -o fsgid=`"
# State
echo "=== State ==="
STAT="`ps -p $$ -o state=`"
case "$STAT" in
	"R") STAT="Running"
	;;
	"S") STAT="Sleeping"
	;;
	"D") STAT="Disk Sleeping"
	;;
	"T") STAT="Stopped"
	;;
	"Z") STAT="Zombie"
	;;
	"X") STAT="Dead"
	;;
esac
echo "Current state of the `ps -p $$ -o command=` command is `ps -p $$ -o state=` ($STAT)"

# Thread Information
echo "=== Thread Information ==="
echo ""
# Priority
echo "=== Priority ==="
echo "Priority Number:`ps -p $$ -o pri=`"
echo "Nice value:`ps -p $$ -o ni=`"
echo ""
# Time Information
echo "=== Time Information ==="
