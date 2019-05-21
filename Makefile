export CC=gcc
export CFLAGS=-Wall -Wextra -std=gnu99 -pthread -g -O0
PWD=$(shell pwd)
export BUILD_DIR=$(PWD)/build

export CLIENT_MODULES=javafx.controls,javafx.fxml


all: controller client


controller: build_dir
	@make -C src/$@


build_dir:
	mkdir -p $(BUILD_DIR)


client: build_dir
	javac --module-path ${PATH_TO_FX} --add-modules javafx.controls,javafx.fxml src/client/mainwindow/*.java -d $(BUILD_DIR)
	cp src/client/mainwindow/*.fxml $(BUILD_DIR)/mainwindow

exec_client:
	java --module-path ${PATH_TO_FX} --add-modules javafx.controls,javafx.fxml -cp build mainwindow.Main


clean:
	@make clean -C src/controller
	rm -rf build/*
	rm -rf *.log*
