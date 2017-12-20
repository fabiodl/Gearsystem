OD=build
TOD=build/turboz
TGOD=tgbuild
SRCDIR=../../src
MAPPER=mapper

COREOBJS= $(OD)/Processor.o $(OD)/Memory.o  $(OD)/Cartridge.o $(OD)/MemoryRule.o  $(OD)/opcodes.o $(OD)/opcodes_cb.o $(OD)/opcodes_ed.o
DISOBJS=  $(OD)/z80ex/z80ex_dasm.o
TURBOOBJS= $(TOD)/FlexMemoryRule.o $(TOD)/PhysicalIo.o $(TOD)/Symbols.o $(TOD)/Disassembly.o $(TOD)/System.o $(TOD)/Placer.o $(TOD)/ProcessorWindow.o $(TOD)/Spinner.o $(TOD)/ExecutionWindow.o $(TOD)/HardwareCondition.o $(TOD)/TDynamicScrollBar.o $(TOD)/TDynamicInputLine.o $(TOD)/ProcessorSymbols.o $(TOD)/AddressFinder.o $(TOD)/Breakpoints.o $(TOD)/GoToDialog.o $(TOD)/BreakpointDialog.o $(TOD)/AddressableWindow.o $(TOD)/DisassemblyWindow.o $(TOD)/MemoryWindow.o $(TOD)/TExistingFileDialog.o  $(TOD)/SymbolWindow.o $(TOD)/LoadRamDialog.o $(TOD)/TurboZ.o $(TOD)/FlashMemory.o 
MAPPEROBJS = $(MAPPER)/obj_dir/Vmapper.o $(MAPPER)/obj_dir/Vmapper__Syms.o $(TOD)/sdCard.o

SHAREDOBJS= $(COREOBJS) $(TURBOOBJS) $(DISOBJS) $(MAPPEROBJS)

TZOBJS = $(SHAREDOBJS) $(TOD)/main.o

TGCOREOBJS = $(TGOD)/Audio.o $(TGOD)/audio/Sms_Apu.o $(TGOD)/audio/Sound_Queue.o $(TGOD)/audio/Blip_Buffer.o $(TGOD)/audio/Multi_Buffer.o $(TGOD)/Video.o $(TGOD)/Input.o $(TGOD)/GameGearIOPorts.o 
TGOBJS = $(SHAREDOBJS) $(TGCOREOBJS) $(TGOD)/turbogear/TurboGear.o

CFLAGS= -Wall -g -DDISASM_GEARSYSTEM -I /usr/include/rhtvision -I $(SRCDIR) -I $(SRCDIR)/z80ex/include -I $(SRCDIR)/piIo/ 
CPPFLAGS= -std=c++11 `pkg-config --cflags libconfig++` -I . -I/usr/share/verilator/include
LDFLAGS= -lrhtv -pthread `pkg-config --libs libconfig++` /usr/share/verilator/include/verilated.cpp


TGCFLAGS = $(CFLAGS) -I $(SRCDIR)/turboz
TGCPPFLAGS= $(CPPFLAGS) `pkg-config --cflags sdl2`
TGLDFLAGS= $(LDFLAGS) `pkg-config --libs sdl2`

ifneq ("$(wildcard $(PATH_TO_FILE))","/opt/vc/include/bcm_host.h")
OBJS +=$(OD)/piIo.o
endif

all: turboz

turbogear: $(MAPPER)/obj_dir/Vmapper.h $(TGOBJS)
	g++ -o ./turbogear $(TGOBJS) $(TGLDFLAGS)

turboz: $(MAPPER)/obj_dir/Vmapper.h $(TZOBJS)
	g++ -o ./turboz $(TZOBJS) $(LDFLAGS)


#dependents
-include $(OBJS:%.o=%.d)


$(OD)/%.o: $(SRCDIR)/%.cpp
	g++ $(CFLAGS) $(CPPFLAGS) -g -c -o $@ $<
	g++ -MM $(CFLAGS) $(CPPFLAGS) $< -MF $(@:.o=.d) -MT $@

$(OD)/z80ex/%.o: $(SRCDIR)/z80ex/%.c
	gcc $(CFLAGS) -g -c -o $@ $< -I $(SRCDIR)/z80ex/include
	gcc -MM $(CFLAGS) $< -MF $(@:.o=.d) -MT $@ -I $(SRCDIR)/z80ex/include


$(TGOD)/%.o: $(SRCDIR)/%.cpp
	g++ $(TGCFLAGS) $(TGCPPFLAGS) -g -c -o $@ $<
	g++ -MM $(TGCFLAGS) $(TGCPPFLAGS) $< -MF $(@:.o=.d) -MT $@

$(OD)/piIo.o: $(SRCDIR)/piIo/piIo.c
	gcc $(CFLAGS) -g -c -o $@ $< -I $(SRCDIR)/piIo
	gcc -MM $(CFLAGS) $< -MF $(@:.o=.d) -MT $@ -I $(SRCDIR)/piIo


$(MAPPER)/obj_dir/Vmapper.h: $(MAPPER)/*.v 
	cd mapper;verilator --cc mapper.v

$(MAPPER)/obj_dir/Vmapper.cpp: $(MAPPER)/*.v 
	cd mapper;verilator --cc mapper.v

buildDir:
	mkdir -p build
	mkdir -p build/piIo
	mkdir -p build/z80ex
	mkdir -p build/turboz
	mkdir -p tgbuild/audio
	mkdir -p tgbuild/turbogear
clean:
	rm -r build ./turboz
