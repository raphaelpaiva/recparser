CFG_DIR=`pwd`
DRIVER_DIR=$CFG_DIR/../
OUTPUT_DIR=$CFG_DIR/out
BIN_DIR=$CFG_DIR/bin

if [ ! -d $OUTPUT_DIR ]; then
	mkdir $OUTPUT_DIR
fi

if [ ! -d $BIN_DIR ]; then
	mkdir $BIN_DIR
fi

cd $OUTPUT_DIR

gcc -c $DRIVER_DIR/ast_pretty_printer.c $DRIVER_DIR/lex.yy.c $DRIVER_DIR/driver.c -g
g++ -c $CFG_DIR/cfg.cpp
g++ -c $CFG_DIR/cfg_gen.cpp
g++ $OUTPUT_DIR/*.o -o $BIN_DIR/cfg

cd $CFG_DIR
