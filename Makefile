CC=gcc
CFLAGS=-Iinclude/ -ggdb

CXX=g++
CXX_FLAGS=-Iinclude/ -ggdb

DEPS = parser/lex.yy.c
OUT = out
BIN = bin

_C_OBJ = ast_pretty_printer.o driver.o  lex.yy.o symtab.o type_checker.o
C_OBJ = $(patsubst %,$(OUT)/%,$(_C_OBJ))

_CXX_OBJ = cfg.o cfg_gen.o ast_utils.o
CXX_OBJ = $(patsubst %,$(OUT)/%,$(_CXX_OBJ))

PARSER_DIR = parser
CFG_DIR = cfg

all: prepare mongac

# Gerandor o lexer
lex.yy.c: $(PARSER_DIR)/monga.l
	lex $(PARSER_DIR)/monga.l

# Compilando Parser
$(OUT)/%.o: $(PARSER_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compilando CFG
$(OUT)/%.o: cfg/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

#Compilando o mongac
$(OUT)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

# Linkando mongac
mongac: $(C_OBJ) $(CXX_OBJ) $(OUT)/mongac.o
	$(CXX) -o $(BIN)/$@ $^ $(CXX_FLAGS)

clean:
	rm -f *~
	rm -f $(PARSER_DIR)/*~
	rm -f $(CFG_DIR)/*~
	rm -f $(OUT)/*.o
	rm -f mongac

prepare:
	mkdir -p $(OUT) $(BIN)

.PHONY: clean
