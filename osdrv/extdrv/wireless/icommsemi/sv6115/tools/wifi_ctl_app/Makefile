CC = gcc
CFLAGS = -c -Wall 
LDFLAGS =
CC_INCLUDE := -I./cli -I./cli/cmds -I./ssv_netlink

CLI_SOURCES+=main.c \
	    app_ctl.c \
		./cli/cli.c \
		./cli/cli_key.c \
		./cli/cmds/cli_cmd.c \
		./ssv_netlink/ssv_netlink.c 
CLI_OBJECTS=$(CLI_SOURCES:.c=.o)
CLI_EXECUTABLE=wifi_ctl_app		
all:  wifi_ctl_apps

wifi_ctl_apps: $(CLI_SOURCES) $(CLI_EXECUTABLE)

$(CLI_EXECUTABLE): $(CLI_OBJECTS) 
	$(CROSS_COMPILE)$(CC) $(CC_INCLUDE) $(LDFLAGS) $(CLI_OBJECTS) -lpthread -o $(CLI_EXECUTABLE) -ldl

.c.o:
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(CC_INCLUDE) $(LDFLAGS) $< -o $@

clean:
	rm *o ./cli/*o ./cli/cmds/*o ./ssv_netlink/*o wifi_ctl_app	


