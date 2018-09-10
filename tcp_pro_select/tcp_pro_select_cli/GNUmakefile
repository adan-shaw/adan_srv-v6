
# 目标声明
.PHONY: help all release install check clean eclipse_all
#
# 默认宏赋值
CC := gcc
CXX := g++
#AR := ar


# 注意:( -i -I 都是加在编译时<附加的>... -l -L 都是加载<链接时用的> )
# 编译选项[[ CPPFLAGS = C预处理器参数(C 和Fortran 编译器也会用到) ]]


# c 编译选项
CFLAGS += -std=c11
#
# c/c++ 共有(由于CPPFLAGS 是c 和cpp 编译器共有的, 可能会发生混编, 
#           所以弃用, 保留c or cpp 分立的局面...保留机动性)
#CPPFLAGS += -std=c++0x
#
# c++ 编译选项
CXXFLAGS += -std=c++0x
#
# 链接器参数(如:"-lld, -lpthread,-lrt 等")
LDFLAGS += -lpthread



# GNU 版本才能用-pg 属性
DEFS += -D_GNU_SOURCE
#DEFS += -D_POSIX_SOURCE




#***
# 目标文件, := 实际上等于 =, 是一个赋值操作
TARGET := obj_out
#LIB_TARGET := obj_lib.a
#***


#>>>>>
# 注意: 多路径通配*.cpp 实际是安全的, 只是生成的.o 文件, 
# 会全部集中在当前目录下面, 所以你不能再像这样:
# objects += ./no_assert/*.o ./make_project/*.o 定义了, 这是错误的...
# 只需:
__assembly += *.s
objects += *.o 
cpp_src += ./src/*.cpp
#c_src := ./src/c_no_assert/*.c ./src/c_make_project/*.c
# 注意2: main() 所在的文件, 应该放到最后!! 
#       顺序问题也是会有的, 有时候会找不到main() 函数
#       调整一下<文件路径>的顺序即可
#
# 注意3: 你不需要指出所有.h 文件所在的地方, 
#       gcc 会自己根据路径找, 现在的gcc 高级了...
#       所以实际只要-MM 一下, 只要找到了依赖, 
#       现在的编译器肯定可以找到对应的文件,
#       编译出问题同样会报告, 根本小题大造!! 这样已经是最简单的方式了...
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

# 拦截游标, 硬性添加调试选项 -W = -Wextra = 打印一些额外警告信息
# -w 禁止显示所有警告信息
# -Wall 会打开一些很有用的警告选项(-W 的子集)
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

# 追加宏定义-DNDEBUG (一般用来禁用assert())
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

# 这个选项可以是供eclipse 调试, 
# 也可以是跨平台调试, 如freebsd 下面的gcc 编译器, IBM 下的gcc 编译器等...
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
