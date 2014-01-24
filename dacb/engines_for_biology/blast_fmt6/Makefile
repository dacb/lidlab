# Makefile for MySQL storage engines
# make sure the correct CXXFLAGS are used for 32/64 bit, .e.g -m32 / -m64
# make sure that the correct shared library flags are available, e.g. -bundle for Mac, -share for linux

SRCS = ha_blast_fmt6.cc ha_core.cc 
HDRS = $(SRCS:.cc=.h)

OBJS = $(SRCS:.cc=.o)
SOS  = ha_blast_fmt6.so

CXX = gcc 
CXXFLAGS = -g -O2 -fno-implicit-templates -fno-exceptions -fno-rtti -m64

MYSQL_FLAGS = -DMYSQL_DYNAMIC_PLUGIN 
MYSQL_INC = -I../sql -I../include -I../regex -I../util -I/opt/local/include  
MYSQL_RPATH = /usr/local/mysql/lib/plugin

all: $(SOS)

.cc.o: $(HDRS)
	$(CXX) $(CXXFLAGS) $(MYSQL_FLAGS) $(MYSQL_INC) -fno-common -fPIC -c $< -o $@

ha_blast_fmt6.so: $(OBJS) ../util/transparent_file.o
	$(CXX) -Wl,-flat_namespace -Wl,-undefined -Wl,suppress -bundle -m64 -o $@ $(OBJS) ../util/transparent_file.o

install: $(SOS)
	\cp $(SOS) $(MYSQL_RPATH)

test: install
# test or input_test
# can subsitute @tmp@ for /tmp?
	@echo "To test whether blast_fmt6 installed properly, you can do this by:"
	@echo "Placing a test file in the temp directory:"
	@echo "%rm -rf /tmp/test.blast_fmt6"
	@echo "%cp test.blast_fmt6 /tmp/test.blast_fmt6"
	@echo "Giving the file the proper permissions:"
	@echo "%chmod oug+rwk /tmp/test.blast_fmt6"
	@echo "then running the test sql file:"
	@echo "%mysql /tmp/test.sql" 
	@echo "Then clearing the temp file after the test is finished:"
	@echo "%rm -rf /tmp/test.blast_fmt6"

clean:
	\rm -rf $(OBJS) $(SOS)
