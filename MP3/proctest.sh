#!/bin/bash
echo "=== Identifiers ==="
echo "== Process ID and Parent Process ID =="
echo "PID: `awk '/^Pid/{print $2}' /proc/$1/status`, PPID: `awk '/^PPid/{print $2}' /proc/$1/status`"
echo "== Effective User and Group ID =="
echo "EUID: `awk '/^Uid/{print $3}' /proc/$1/status`, EGID: `awk '/^Gid/{print $3}' /proc/$1/status`"
echo "== Real User and Group ID =="
echo "RUID: `awk '/^Uid/{print $2}' /proc/$1/status`, RGID: `awk '/^Gid/{print $2}' /proc/$1/status`"
echo "== File System User and Group ID =="
echo "FSUID: `awk '/^Uid/{print $5}' /proc/$1/status`, FSGID: `awk '/^Gid/{print $5}' /proc/$1/status`"
echo ""
echo "=== State ==="
state="`awk '/^State/{print $2}' /proc/$1/status`"
# Add specific description for state
case $state in
	"R") state="Running"
	;;
	"S") state="Sleeping"
	;;
	"D") state="Disk Sleeping"
	;;
	"T") state="Stopped"
	;;
	"Z") state="Zombie"
	;;
	"X") state="Dead"
	;;
esac
echo "State of process: `awk '/^State/{print $2}' /proc/$1/status` ($state)"
echo ""
echo "=== Thread Information ==="
# Print out all directories
echo "== Thread IDs =="
for directory in `ls /proc/$1/task`; do
	echo $directory
done
echo ""
echo "=== Priority ==="
echo "Priority Number: `awk '//{print $18}' /proc/$1/stat`"
echo "Nice Value: `awk '//{print $19}' /proc/$1/stat`"
echo ""
echo "=== Time Information ==="
echo "stime: `awk '//{print $15}' /proc/$1/stat`, utime: `awk '//{print $14}' /proc/$1/stat`"
echo "cstime: `awk '//{print $17}' /proc/$1/stat`, cutime: `awk '//{print $16}' /proc/$1/stat`"
echo ""
echo "=== Address Space ==="
echo "Startcode: `awk '//{print $26}' /proc/$1/stat`, Endcode: `awk '//{print $27}' /proc/$1/stat`"
echo "ESP: `awk '//{print $29}' /proc/$1/stat`, EIP: `awk '//{print $30}' /proc/$1/stat`"
echo ""
echo "=== Resources ==="
echo "== File Handles =="
# Number of file handles
echo "Number of file handles: `ls /proc/$1/fd | wc -l`"
echo "== Context Switches =="
echo "Voluntary context switches: `awk '/^voluntary_ctxt_switches/{print $2}' /proc/$1/status`"
echo "Involuntary context switches: `awk '/^nonvoluntary_ctxt_switches/{print $2}' /proc/$1/status`"
echo ""
echo "=== Processors ==="
echo "== Allowed Processors =="
echo "Allowed processors: `awk '/^Cpus_allowed_list/{print $2}' /proc/$1/status`"
echo "Last processor used: `awk '//{print $39}' /proc/$1/stat`"
echo ""
echo "=== Memory Map ==="
echo "Writing memory map to file with name \"memorymap.txt\"..."
cat /proc/$1/maps > memorymap.txt
echo "Done writing memory map."