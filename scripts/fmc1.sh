#/bin/bash

# Main logic from script
case "$1" in
    start)
        echo start
	ssh fmc1 systemctl --user start xppi
	ssh fmc1 systemctl --user start xpopengc
        ;;
    stop)
        echo stop
	ssh fmc1 systemctl --user stop xppi
	ssh fmc1 systemctl --user stop xpopengc
        ;;
    status)
        echo status
	ssh fmc1 systemctl --user status xppi
	ssh fmc1 systemctl --user status xpopengc
        ;;
    restart)
        echo restart
	ssh fmc1 systemctl --user restart xppi
	ssh fmc1 systemctl --user restart xpopengc
        ;;
    shutdown)
	echo shutdown
	ssh fmc1 sudo shutdown now
	;;
  *)
    echo "${RED}Invalid arguments${NC}"
    echo " Usages: $0 ${BLUE}{ start | stop | status | restart | shutdown }${NC}"
    exit 1
esac
exit 0