
.PHONY: help all release install check clean eclipse_all

CC := gcc
CXX := g++

# c 编译选项
CFLAGS += -std=c11
# c++ 编译选项
CXXFLAGS += -std=c++0x
#
# 链接器参数(如:"-lld, -lpthread,-lrt 等")
LDFLAGS += -lpthread

DEFS += -D_GNU_SOURCE


#***
TARGET := obj_out
#LIB_TARGET := obj_lib.a
#***


#>>>>>
__assembly += *.s
objects += *.o 
cpp_src += ./src/*.cpp
#<<<<<


help:
	@echo ""
	@echo "passed, please make a choose target: make [target]"
	@echo "[target]::{"
	@echo "help all release install check clean eclipse_all"
	@echo "}"
	@echo "demo:"
	@echo "make all"
	@echo ""
	@echo ""

all:CFLAGS += -ggdb3 -W -pg
	CXXFLAGS += -ggdb3 -W -pg
all:
	@echo ""
	@echo ">>building all src with [-ggdb3 -W -pg]<<"
	# 预编译和编译一起做
	#$(CC) -S $(c_src) $(CFLAGS) $(DEFS)
	$(CXX) -S $(cpp_src) $(CXXFLAGS) $(DEFS)
	# 汇编
	#$(CC) -c $(__assembly)
	$(CXX) -c $(__assembly)
	# 链接
	$(CXX) -o $(TARGET) $(objects) $(LDFLAGS)
	#$(CC) -o $(TARGET) $(objects) $(LDFLAGS)
	@echo ">>make target-all finish<<"
	@echo ""
	@echo ""

release:CFLAGS += -w -O2
	CXXFLAGS += -w -O2
	DEFS += -DNDEBUG 
release:
	@echo ""
	@echo ">>building all src with [-w -O2]<<"
	# 预编译和编译一起做
	#$(CC) -S $(c_src) $(CFLAGS) $(DEFS)
	$(CXX) -S $(cpp_src) $(CXXFLAGS) $(DEFS)
	# 汇编
	#$(CC) -c $(__assembly)
	$(CXX) -c $(__assembly)
	# 链接
	$(CXX) -o $(TARGET) $(objects) $(LDFLAGS)
	#$(CC) -o $(TARGET) $(objects) $(LDFLAGS)
	@echo ">>make target-release finish<<"
	@echo ""
	@echo ""
	
install:
	@echo ""
	@echo ">>install needs root authority<<"
	@echo ""
	@echo ""

check:
	@echo ""
	@echo ">>all source file<<"
	#@echo $(c_src)
	@echo $(cpp_src)
	@echo ""
	@echo ">>checking all src<<"
	#$(CC) -MM $(c_src)
	$(CXX) -MM $(cpp_src)
	@echo ""
	@echo ">>make -E src<<"
	#$(CC) -E $(c_src)
	$(CXX) -E $(cpp_src)
	@echo ""
	@echo ""

clean:
	@echo ""
	rm $(TARGET) $(objects) $(__assembly)
	@echo ">>clean up finish<<"
	@echo ""
	@echo ""


eclipse_all:CFLAGS += -g3 -W
	CXXFLAGS += -g3 -W
eclipse_all:
	@echo ""
	@echo ">>building all src with [-g3 -W]<<"
	# 预编译和编译一起做
	#$(CC) -S $(c_src) $(CFLAGS) $(DEFS)
	$(CXX) -S $(cpp_src) $(CXXFLAGS) $(DEFS)
	# 汇编
	#$(CC) -c $(__assembly)
	$(CXX) -c $(__assembly)
	# 链接
	$(CXX) -o $(TARGET) $(objects) $(LDFLAGS)
	#$(CC) -o $(TARGET) $(objects) $(LDFLAGS)
	@echo ">>make target-all finish<<"
	@echo ""
	@echo ""



