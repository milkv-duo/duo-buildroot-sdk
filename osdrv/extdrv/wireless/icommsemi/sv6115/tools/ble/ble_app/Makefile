CC = gcc
CFLAGS = -c -Wall -fshort-enums
LDFLAGS =
CC_INCLUDE := -I./services/api/ssv_netlink -I./services/api -I./cli\
	    -I./services/include/apache-mynewt-nimble-1.2.0/host/bluedroid/api/include \
		-I./services/include/apache-mynewt-nimble-1.2.0/common/include

CLI_SOURCES+=main.c \
	    ./services/api/ssv_netlink/ssv_ble_services.c \
	    ./services/api/ssv_netlink/ssv_netlink.c \
		./services/api/ssv_hal_ble_common.c \
		./services/api/ssv_hal_ble_gap.c \
		./services/api/ssv_hal_ble_gatts.c \
		./services/api/ssv_hal_ble_gattc.c \
		./services/api/ssv_gatt_common_api.c \
		./services/api/ssv_gap_ble_api.c \
		./cli/cli_key.c \
		./cli/cli.c

CLI_OBJECTS=$(CLI_SOURCES:.c=.o)
CLI_EXECUTABLE=ble_app
all:  ble_app

ble_app: $(CLI_SOURCES) $(CLI_EXECUTABLE)

$(CLI_EXECUTABLE): $(CLI_OBJECTS) 
	$(CROSS_COMPILE)$(CC) $(CC_INCLUDE) $(LDFLAGS) $(CLI_OBJECTS) -lpthread -o $(CLI_EXECUTABLE) -ldl

.c.o:
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(CC_INCLUDE) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o ./cli/*.o ./services/api/ssv_netlink/*.o ./services/api/*.o ble_app


