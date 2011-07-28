CC=gcc
CFLAGS=-Iinclude/ -ggdb

CXX=g++
CXX_FLAGS=-Iinclude/ -ggdb

DEPS = parser/lex.yy.c

OUT = out
BIN = bin

PARSER_DIR = parser
CFG_DIR = cfg
SSA_DIR = ssa
UTIL_DIR = util
TMP_DIR = tmp

_C_OBJ = ast_pretty_printer.o driver.o  lex.yy.o symtab.o type_checker.o
C_OBJ = $(patsubst %,$(OUT)/%,$(_C_OBJ))

_CXX_OBJ = TAC.o operations.o branch_operations.o cfg.o cfg_gen.o ast_utils.o ssa.o rm_movs.o
CXX_OBJ = $(patsubst %,$(OUT)/%,$(_CXX_OBJ))

all: prepare mongac

# Gerandor o lexer
lex.yy.c: $(PARSER_DIR)/monga.l
	lex $(PARSER_DIR)/monga.l

# Compilando Parser
$(OUT)/%.o: $(PARSER_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compilando CFG
$(OUT)/%.o: $(CFG_DIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

# Compilando SSA
$(OUT)/%.o: $(SSA_DIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

#Compilando o mongac
$(OUT)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

# Linkando mongac
mongac: $(C_OBJ) $(CXX_OBJ) $(OUT)/mongac.o
	$(CXX) -o $(BIN)/$@ $^ $(CXX_FLAGS)

clean:
	rm -f *~ $(PARSER_DIR)/*~ $(CFG_DIR)/*~ $(SSA_DIR)/*~ $(OUT)/*.o $(BIN)/* $(UTIL_DIR)/*~ $(TMP_DIR)/*

prepare:
	mkdir -p $(OUT) $(BIN)

.PHONY: clean
