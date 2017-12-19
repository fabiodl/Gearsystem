OD=build
TOD=build/turboz
MAPPER=mapper

GEAROBJS= $(OD)/Processor.o $(OD)/Memory.o  $(OD)/Cartridge.o $(OD)/MemoryRule.o  $(OD)/opcodes.o $(OD)/opcodes_cb.o $(OD)/opcodes_ed.o
DISOBJS=  $(OD)/z80ex/z80ex_dasm.o
TURBOOBJS= $(TOD)/FlexMemoryRule.o $(TOD)/PhysicalIo.o $(TOD)/Symbols.o $(TOD)/Disassembly.o $(TOD)/System.o $(TOD)/Placer.o $(TOD)/ProcessorWindow.o $(TOD)/Spinner.o $(TOD)/ExecutionWindow.o $(TOD)/HardwareCondition.o $(TOD)/TDynamicScrollBar.o $(TOD)/TDynamicInputLine.o $(TOD)/ProcessorSymbols.o $(TOD)/AddressFinder.o $(TOD)/Breakpoints.o $(TOD)/GoToDialog.o $(TOD)/BreakpointDialog.o $(TOD)/AddressableWindow.o $(TOD)/DisassemblyWindow.o $(TOD)/MemoryWindow.o $(TOD)/TExistingFileDialog.o  $(TOD)/SymbolWindow.o $(TOD)/LoadRamDialog.o $(TOD)/turboz.o
MAPPEROBJS = $(MAPPER)/obj_dir/Vmapper.o $(MAPPER)/obj_dir/Vmapper__Syms.o $(TOD)/sdCard.o

OBJS= $(GEAROBJS) $(TURBOOBJS) $(DISOBJS) $(MAPPEROBJS)
CFLAGS= -Wall -g -DDISASM_GEARSYSTEM -I /usr/include/rhtvision -I ../../../src -I ../../../src/z80ex/include -I ../../../src/piIo/ 
LDFLAGS= -lrhtv -pthread `pkg-config --libs libconfig++` /usr/share/verilator/include/verilated.cpp
CPPFLAGS= -std=c++11 `pkg-config --cflags libconfig++` -I . -I/usr/share/verilator/include


ifneq ("$(wildcard $(PATH_TO_FILE))","/opt/vc/include/bcm_host.h")
OBJS +=$(OD)/piIo.o
endif




turboz: $(OBJS)
	g++ -o ./turboz $(OBJS) $(LDFLAGS)


#dependents
-include $(OBJS:%.o=%.d)


$(OD)/%.o: ../../../src/%.cpp
	g++ $(CFLAGS) $(CPPFLAGS) -g -c -o $@ $<
	g++ -MM $(CFLAGS) $(CPPFLAGS) $< -MF $(@:.o=.d) -MT $@

$(OD)/z80ex/%.o: ../../../src/z80ex/%.c
	gcc $(CFLAGS) -g -c -o $@ $< -I ../../../src/z80ex/include
	gcc -MM $(CFLAGS) $< -MF $(@:.o=.d) -MT $@ -I ../../../src/z80ex/include


$(OD)/piIo.o: ../../../src/piIo/piIo.c
	gcc $(CFLAGS) -g -c -o $@ $< -I ../../../src/piIo
	gcc -MM $(CFLAGS) $< -MF $(@:.o=.d) -MT $@ -I ../../../src/piIo


$(MAPPER)/obj_dir/Vmapper.h: $(MAPPER)/*.v 
	cd mapper;verilator --cc mapper.v

$(MAPPER)/obj_dir/Vmapper.cpp: $(MAPPER)/*.v 
	cd mapper;verilator --cc mapper.v

clean:
	rm $(OBJS) ./turboz
