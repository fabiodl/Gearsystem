OD=build
TOD=build/turboz
GEAROBJS= $(OD)/Processor.o $(OD)/Memory.o  $(OD)/Cartridge.o $(OD)/MemoryRule.o  $(OD)/opcodes.o $(OD)/opcodes_cb.o $(OD)/opcodes_ed.o
DISOBJS=  $(OD)/z80ex/z80ex_dasm.o
TURBOOBJS= $(TOD)/FlexMemoryRule.o $(TOD)/Symbols.o $(TOD)/Disassembly.o $(TOD)/System.o $(TOD)/Placer.o $(TOD)/ProcessorWindow.o $(TOD)/Spinner.o $(TOD)/ExecutionWindow.o $(TOD)/TDynamicScrollBar.o $(TOD)/TDynamicInputLine.o $(TOD)/ProcessorSymbols.o $(TOD)/GoToDialog.o $(TOD)/DisassemblyWindow.o $(TOD)/turboz.o


OBJS= $(GEAROBJS) $(TURBOOBJS) $(DISOBJS)
CFLAGS= -Wall -g -DDISASM_GEARSYSTEM -I /usr/include/rhtvision -I ../../../src -I ../../../src/z80ex/include
LDFLAGS= -lrhtv -pthread
CPPFLAGS= -std=c++11 

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

clean:
	rm $(OBJS) ./turboz
