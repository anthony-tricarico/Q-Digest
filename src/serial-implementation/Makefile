vpath %.c src
vpath %.h include

CC = gcc
CFLAGS = -I include
TESTFLAGS = -I include -DTEST

BUILD_DIR = build
EXEC_DIR = bin

all: $(EXEC_DIR)/main
test-queue: $(EXEC_DIR)/queue
test-linked: $(EXEC_DIR)/linked
test-all: $(EXEC_DIR)/test

.PHONY: clean clean-files-cluster

$(EXEC_DIR)/test: qcore.c dynamic_array.c memory_utils.c | $(EXEC_DIR)
	$(CC) $(TESTFLAGS) $^ -o $@

$(EXEC_DIR)/queue: queue.c memory_utils.c | $(EXEC_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(EXEC_DIR)/linked: linked.c | $(EXEC_DIR)
	$(CC) $(TESTFLAGS) $^ -o $@

$(EXEC_DIR)/%: $(BUILD_DIR)/%.o | $(EXEC_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.c test.h

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

clean:
	rm -rf $(BUILD_DIR) $(EXEC_DIR)

clean-files-cluster:
	rm -rf *sh.*

