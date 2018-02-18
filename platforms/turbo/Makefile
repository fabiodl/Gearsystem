OD=build
TOD=build/turboz
TGOD=tgbuild
SRCDIR=../../src
MAPPER=mapper

COREOBJS= $(OD)/Processor.o $(OD)/Memory.o  $(OD)/Cartridge.o $(OD)/MemoryRule.o  $(OD)/opcodes.o $(OD)/opcodes_cb.o $(OD)/opcodes_ed.o
DISOBJS=  $(OD)/z80ex/z80ex_dasm.o
TURBOOBJS= $(TOD)/FlexMemoryRule.o $(TOD)/PhysicalIo.o $(TOD)/Symbols.o $(TOD)/Disassembly.o $(TOD)/System.o $(TOD)/Placer.o $(TOD)/ProcessorWindow.o $(TOD)/Spinner.o $(TOD)/ExecutionWindow.o $(TOD)/HardwareCondition.o $(TOD)/TDynamicScrollBar.o $(TOD)/TDynamicInputLine.o $(TOD)/ProcessorSymbols.o $(TOD)/AddressFinder.o $(TOD)/Breakpoints.o $(TOD)/GoToDialog.o $(TOD)/BreakpointDialog.o $(TOD)/AddressableWindow.o $(TOD)/DisassemblyWindow.o $(TOD)/MemoryWindow.o $(TOD)/TExistingFileDialog.o  $(TOD)/SymbolWindow.o $(TOD)/LoadRamDialog.o $(TOD)/TurboZ.o $(TOD)/FlashMemory.o $(TOD)/Sram.o $(TOD)/sdCard.o
MAPPEROBJS = $(MAPPER)/obj_dir/Vtop.o $(MAPPER)/obj_dir/Vtop__Syms.o 

SHAREDOBJS= $(COREOBJS) $(TURBOOBJS) $(DISOBJS) $(MAPPEROBJS)

TZOBJS = $(SHAREDOBJS) $(TOD)/main.o

TGCOREOBJS = $(TGOD)/Audio.o $(TGOD)/audio/Sms_Apu.o $(TGOD)/audio/Sound_Queue.o $(TGOD)/audio/Blip_Buffer.o $(TGOD)/audio/Multi_Buffer.o $(TGOD)/Video.o $(TGOD)/Input.o $(TGOD)/GameGearIOPorts.o $(TGOD)/SmsIOPorts.o

TGOBJS = $(SHAREDOBJS) $(TGCOREOBJS) $(TGOD)/turbogear/TurboGear.o $(TGOD)/turbogear/VDPView.o $(TGOD)/turbogear/SDLInput.o $(TGOD)/turbogear/FrameBuffer.o $(TGOD)/turbogear/TGSystem.o $(TGOD)/turbogear/InputBuffer.o $(TGOD)/turbogear/main.o $(TGOD)/turbogear/CpldClock.o

VERILATORINCLUDE= /usr/local/share/verilator/include

CFLAGS= -Wall -g -DDISASM_GEARSYSTEM -I /usr/include/rhtvision -I $(SRCDIR) -I $(SRCDIR)/z80ex/include -I $(SRCDIR)/piIo/ 
CPPFLAGS= -std=c++11 `pkg-config --cflags libconfig++` -I . -I $(VERILATORINCLUDE)
LDFLAGS= -lrhtv -pthread `pkg-config --libs libconfig++` $(VERILATORINCLUDE)/verilated.cpp


TGCFLAGS = $(CFLAGS) -I $(SRCDIR)/turboz
TGCPPFLAGS= $(CPPFLAGS) `pkg-config --cflags sdl2`
TGLDFLAGS= $(LDFLAGS) `pkg-config --libs sdl2`

ifneq ("$(wildcard $(PATH_TO_FILE))","/opt/vc/include/bcm_host.h")
OBJS +=$(OD)/piIo.o
endif

all: buildDir turboz turbogear

turbogear: $(MAPPER)/obj_dir/Vtop.h $(TGOBJS)
	g++ -o ./turbogear $(TGOBJS) $(TGLDFLAGS)

turboz: $(MAPPER)/obj_dir/Vtop.h $(TZOBJS)
	g++ -o ./turboz $(TZOBJS) $(LDFLAGS)


#dependents
-include $(TGOBJS:%.o=%.d)
-include $(TZOBJS:%.o=%.d)


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


$(OD)/turboz/FlexMemoryRule.o: $(SRCDIR)/turboz/FlexMemoryRule.cpp $(MAPPER)/obj_dir/VtopPrinter.h
	g++ -MM $(CFLAGS) $(CPPFLAGS) $< -MF $(@:.o=.d) -MT $@
	g++ $(CFLAGS) $(CPPFLAGS) -g -c -o $@ $<

$(MAPPER)/obj_dir/VtopPrinter.h: $(MAPPER)/obj_dir/Vtop.cpp
	python makeDescription.py


$(MAPPER)/obj_dir/Vtop.h: $(MAPPER)/*.v 
	cd mapper;verilator --cc top.v;

$(MAPPER)/obj_dir/Vtop.cpp: $(MAPPER)/*.v 
	cd mapper;verilator --cc top.v

buildDir:
	mkdir -p build
	mkdir -p build/piIo
	mkdir -p build/z80ex
	mkdir -p build/turboz
	mkdir -p tgbuild/audio
	mkdir -p tgbuild/turbogear
clean:
	-rm -rf build tgbuild ./turboz

