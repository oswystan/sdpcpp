#######################################################################
##                     Copyright (C) 2017 wystan
##
##       filename: makefile
##    description:
##        created: 2017-07-06 11:42:13
##         author: wystan
##
#######################################################################
.PHONY: all clean

bin := main
lib := libsdp.a
header := $(wildcard *.h)
src := $(wildcard *.c *.cpp)
obj := $(src:.c=.o)
obj := $(obj:.cpp=.o)
ld_flags := -lcrypto
c_flags := -Wall -Werror -fPIC

all: $(bin) $(lib)

$(lib) : sdp.o
	@echo "[gen] "$@
	@rm -f $@
	@ar -r $@ $^ 2>/dev/null
$(bin): $(obj)
	@g++ $^ -o $(bin) $(ld_flags)
	@strip $@
	@echo "[gen] "$@
%.o:%.c
	@echo "[ cc] "$@
	@gcc $(c_flags) -c $< -o $@
%.o:%.cpp $(header)
	@echo "[cpp] "$@
	@g++ -std=c++11 $(c_flags) -c $< -o $@

clean:
	@echo "cleaning..."
	@rm -f *.o $(bin) $(lib)
	@echo "done."
test: all
	@echo "testing ..."
	@./main
	@echo "done."

#######################################################################
