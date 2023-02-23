CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++

SDIR = $(PWD)
SRCS = $(wildcard $(SDIR)/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = profileSD
EXTRA_CFLAGS += -fPIC -O3

all : $(TARGET)

$(SDIR)/%.o: $(SDIR)/%.c
	@$(CC) $(EXTRA_CFLAGS) -o $@ -c $<
	@echo [$(notdir $(CC))] $(notdir $@)

$(TARGET) : $(OBJS)
	@$(CXX) -o $@ $(OBJS)
	@echo [LINK]$(END)[$(notdir $(CXX))] $(notdir $@)

.PHONY : clean
clean:
	@rm -f $(OBJS) $(TARGET)
